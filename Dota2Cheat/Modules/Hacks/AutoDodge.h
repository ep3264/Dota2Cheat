#pragma once
#include "../../CheatSDK/include.h"
#include "../MListeners.h"
namespace Modules {
	inline class AutoDodge : public IFrameListener {
	public:
		void OnFrame() override;
	} AutoDodge;
}