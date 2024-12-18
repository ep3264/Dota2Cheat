#pragma once
#include "../../CheatSDK/include.h"

namespace Modules {
	inline class M_TrueSightESP {
		std::map<CDOTAModifier*, ParticleWrapper> TrackedModifiers{};
	public:
		void OnModifierRemoved(CDOTAModifier* modifier);

		void OnModifierAdded(CDOTAModifier* modifier);

		void OnDisabled() {
			for (auto& [_, wrap] : TrackedModifiers) {
				CParticleMgr::Get()->DestroyParticle(wrap);
			}
		}
	} TrueSightESP{};
}