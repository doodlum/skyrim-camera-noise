#include "ModAPI.h"
#include "CameraNoiseManager.h"

bool CameraNoiseInterfaceImpl::LoadIni(std::string filepath) {
	return CameraNoiseManager::GetSingleton()->LoadCustomINI(filepath, false);
}

bool CameraNoiseInterfaceImpl::UnloadIni(std::string filepath)
{
	return CameraNoiseManager::GetSingleton()->LoadCustomINI(filepath, true);
}

void CameraNoiseInterfaceImpl::ResetIni()
{
	CameraNoiseManager::GetSingleton()->ResetINIs();
}
