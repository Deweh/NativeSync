#pragma once

namespace RE
{
	namespace BGSAnimationSystemUtils
	{
		inline bool InitializeActorInstant(RE::Actor* a_actor, bool a_update)
		{
			using func_t = decltype(&InitializeActorInstant);
			REL::Relocation<func_t> func{ REL::ID(672857) };
			return func(a_actor, a_update);
		}

		inline bool IsIdleLoading(RE::TESObjectREFR* a_ref, const BSFixedString& a_idl)
		{
			using func_t = decltype(&IsIdleLoading);
			REL::Relocation<func_t> func{ REL::ID(686648) };
			return func(a_ref, a_idl);
		}
	};
}
