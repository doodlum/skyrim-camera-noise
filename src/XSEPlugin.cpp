#include "CameraNoiseManager.h"
#include "Hooks.h"
#include "ENBHandler.h"
#include "Serialization.h"

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	ENBHandler::MessageHandler(a_msg);
}

bool SetupHooks()
{
	std::vector<float> interp(12);
	std::fill(interp.begin(), interp.end(), 0.0f);
	CameraNoiseManager::GetSingleton()->SetInterpolation(interp, true);
	Hooks::Install();
	return true;
}
