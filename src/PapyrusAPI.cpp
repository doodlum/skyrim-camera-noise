#include "PapyrusAPI.h"

#include "CameraNoiseManager.h"

namespace PapyrusAPI
{
	bool LoadINI(RE::StaticFunctionTag*, RE::BSFixedString filepath)
	{
		return CameraNoiseManager::GetSingleton()->LoadCustomINI(filepath, false);
	}

	bool UnloadINI(RE::StaticFunctionTag*, RE::BSFixedString filepath)
	{
		return CameraNoiseManager::GetSingleton()->LoadCustomINI(filepath, true);
	}

	bool ResetINI(RE::StaticFunctionTag*)
	{
		CameraNoiseManager::GetSingleton()->ResetINIs();
		return true;
	}

	bool RegisterFunctions(RE::BSScript::IVirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("LoadIni"sv, "CameraNoise"sv, LoadINI);
		a_vm->RegisterFunction("UnloadIni"sv, "CameraNoise"sv, UnloadINI);
		a_vm->RegisterFunction("ResetIni"sv, "CameraNoise"sv, ResetINI);
		return true;
	}

	void SetupAPI()
	{
		const auto papyrus = SKSE::GetPapyrusInterface();
		papyrus->Register(RegisterFunctions);
	}

}
