#include <glm/gtc/constants.hpp>
#include "CameraNoiseManager.h"
#include "IniHandler.h"

#include "ENB/enbseries.h"

void CameraNoiseManager::LoadINI()
{
	std::lock_guard<std::shared_mutex> lk(fileLock);
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(L"Data\\SKSE\\Plugins\\CameraNoise.ini");
	IniHandler::GetSingleton()->GetAllSettings(ini, this);
}

bool CameraNoiseManager::CheckCustomINI(const std::string& strPath, bool a_isUnloading) {
	if (a_isUnloading && inis.contains(strPath)) {
		inis.erase(strPath);
		return true;
	} else {
		if (!a_isUnloading && !inis.contains(strPath)) {
			inis.insert(strPath);
			return true;
		}
	}
	return false;
}

bool CameraNoiseManager::LoadCustomINI(RE::BSFixedString a_filepath, bool a_isUnloading)
{
	std::string strPath = std::string(a_filepath.data());
	if (CheckCustomINI(strPath, a_isUnloading)) {
		std::lock_guard<std::shared_mutex> lk(fileLock);
		CSimpleIniA ini;
		ini.SetUnicode();
		std::string fullPath = "Data\\SKSE\\Plugins\\_CameraNoise\\" + strPath;
		std::wstring widestr = std::wstring(fullPath.begin(), fullPath.end());
		ini.LoadFile(widestr.c_str());

		float modifier = a_isUnloading ? -1.0f : 1.0f;
		IniHandler::GetSingleton()->ModAllSettings(ini, this, modifier);
		interpolationTransitionSpeed = (int)ini.GetDoubleValue("Settings", "iTransitionSpeed", 1);
		bInterpolation = true;
		
		return true;
	} else {
		return false;
	}
}

void CameraNoiseManager::SaveINI()
{
	std::lock_guard<std::shared_mutex> lk(fileLock);
	CSimpleIniA ini;
	ini.SetUnicode();
	IniHandler::GetSingleton()->SetAllSettings(ini, this);
	ini.SaveFile(L"Data\\SKSE\\Plugins\\CameraNoise.ini");
}

void CameraNoiseManager::ResetINIs() {
	CameraNoiseManager::GetSingleton()->LoadINI();
	CameraNoiseManager::GetSingleton()->inis.clear();
}

bool CameraNoiseManager::InterpolationHasEnded() {
	return interpolation.first.fFrequency1 == 0.0f && interpolation.first.fFrequency2 == 0.0f && interpolation.first.fFrequency3 == 0.0f &&
		interpolation.first.fAmplitude1 == 0.0f && interpolation.first.fAmplitude2 == 0.0f && interpolation.first.fAmplitude3 == 0.0f &&
		interpolation.second.fFrequency1 == 0.0f && interpolation.second.fFrequency2 == 0.0f && interpolation.second.fFrequency3 == 0.0f &&
		interpolation.second.fAmplitude1 == 0.0f && interpolation.second.fAmplitude2 == 0.0f && interpolation.second.fAmplitude3 == 0.0f;
}

void CameraNoiseManager::SetInterpolation(const std::vector<float>& _data, bool use_interpolation) {
	if (!use_interpolation) {
		FirstPerson.fFrequency1 = _data[0];
		FirstPerson.fFrequency2 = _data[1];
		FirstPerson.fFrequency3 = _data[2];
		FirstPerson.fAmplitude1 = _data[3];
		FirstPerson.fAmplitude2 = _data[4];
		FirstPerson.fAmplitude3 = _data[5];

		ThirdPerson.fFrequency1 = _data[6];
		ThirdPerson.fFrequency2 = _data[7];
		ThirdPerson.fFrequency3 = _data[8];
		ThirdPerson.fAmplitude1 = _data[9];
		ThirdPerson.fAmplitude2 = _data[10];
		ThirdPerson.fAmplitude3 = _data[11];
	} else {
		interpolation.first.fFrequency1 = _data[0];
		interpolation.first.fFrequency2 = _data[1];
		interpolation.first.fFrequency3 = _data[2];
		interpolation.first.fAmplitude1 = _data[3];
		interpolation.first.fAmplitude2 = _data[4];
		interpolation.first.fAmplitude3 = _data[5];

		interpolation.second.fFrequency1 = _data[6];
		interpolation.second.fFrequency2 = _data[7];
		interpolation.second.fFrequency3 = _data[8];
		interpolation.second.fAmplitude1 = _data[9];
		interpolation.second.fAmplitude2 = _data[10];
		interpolation.second.fAmplitude3 = _data[11];
	}
}

