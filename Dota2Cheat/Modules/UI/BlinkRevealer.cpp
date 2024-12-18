#include "BlinkRevealer.h"
#include <usermessages.pb.h>
#include "../../CheatSDK/include.h"

using namespace std::literals;

void Modules::M_BlinkRevealer::Draw() {
	if (!Config::BlinkRevealer)
		return;
	MTM_LOCK;

	static constexpr ImVec2 iconSize{ 48,48 };
	for (auto& [hero, data] : Blinks) {
		if (!hero->IsDormant())
			continue;
		auto dl = ImGui::GetBackgroundDrawList();

		auto icon = assets.spellIcons.Load(data.qop ? "queenofpain_blink" : "antimage_blink");

		auto fade = int(data.fadeCounter / data.fadeTime * 255);
		auto drawPos = WorldToScreen(data.pos);
		dl->AddCircle(
			drawPos,
			iconSize.x / 2 + 2,
			ImColor{ 0,0,0, fade },
			0,
			2
		);
		dl->AddImageRounded(
			icon,
			drawPos - iconSize / 2,
			drawPos + iconSize / 2,
			{ 0,0 },
			{ 1,1 },
			ImColor{ 255, 255, 255, fade },
			iconSize.x / 2);
	}
}

void Modules::M_BlinkRevealer::OnFrame() {
	MTM_LOCK;

	auto timeDelta = CGameRules::Get()->GetGameTime() - lastTime;
	lastTime = CGameRules::Get()->GetGameTime();
	for (auto it = Blinks.begin(); it != Blinks.end();) {
		auto& data = it->second;
		data.fadeCounter -= timeDelta;
		if (data.fadeCounter <= 0)
			it = Blinks.erase(it);
		else
			++it;
	}
}

void Modules::M_BlinkRevealer::OnReceivedMsg(NetMessageHandle_t* msgHandle, google::protobuf::Message* msg) {
	if (!ctx.localHero)
		return;

	if (msgHandle->messageID != UM_ParticleManager)
		return;

	MTM_LOCK;
	auto pmMsg = reinterpret_cast<CUserMsg_ParticleManager*>(msg);
	auto msgIndex = pmMsg->index();
	switch (pmMsg->type()) {
	case GAME_PARTICLE_MANAGER_EVENT_CREATE: {
		auto particle = pmMsg->create_particle();
		if (!particle.has_particle_name_index())
			break;

		const auto szParticleName = CResourceSystem::Get()->GetResourceName(particle.particle_name_index());
		if (!szParticleName)
			break;

		auto ent = CEntSys::Get()->GetEntity(NH2IDX(particle.entity_handle()));
		if (!ent)
			ent = CEntSys::Get()->GetEntity(NH2IDX(particle.entity_handle_for_modifiers()));
		if (!ent || ent->IsSameTeam(ctx.localHero))
			break;


		std::string_view particleName = szParticleName;
		auto am = "particles/units/heroes/hero_antimage/antimage_blink_end.vpcf"sv,
			qop = "particles/units/heroes/hero_queenofpain/queen_blink_end.vpcf"sv;

		if (particleName == qop)
			Blinks[ent].qop = true;
		else if (particleName != am) // if it's neither QoP nor AM, then it's not a blink
			Blinks.erase(ent);

		break;
	}
	case GAME_PARTICLE_MANAGER_EVENT_UPDATE_ENT: {

		if (pmMsg->update_particle_ent().control_point() == 1) {
			auto pos = pmMsg->update_particle_ent().fallback_position();
			auto hero = CEntSys::Get()->GetEntity(NH2IDX(pmMsg->update_particle_ent().entity_handle()));
			if (Blinks.contains(hero))
			{
				auto& data = Blinks[hero];
				data.pos = pos;
				data.fadeCounter = data.fadeTime = 3;
			}
		}
		break;
	}
	};
}
