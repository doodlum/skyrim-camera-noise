#pragma once

namespace PapyrusAPI
{
	bool LoadINI(RE::StaticFunctionTag*, RE::BSFixedString filepath);
	bool UnloadINI(RE::StaticFunctionTag*, RE::BSFixedString filepath);
	bool ResetINI(RE::StaticFunctionTag*);

	bool RegisterFunctions(RE::BSScript::IVirtualMachine* a_vm);

	void SetupAPI();
}
