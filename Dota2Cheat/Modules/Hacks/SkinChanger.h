#pragma once
#include "../../CheatSDK/include.h"
#include <nlohmann/json.hpp>

// I mean, I tried
// maybe someone else figures it out.
namespace Modules {
	inline
		class SkinChanger {
		std::queue<uint32_t> itemsToCreate;

		CEconItem* CreateItem(uint32_t unDefIndex);
		bool AddItem(uint32_t unDefIndex);

		public:
			struct QueuedEquip {
				uint16_t unClass, unSlot;
			};

			std::map<uint32_t, CEconItem*> FakeItems;
			std::map<uint16_t, std::map<uint16_t, CEconItem*>> EquippedItems;

			uint32_t itemIdCounter = 0x20000000,
				invPosCounter = 0;

			std::map<uint32_t, QueuedEquip> itemsToEquip;

			void DeleteSOCacheFiles() {
				auto dotaPath = std::filesystem::current_path().string() + "\\..\\..\\dota\\";
				if (std::filesystem::exists(dotaPath))
					for (auto& file : std::filesystem::directory_iterator(dotaPath)) {
						auto filePath = file.path().string();
						if (filePath.substr(filePath.size() - 3) == "soc")
							std::filesystem::remove(filePath);
					}
			}
			void ParseItemDefs(std::istream& stream);

			// structure reversed from CEconItem::IsStyleUnlocked
			// xref: "unlocked styles"
			void UnlockAllStyles(CEconItem* pItem);

			void ProcessCreateQueue() {
				while (!itemsToCreate.empty()) {
					auto& item = itemsToCreate.front();

					AddItem(item);

					itemsToCreate.pop();
				}
			}
			// Call in main thread
			void Equip(CEconItem* pItem, uint16_t unClass, uint16_t unSlot);
			// Call in main thread
			void Unequip(CEconItem* pItem) {
				auto& classEquips = EquippedItems[pItem->Class()];

				classEquips.erase(pItem->Slot());
				if (classEquips.size() == 0)
					EquippedItems.erase(pItem->Class());

				pItem->Class() = 0;
				pItem->Slot() = static_cast<uint16_t>(-1);
				pItem->Flag() = 2;

				SOUpdated(pItem);
			}

			void QueueAddItem(uint32_t unDefIndex) {
				itemsToCreate.push(unDefIndex);
			}

			CEconItem* GetItemByDefIndex(uint32_t unDefIndex) {
				for (const auto& [id, item] : FakeItems) {
					if (item->m_unDefIndex == unDefIndex) return item;
				}
				return nullptr;
			}

			void SOUpdated(CEconItem* item) {
				auto inv = CGCClient::Get()->GetSOListeners()[1];
				auto soid = inv->GetSOCache()->GetOwner();
				inv->SOUpdated(&soid, item, eSOCacheEvent_Incremental);
			}
	} SkinChanger;
}