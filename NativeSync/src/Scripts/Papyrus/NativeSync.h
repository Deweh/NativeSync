#pragma once
#include "RE/BGSAnimationSystemUtils.h"
#include <unordered_set>

namespace Papyrus::NativeSync
{
	constexpr auto SCRIPT_NAME{ "NativeSync"sv };

	const std::unordered_set<std::string> resetRaces = { "AT_TentacleRace", "MirelurkRace" };

	bool AddItem(std::monostate, RE::TESObjectREFR* container, RE::TESForm* obj, int count) {
		if (!container || !obj || count < 1) {
			return false;
		}

		auto boundObj = obj->As<RE::TESBoundObject>();

		if (!boundObj) {
			return false;
		}

		container->AddInventoryItem(boundObj, nullptr, count, nullptr, nullptr, nullptr);
		return true;
	}

	bool RemoveItem(std::monostate, RE::TESObjectREFR* container, RE::TESForm* obj, int count, RE::TESObjectREFR* otherContainer) {
		if (!container || !container->inventoryList || !obj || count < 1) {
			return false;
		}

		RE::TESObjectREFR::RemoveItemData removeData(obj, count);
		removeData.a_otherContainer = otherContainer;
		container->RemoveItem(removeData);
		return true;
	}

	bool PlaySyncedIdles(std::monostate, std::vector<RE::TESIdleForm*> a_idles, std::vector<RE::Actor*> a_actors)
	{
		size_t arrSize = a_actors.size();

		if (arrSize != a_idles.size()) {
			logger::warn(FMT_STRING("PlaySyncedIdles(): Actor array of size {:s} does not match idle array size. No idles will be played with this function call."), std::to_string(arrSize));
			return false;
		}

		for (auto& i : a_idles) {
			if (i == nullptr) {
				logger::warn(FMT_STRING("PlaySyncedIdles(): One or more idles in the array are invalid. No idles will be played with this function call."));
				return false;
			}
		}

		for (auto& a : a_actors) {
			if (a == nullptr || a->currentProcess == nullptr) {
				logger::warn(FMT_STRING("PlaySyncedIdles(): One or more actors in the array are invalid or have no AI process. No idles will be played with this function call."), std::to_string(arrSize));
				return false;
			}
		}
		
		for (size_t i = 0; i < arrSize; i++) {
			auto& a = a_actors[i];
			a->currentProcess->PlayIdle(a, (int)RE::DEFAULT_OBJECT::kActionIdle, a_idles[i], false);
		}

		auto idlesCopy = new std::vector<RE::TESIdleForm*>(a_idles);
		auto actorHandles = new std::vector<RE::ActorHandle>();
		actorHandles->reserve(a_actors.size());

		for (auto& a : a_actors) {
			actorHandles->push_back(a->GetActorHandle());
		}

		std::thread([actorHandles = std::move(actorHandles), idlesCopy = std::move(idlesCopy)]() {
			bool oneLoading = false;

			do {
				oneLoading = false;
				for (size_t i = 0; i < actorHandles->size(); i++) {
					auto a = (*actorHandles)[i].get().get();
					if (a != nullptr && RE::BGSAnimationSystemUtils::IsIdleLoading(a, (*idlesCopy)[i]->animFileName)) {
						oneLoading = true;
						break;
					}
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
			} while (oneLoading);

			F4SE::GetTaskInterface()->AddTask([actorHandles = std::move(actorHandles), idlesCopy = std::move(idlesCopy)]() {
				for (size_t i = 0; i < actorHandles->size(); i++) {
					auto a = (*actorHandles)[i].get().get();
					if (a != nullptr && a->currentProcess != nullptr) {
						if (a->race != nullptr && resetRaces.contains(std::string(a->race->formEditorID))) {
							//a->currentProcess->StopCurrentIdle(a);
							a->NotifyAnimationGraphImpl("g_defaultState");
						}
						a->currentProcess->PlayIdle(a, (int)RE::DEFAULT_OBJECT::kActionIdle, (*idlesCopy)[i], false);
					}
				}
				
				delete idlesCopy;
				delete actorHandles;
			});
			
		}).detach();

		return true;
	}

	bool ResetActorAnimState(std::monostate, RE::Actor* a_actor) {
		if (a_actor != nullptr && a_actor->currentProcess != nullptr) {
			a_actor->currentProcess->StopCurrentIdle(a_actor);
			return true;
		} else {
			return false;
		}
	}
}