float CameraNoiseManager::GetInterpolation(float i_value) {
	if (i_value == 0.0f) {
		return 0.0f;
	} else if (i_value > 0.0f) {
		if (i_value >= 1.0f) {
			return 1.0f;
		} else {
			return i_value;
		}
	} else {
		if (i_value <= 1.0f) {
			return -1.0f;
		} else {
			return i_value;
		}
	}
}

void CameraNoiseManager::ApplyInterpolation(Settings& currSettings, Settings& currInterpolation, float Settings::* field) {
	float modifier = GetInterpolation(currInterpolation.*field);
	if (modifier != 0.0f) {
		currSettings.*field -= modifier;
		currInterpolation.*field -= modifier;
	}
}

void CameraNoiseManager::ApplyInterpolations() {
	ApplyInterpolation(FirstPerson, interpolation.first, &Settings::fFrequency1);
	ApplyInterpolation(FirstPerson, interpolation.first, &Settings::fFrequency2);
	ApplyInterpolation(FirstPerson, interpolation.first, &Settings::fFrequency3);
	ApplyInterpolation(FirstPerson, interpolation.first, &Settings::fAmplitude1);
	ApplyInterpolation(FirstPerson, interpolation.first, &Settings::fAmplitude2);
	ApplyInterpolation(FirstPerson, interpolation.first, &Settings::fAmplitude3);

	ApplyInterpolation(ThirdPerson, interpolation.second, &Settings::fFrequency1);
	ApplyInterpolation(ThirdPerson, interpolation.second, &Settings::fFrequency2);
	ApplyInterpolation(ThirdPerson, interpolation.second, &Settings::fFrequency3);
	ApplyInterpolation(ThirdPerson, interpolation.second, &Settings::fAmplitude1);
	ApplyInterpolation(ThirdPerson, interpolation.second, &Settings::fAmplitude2);
	ApplyInterpolation(ThirdPerson, interpolation.second, &Settings::fAmplitude3);
}

void CameraNoiseManager::CheckInterpolate() {
	if (interpolationCounter % iInterpolationY < iInterpolationX) {
		ApplyInterpolations();
		if (InterpolationHasEnded()) {
			bInterpolation = false;
			interpolationCounter = 0;
		} else {
			interpolationCounter += 1;
		}
	} else {
		interpolationCounter += 1;
	}
}

void CameraNoiseManager::Interpolate()
{
	if (bInterpolation) {
		if (interpolationTransitionSpeed > 0) {
			for (int idx = 0; idx < interpolationTransitionSpeed; idx++) {
				CheckInterpolate();
			}
		} else {
			if (interpolationTransitionIdx <= interpolationTransitionSpeed) {
				interpolationTransitionIdx = 0;
				CheckInterpolate();
			} else {
				interpolationTransitionIdx -= 1;
			}
		}
	}
}

