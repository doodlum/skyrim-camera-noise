#include "CameraNoiseManager.h"

#define GetSettingFloat(a_section, a_name, a_setting) a_setting = (float)ini.GetDoubleValue(a_section, a_name, 1.0f);
#define SetSettingFloat(a_section, a_name, a_setting) ini.SetDoubleValue(a_section, a_name, a_setting);

#define ModSettingFloat(a_section, a_name, a_setting, a_modifier) a_setting = std::max(0.0f, a_setting + ((float)ini.GetDoubleValue(a_section, a_name, 0.0f) * a_modifier));
#define ModInterpolation(a_section, a_name, a_setting, a_modifier) a_setting += ((float)ini.GetDoubleValue(a_section, a_name, 0.0f) * a_modifier);
#define SetInterpolation(a_section, a_name, a_setting, a_interp) a_interp += ((float)ini.GetDoubleValue(a_section, a_name, 0.0f) - a_setting);

#define GetSettingBool(a_section, a_setting, a_default) a_setting = ini.GetBoolValue(a_section, #a_setting, a_default);
#define SetSettingBool(a_section, a_setting) ini.SetBoolValue(a_section, #a_setting, a_setting);

#define GetSettingInt(a_section, a_setting, a_default) a_setting = ini.GetLongValue(a_section, #a_setting, a_default);
#define SetSettingInt(a_section, a_setting) ini.SetLongValue(a_section, #a_setting, a_setting);
 

std::vector<float> CameraNoiseManager::GetData(bool use_interpolation)
{
	if (!use_interpolation) {
		return std::vector<float>{ FirstPerson.fFrequency1, FirstPerson.fFrequency2, FirstPerson.fFrequency3,
			FirstPerson.fAmplitude1, FirstPerson.fAmplitude2, FirstPerson.fAmplitude3,
			ThirdPerson.fFrequency1, ThirdPerson.fFrequency2, ThirdPerson.fFrequency3,
			ThirdPerson.fAmplitude1, ThirdPerson.fAmplitude2, ThirdPerson.fAmplitude3 };
	} else {
		return std::vector<float>{ interpolation.first.fFrequency1, interpolation.first.fFrequency2, interpolation.first.fFrequency3,
			interpolation.first.fAmplitude1, interpolation.first.fAmplitude2, interpolation.first.fAmplitude3,
			interpolation.second.fFrequency1, interpolation.second.fFrequency2, interpolation.second.fFrequency3,
			interpolation.second.fAmplitude1, interpolation.second.fAmplitude2, interpolation.second.fAmplitude3 };
	}
}

void CameraNoiseManager::SetData(const std::vector<float>& _data, bool use_interpolation)
{
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

void CameraNoiseManager::LoadINI()
{
	std::lock_guard<std::shared_mutex> lk(fileLock);
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(L"Data\\SKSE\\Plugins\\CameraNoise.ini");

	GetSettingBool("Global", bEnabled, true);

	GetSettingInt("Global", iInterpolationX, 3);
	GetSettingInt("Global", iInterpolationY, 5);

	GetSettingFloat("FirstPerson", "fFrequency1", FirstPerson.fFrequency1);
	GetSettingFloat("FirstPerson", "fFrequency2", FirstPerson.fFrequency2);
	GetSettingFloat("FirstPerson", "fFrequency3", FirstPerson.fFrequency3);
	GetSettingFloat("FirstPerson", "fAmplitude1", FirstPerson.fAmplitude1);
	GetSettingFloat("FirstPerson", "fAmplitude2", FirstPerson.fAmplitude2);
	GetSettingFloat("FirstPerson", "fAmplitude3", FirstPerson.fAmplitude3);

	GetSettingFloat("ThirdPerson", "fFrequency1", ThirdPerson.fFrequency1);
	GetSettingFloat("ThirdPerson", "fFrequency2", ThirdPerson.fFrequency2);
	GetSettingFloat("ThirdPerson", "fFrequency3", ThirdPerson.fFrequency3);
	GetSettingFloat("ThirdPerson", "fAmplitude1", ThirdPerson.fAmplitude1);
	GetSettingFloat("ThirdPerson", "fAmplitude2", ThirdPerson.fAmplitude2);
	GetSettingFloat("ThirdPerson", "fAmplitude3", ThirdPerson.fAmplitude3);
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

void CameraNoiseManager::LoadCustomINI(RE::BSFixedString a_filepath, bool a_isUnloading)
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

		ModInterpolation("FirstPerson", "fFrequency1", interpolation.first.fFrequency1, modifier);
		ModInterpolation("FirstPerson", "fFrequency2", interpolation.first.fFrequency2, modifier);
		ModInterpolation("FirstPerson", "fFrequency3", interpolation.first.fFrequency3, modifier);
		ModInterpolation("FirstPerson", "fAmplitude1", interpolation.first.fAmplitude1, modifier);
		ModInterpolation("FirstPerson", "fAmplitude2", interpolation.first.fAmplitude2, modifier);
		ModInterpolation("FirstPerson", "fAmplitude3", interpolation.first.fAmplitude3, modifier);

		ModInterpolation("ThirdPerson", "fFrequency1", interpolation.second.fFrequency1, modifier);
		ModInterpolation("ThirdPerson", "fFrequency2", interpolation.second.fFrequency2, modifier);
		ModInterpolation("ThirdPerson", "fFrequency3", interpolation.second.fFrequency3, modifier);
		ModInterpolation("ThirdPerson", "fAmplitude1", interpolation.second.fAmplitude1, modifier);
		ModInterpolation("ThirdPerson", "fAmplitude2", interpolation.second.fAmplitude2, modifier);
		ModInterpolation("ThirdPerson", "fAmplitude3", interpolation.second.fAmplitude3, modifier);

		bInterpolation = true;
	}
}

