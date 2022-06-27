#include "Hooks.h"
#include "CameraShakeManager.h"

namespace Hooks
{
	void Hooks::Install()
	{
		CameraShakeManager::InstallHooks();
		logger::info("Installed all hooks");
	}
}