RE::NiMatrix3 MatrixFromAxisAngle(const RE::NiPoint3& axis, float theta)
{
	RE::NiPoint3 a = axis;
	float cosTheta = cosf(theta);
	float sinTheta = sinf(theta);
	RE::NiMatrix3 result;

	result.entry[0][0] = cosTheta + a.x * a.x * (1 - cosTheta);
	result.entry[0][1] = a.x * a.y * (1 - cosTheta) - a.z * sinTheta;
	result.entry[0][2] = a.x * a.z * (1 - cosTheta) + a.y * sinTheta;

	result.entry[1][0] = a.y * a.x * (1 - cosTheta) + a.z * sinTheta;
	result.entry[1][1] = cosTheta + a.y * a.y * (1 - cosTheta);
	result.entry[1][2] = a.y * a.z * (1 - cosTheta) - a.x * sinTheta;

	result.entry[2][0] = a.z * a.x * (1 - cosTheta) - a.y * sinTheta;
	result.entry[2][1] = a.z * a.y * (1 - cosTheta) + a.x * sinTheta;
	result.entry[2][2] = cosTheta + a.z * a.z * (1 - cosTheta);

	return result;
}

RE::NiPointer<RE::NiCamera> GetNiCamera(RE::PlayerCamera* camera)
{
	// Do other things parent stuff to the camera node? Better safe than sorry I guess
	if (camera->cameraRoot->GetChildren().size() == 0)
		return nullptr;
	for (auto& entry : camera->cameraRoot->GetChildren()) {
		auto asCamera = skyrim_cast<RE::NiCamera*>(entry.get());
		if (asCamera)
			return RE::NiPointer<RE::NiCamera>(asCamera);
	}
	return nullptr;
}

void UpdateInternalWorldToScreenMatrix(RE::NiCamera* a_niCamera)
{
	using func_t = decltype(&UpdateInternalWorldToScreenMatrix);
	REL::Relocation<func_t> func{ REL::RelocationID(69271, 70641) };
	func(a_niCamera);
}

void CameraNoiseManager::Update(RE::TESCamera* a_camera)
{
	if (bEnabled && !RE::UI::GetSingleton()->GameIsPaused() && !RE::UI::GetSingleton()->IsMenuOpen("MapMenu")) {
		static float& g_deltaTime = (*(float*)RELOCATION_ID(523660, 410199).address());

		Interpolate();

		Settings settings = RE::PlayerCamera::GetSingleton()->IsInFirstPerson() ? FirstPerson : ThirdPerson;

		timeElapsed1 += g_deltaTime * settings.fFrequency1;
		timeElapsed2 += g_deltaTime * settings.fFrequency2;
		timeElapsed3 += g_deltaTime * 5.0f * settings.fFrequency3;

		RE::NiPoint3 translationOffset = {
			(float)perlin1.noise1D(timeElapsed1),
			(float)perlin2.noise1D(timeElapsed1),
			(float)perlin3.noise1D(timeElapsed1)
		};

		a_camera->cameraRoot->local.translate += translationOffset * 0.5f * settings.fAmplitude1;

		// This is wrong, but it looks right
		RE::NiPoint3 rotationOffset = {
			(float)perlin3.noise1D(timeElapsed2) * glm::two_pi<float>(),
			(float)perlin4.noise1D(timeElapsed2) * glm::two_pi<float>(),
			(float)perlin5.noise1D(timeElapsed2) * glm::two_pi<float>()
		};

		a_camera->cameraRoot->local.rotate = a_camera->cameraRoot->local.rotate * MatrixFromAxisAngle(rotationOffset, 
			0.00015f * settings.fAmplitude2);

		RE::NiPoint3 rotationOffset2 = {
			(float)perlin5.noise1D(timeElapsed3) * glm::two_pi<float>(),
			(float)perlin6.noise1D(timeElapsed3) * glm::two_pi<float>(),
			(float)perlin7.noise1D(timeElapsed3) * glm::two_pi<float>()
		};

		a_camera->cameraRoot->local.rotate = a_camera->cameraRoot->local.rotate * MatrixFromAxisAngle(rotationOffset2, 
			0.00005f * settings.fAmplitude3);

		RE::NiUpdateData updateData;
		a_camera->cameraRoot->UpdateDownwardPass(updateData, 0);

		auto playerCamera = RE::PlayerCamera::GetSingleton();
		auto niCamera = GetNiCamera(playerCamera);
		if (niCamera && niCamera.get())
			UpdateInternalWorldToScreenMatrix(niCamera.get());
	}
}