void CameraNoiseManager::SaveINI()
{
	std::lock_guard<std::shared_mutex> lk(fileLock);
	CSimpleIniA ini;
	ini.SetUnicode();

	SetSettingBool("Global", bEnabled);

	SetSettingInt("Global", iInterpolationX);
	SetSettingInt("Global", iInterpolationY);

	SetSettingFloat("FirstPerson", "fFrequency1", FirstPerson.fFrequency1);
	SetSettingFloat("FirstPerson", "fFrequency2", FirstPerson.fFrequency2);
	SetSettingFloat("FirstPerson", "fFrequency3", FirstPerson.fFrequency3);
	SetSettingFloat("FirstPerson", "fAmplitude1", FirstPerson.fAmplitude1);
	SetSettingFloat("FirstPerson", "fAmplitude2", FirstPerson.fAmplitude2);
	SetSettingFloat("FirstPerson", "fAmplitude3", FirstPerson.fAmplitude3);

	SetSettingFloat("ThirdPerson", "fFrequency1", ThirdPerson.fFrequency1);
	SetSettingFloat("ThirdPerson", "fFrequency2", ThirdPerson.fFrequency2);
	SetSettingFloat("ThirdPerson", "fFrequency3", ThirdPerson.fFrequency3);
	SetSettingFloat("ThirdPerson", "fAmplitude1", ThirdPerson.fAmplitude1);
	SetSettingFloat("ThirdPerson", "fAmplitude2", ThirdPerson.fAmplitude2);
	SetSettingFloat("ThirdPerson", "fAmplitude3", ThirdPerson.fAmplitude3);

	ini.SaveFile(L"Data\\SKSE\\Plugins\\CameraNoise.ini");
}

bool CameraNoiseManager::CheckInterpolation() {
	return interpolation.first.fFrequency1 == 0.0f && interpolation.first.fFrequency2 == 0.0f && interpolation.first.fFrequency3 == 0.0f &&
		interpolation.first.fAmplitude1 == 0.0f && interpolation.first.fAmplitude2 == 0.0f && interpolation.first.fAmplitude3 == 0.0f &&
		interpolation.second.fFrequency1 == 0.0f && interpolation.second.fFrequency2 == 0.0f && interpolation.second.fFrequency3 == 0.0f &&
		interpolation.second.fAmplitude1 == 0.0f && interpolation.second.fAmplitude2 == 0.0f && interpolation.second.fAmplitude3 == 0.0f;
}

float CameraNoiseManager::GetInterpolation(float i_value) {
	if (i_value > 0.0f) {
		if (i_value >= 1.0f) {
			return 1.0f;
		} else {
			return i_value;
		}
	} else {
		if (i_value < 0.0f) {
			if (i_value <= 1.0f) {
				return -1.0f;
			} else {
				return i_value;
			}
		}
	}
	return 0.0f;
}

void CameraNoiseManager::ApplyInterpolation(Settings& currSettings, Settings& currInterpolation, float Settings::* field) {
	float modifier = GetInterpolation(currInterpolation.*field);
	if (modifier != 0.0f) {
		currSettings.*field -= modifier;
		currInterpolation.*field -= modifier;
	}
}

void CameraNoiseManager::Interpolate()
{
	if (bInterpolation) {
		if (interpolationCounter % iInterpolationY < iInterpolationX) {
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

			if (CheckInterpolation()) {
				bInterpolation = false;
				interpolationCounter = 0;
			} else {
				interpolationCounter += 1;
			}
		} else {
			interpolationCounter += 1;
		}
	}
}

extern ENB_API::ENBSDKALT1001* g_ENB;

#define TWDEF "group = 'MOD:Camera Noise' precision = 2 step = 0.01 "
#define TWDEF2 "group = 'First Person' precision = 2 step = 0.01 "
#define TWDEF3 "group = 'Third Person' precision = 2 step = 0.01 "

