#pragma once
#include "../../CheatSDK/include.h"
#include "../MListeners.h"

namespace Modules {
	inline class M_ParticleGarbageCollector : public IFrameListener {
		private:
			struct ParticleGCInfo {
				float assignTime;
				float dieTime{};
				ParticleWrapper particleWrap;
			};
			std::vector<ParticleGCInfo> gcInfo;
		public:

			void SetDieTime(ParticleWrapper particle, float dieTime) {
				gcInfo.push_back(
					ParticleGCInfo{
						.assignTime = CGameRules::Get()->GetGameTime(),
						.dieTime = dieTime,
						.particleWrap = particle
					}
				);
			}
			void RemoveFromGC(ParticleWrapper particle);
			void OnFrame();
			void Reset() {
				gcInfo.clear();
			}
	} ParticleGC;
}