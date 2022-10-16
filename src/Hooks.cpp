#include "Hooks.h"
#include "CameraNoiseManager.h"

namespace Hooks
{
	void Hooks::Install()
	{
		CameraNoiseManager::InstallHooks();
		logger::info("Installed all hooks");
	}
}