void CameraNoiseManager::RefreshUI()
{
	auto bar = g_ENB->TwGetBarByEnum(!REL::Module::IsVR() ? ENB_API::ENBWindowType::EditorBarEffects : ENB_API::ENBWindowType::EditorBarObjects);  // ENB misnames its own bar, whoops!
	g_ENB->TwAddVarRW(bar, "EnableCameraNoise", ETwType::TW_TYPE_BOOLCPP, &bEnabled, TWDEF);
	g_ENB->TwAddVarRW(bar, "InterpolationX", ETwType::TW_TYPE_UINT32, &iInterpolationX, TWDEF);
	g_ENB->TwAddVarRW(bar, "InterpolationY", ETwType::TW_TYPE_UINT32, &iInterpolationY, TWDEF);

	g_ENB->TwAddVarRW(bar, "1PFrequency1", ETwType::TW_TYPE_FLOAT, &FirstPerson.fFrequency1, TWDEF2 " label = 'fFrequency1 (Translation)'");
	g_ENB->TwAddVarRW(bar, "1PFrequency2", ETwType::TW_TYPE_FLOAT, &FirstPerson.fFrequency2, TWDEF2 " label = 'fFrequency2 (Rotation)'");
	g_ENB->TwAddVarRW(bar, "1PFrequency3", ETwType::TW_TYPE_FLOAT, &FirstPerson.fFrequency3, TWDEF2 " label = 'fFrequency3 (Rotation)'");
	g_ENB->TwAddVarRW(bar, "1PAmplitude1", ETwType::TW_TYPE_FLOAT, &FirstPerson.fAmplitude1, TWDEF2 " label = 'fAmplitude1 (Rotation)'");
	g_ENB->TwAddVarRW(bar, "1PAmplitude2", ETwType::TW_TYPE_FLOAT, &FirstPerson.fAmplitude2, TWDEF2 " label = 'fAmplitude2 (Rotation)'");
	g_ENB->TwAddVarRW(bar, "1PAmplitude3", ETwType::TW_TYPE_FLOAT, &FirstPerson.fAmplitude3, TWDEF2 " label = 'fAmplitude3 (Rotation)'");

	g_ENB->TwAddVarRW(bar, "3PFrequency1", ETwType::TW_TYPE_FLOAT, &ThirdPerson.fFrequency1, TWDEF3 " label = 'fFrequency1 (Translation)'");
	g_ENB->TwAddVarRW(bar, "3PFrequency2", ETwType::TW_TYPE_FLOAT, &ThirdPerson.fFrequency2, TWDEF3 " label = 'fFrequency2 (Rotation)'");
	g_ENB->TwAddVarRW(bar, "3PFrequency3", ETwType::TW_TYPE_FLOAT, &ThirdPerson.fFrequency3, TWDEF3 " label = 'fFrequency3 (Rotation)'");
	g_ENB->TwAddVarRW(bar, "3PAmplitude1", ETwType::TW_TYPE_FLOAT, &ThirdPerson.fAmplitude1, TWDEF3 " label = 'fAmplitude1 (Translation)'");
	g_ENB->TwAddVarRW(bar, "3PAmplitude2", ETwType::TW_TYPE_FLOAT, &ThirdPerson.fAmplitude2, TWDEF3 " label = 'fAmplitude2 (Rotation)'");
	g_ENB->TwAddVarRW(bar, "3PAmplitude3", ETwType::TW_TYPE_FLOAT, &ThirdPerson.fAmplitude3, TWDEF3 " label = 'fAmplitude3 (Rotation)'");

	g_ENB->TwDefine("EditorBarEffects/'First Person' group = 'MOD:Camera Noise'");
	g_ENB->TwDefine("EditorBarEffects/'Third Person' group = 'MOD:Camera Noise'");
	g_ENB->TwDefine("EditorBarEffects/'MOD:Camera Noise' opened=false");
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
	if (bEnabled && !RE::UI::GetSingleton()->GameIsPaused()) {
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

		a_camera->cameraRoot->local.rotate = a_camera->cameraRoot->local.rotate * MatrixFromAxisAngle(rotationOffset, 0.00015f * settings.fAmplitude2);

		RE::NiPoint3 rotationOffset2 = {
			(float)perlin5.noise1D(timeElapsed3) * glm::two_pi<float>(),
			(float)perlin6.noise1D(timeElapsed3) * glm::two_pi<float>(),
			(float)perlin7.noise1D(timeElapsed3) * glm::two_pi<float>()
		};

		a_camera->cameraRoot->local.rotate = a_camera->cameraRoot->local.rotate * MatrixFromAxisAngle(rotationOffset2, 0.00005f * settings.fAmplitude3);

		RE::NiUpdateData updateData;
		a_camera->cameraRoot->UpdateDownwardPass(updateData, 0);

		auto playerCamera = RE::PlayerCamera::GetSingleton();
		auto niCamera = GetNiCamera(playerCamera);
		if (niCamera && niCamera.get())
			UpdateInternalWorldToScreenMatrix(niCamera.get());
	}
}
