#pragma once

#include "Papyrus/NativeSync.h"

namespace Papyrus
{
	bool RegisterFunctions(RE::BSScript::IVirtualMachine* a_VM)
	{
		a_VM->BindNativeMethod(NativeSync::SCRIPT_NAME, "PlaySyncedIdles", NativeSync::PlaySyncedIdles, true);
		a_VM->BindNativeMethod(NativeSync::SCRIPT_NAME, "AddItem", NativeSync::AddItem, false);
		a_VM->BindNativeMethod(NativeSync::SCRIPT_NAME, "RemoveItem", NativeSync::RemoveItem, false);
		a_VM->BindNativeMethod(NativeSync::SCRIPT_NAME, "ResetActorAnimState", NativeSync::ResetActorAnimState, false);

		return true;
	}
}
