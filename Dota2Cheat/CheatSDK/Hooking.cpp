#include "Hooking.h"

void Hooks::SetUpByteHooks() {
	HOOKFUNC_SIGNATURES(PrepareUnitOrders);
	HOOKFUNC_SIGNATURES(BIsEmoticonUnlocked);
	// HOOKFUNC_SIGNATURES(CDOTAPanoramaMinimapRenderer__Render);
#ifdef _DEBUG
	//HOOKFUNC_SIGNATURES(DispatchPacket);
	HOOKFUNC_SIGNATURES(BAsyncSendProto);
#endif // _DEBUG
	HOOKFUNC_SIGNATURES(SaveSerializedSOCache);
}


void Hooks::SetUpVirtualHooks(bool log) {
	{
		// NetChan constructor
		// vtable ptr at 0x15
		uintptr_t** vtable = SigScan::Find("40 53 56 57 41 56 48 83 EC ? 45 33 F6 48 8D 71", "networksystem.dll").Offset(0x15).GetAbsoluteAddress(3, 7);
		uintptr_t* PostReceivedNetMessage = vtable[86], * SendNetMessage = vtable[69]; // bytehooking through vtables, how's that, Elon Musk?
		HOOKFUNC(PostReceivedNetMessage);
		HOOKFUNC(SendNetMessage);
	}
	{
		// CDOTA_Buff destructor
		// vtable ptr at 0xd
		auto OnRemoveModifier = SigScan::Find("4C 8B DC 56 41 57", "client.dll");
		uintptr_t** vtable = OnRemoveModifier.Offset(0xd).GetAbsoluteAddress(3);
		uintptr_t* OnAddModifier = vtable[VTableIndexes::CDOTA_Buff::OnAddModifier];
		HOOKFUNC(OnAddModifier);
		HOOKFUNC(OnRemoveModifier);
	}
	{
		// xref: "CParticleCollection::~CParticleCollection [%p]\n"
		auto particleDestructor = SigScan::Find("E8 ? ? ? ? 40 F6 C7 01 74 34", "particles.dll")
			.GetAbsoluteAddress(1);
		uintptr_t** vtable = particleDestructor
			.Offset(0x19)
			.GetAbsoluteAddress(3);
		auto SetRenderingEnabled = vtable[VTableIndexes::CParticleCollection::SetRenderingEnabled];
		HOOKFUNC(SetRenderingEnabled);
	}
	{
		auto vmt = VMT(Interfaces::EntitySystem);
		void* OnAddEntity = vmt.GetVM(14), * OnRemoveEntity = vmt.GetVM(15);
		HOOKFUNC(OnAddEntity);
		HOOKFUNC(OnRemoveEntity);
	}
	{
		// RunFrame: xref "CUIEngineSource2::RunFrame", never changes tho
		void* RunScript = Interfaces::UIEngine->GetVFunc(88).ptr,
			* RunFrame = Interfaces::UIEngine->GetVFunc(6).ptr;
		HOOKFUNC(RunFrame);
		HOOKFUNC(RunScript);
	}
}
