#include "CameraNoiseManager.h"

#define GetSettingFloat(a_section, a_name, a_setting) a_setting = (float)ini.GetDoubleValue(a_section, a_name, 1.0f);
#define SetSettingFloat(a_section, a_name, a_setting) ini.SetDoubleValue(a_section, a_name, a_setting);

#define GetSettingBool(a_section, a_setting, a_default) a_setting = ini.GetBoolValue(a_section, #a_setting, a_default);
#define SetSettingBool(a_section, a_setting) ini.SetBoolValue(a_section, #a_setting, a_setting);

void CameraNoiseManager::LoadINI()
{
	std::lock_guard<std::shared_mutex> lk(fileLock);
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(L"Data\\SKSE\\Plugins\\CameraNoise.ini");

	GetSettingBool("Global", bEnabled, true);

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

void CameraNoiseManager::SaveINI()
{
	std::lock_guard<std::shared_mutex> lk(fileLock);
	CSimpleIniA ini;
	ini.SetUnicode();

	SetSettingBool("Global", bEnabled);

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

extern ENB_API::ENBSDKALT1001* g_ENB;

#define TWDEF "group = 'MOD:Camera Noise' precision = 2 step = 0.01 "
#define TWDEF2 "group = 'First Person' precision = 2 step = 0.01 "
#define TWDEF3 "group = 'Third Person' precision = 2 step = 0.01 "

void CameraNoiseManager::RefreshUI()
{
	auto bar = g_ENB->TwGetBarByEnum(!REL::Module::IsVR() ? ENB_API::ENBWindowType::EditorBarEffects : ENB_API::ENBWindowType::EditorBarObjects);  // ENB misnames its own bar, whoops!
	g_ENB->TwAddVarRW(bar, "EnableCameraNoise", ETwType::TW_TYPE_BOOLCPP, &bEnabled, TWDEF);

	g_ENB->TwAddVarRW(bar, "1PFrequency1", ETwType::TW_TYPE_FLOAT, &FirstPerson.fFrequency1, TWDEF2 " label = fFrequency1 (Translation)");
	g_ENB->TwAddVarRW(bar, "1PFrequency2", ETwType::TW_TYPE_FLOAT, &FirstPerson.fFrequency2, TWDEF2 " label = fFrequency2 (Rotation)");
	g_ENB->TwAddVarRW(bar, "1PFrequency3", ETwType::TW_TYPE_FLOAT, &FirstPerson.fFrequency3, TWDEF2 " label = fFrequency3 (Rotation)");
	g_ENB->TwAddVarRW(bar, "1PAmplitude1", ETwType::TW_TYPE_FLOAT, &FirstPerson.fAmplitude1, TWDEF2 " label = fAmplitude1 (Rotation)");
	g_ENB->TwAddVarRW(bar, "1PAmplitude2", ETwType::TW_TYPE_FLOAT, &FirstPerson.fAmplitude2, TWDEF2 " label = fAmplitude2 (Rotation)");
	g_ENB->TwAddVarRW(bar, "1PAmplitude3", ETwType::TW_TYPE_FLOAT, &FirstPerson.fAmplitude3, TWDEF2 " label = fAmplitude3 (Rotation)");

	g_ENB->TwAddVarRW(bar, "3PFrequency1", ETwType::TW_TYPE_FLOAT, &ThirdPerson.fFrequency1, TWDEF3 " label = fFrequency1 (Translation)");
	g_ENB->TwAddVarRW(bar, "3PFrequency2", ETwType::TW_TYPE_FLOAT, &ThirdPerson.fFrequency2, TWDEF3 " label = fFrequency2 (Rotation)");
	g_ENB->TwAddVarRW(bar, "3PFrequency3", ETwType::TW_TYPE_FLOAT, &ThirdPerson.fFrequency3, TWDEF3 " label = fFrequency3 (Rotation)");
	g_ENB->TwAddVarRW(bar, "3PAmplitude1", ETwType::TW_TYPE_FLOAT, &ThirdPerson.fAmplitude1, TWDEF3 " label = fAmplitude1 (Translation)");
	g_ENB->TwAddVarRW(bar, "3PAmplitude2", ETwType::TW_TYPE_FLOAT, &ThirdPerson.fAmplitude2, TWDEF3 " label = fAmplitude2 (Rotation)");
	g_ENB->TwAddVarRW(bar, "3PAmplitude3", ETwType::TW_TYPE_FLOAT, &ThirdPerson.fAmplitude3, TWDEF3 " label = fAmplitude3 (Rotation)");

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

void CameraNoiseManager::Update(RE::TESCamera* a_camera)
{
	if (bEnabled) {

		static float& g_deltaTime = (*(float*)RELOCATION_ID(523660, 410199).address());

		Settings settings = RE::PlayerCamera::GetSingleton()->IsInFirstPerson() ? FirstPerson : ThirdPerson;

		timeElapsed1 += g_deltaTime * settings.fFrequency1;
		timeElapsed2 += g_deltaTime * settings.fFrequency2;
		timeElapsed3 += g_deltaTime * 3.0f * settings.fFrequency3;

		RE::NiPoint3 translationOffset = {
			(float)perlin1.noise1D(timeElapsed1),
			(float)perlin2.noise1D(timeElapsed1),
			(float)perlin3.noise1D(timeElapsed1)
		};

		a_camera->cameraRoot->local.translate += translationOffset * settings.fAmplitude1;

		RE::NiPoint3 rotationOffset = {
			(float)perlin3.noise1D(timeElapsed2) * glm::two_pi<float>(),
			(float)perlin4.noise1D(timeElapsed2) * glm::two_pi<float>(),
			(float)perlin5.noise1D(timeElapsed2) * glm::two_pi<float>()
		};

		a_camera->cameraRoot->local.rotate = a_camera->cameraRoot->local.rotate * MatrixFromAxisAngle(rotationOffset, 0.0005f * settings.fAmplitude2);

		RE::NiPoint3 rotationOffset2 = {
			(float)perlin5.noise1D(timeElapsed3) * glm::two_pi<float>(),
			(float)perlin6.noise1D(timeElapsed3) * glm::two_pi<float>(),
			(float)perlin7.noise1D(timeElapsed3) * glm::two_pi<float>()
		};

		a_camera->cameraRoot->local.rotate = a_camera->cameraRoot->local.rotate * MatrixFromAxisAngle(rotationOffset2, 0.00025f * settings.fAmplitude3);

		RE::NiUpdateData updateData;
		a_camera->cameraRoot->UpdateDownwardPass(updateData, 0);
	}
}
