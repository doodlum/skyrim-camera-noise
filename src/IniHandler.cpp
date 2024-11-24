#include "IniHandler.h"

float IniHandler::GetSettingFloat(CSimpleIniA& ini, const char* a_section, const char* a_name, float& a_setting) {
	a_setting = (float)ini.GetDoubleValue(a_section, a_name, 1.0f);
	return a_setting;
}

void IniHandler::SetSettingFloat(CSimpleIniA& ini, const char* a_section, const char* a_name, float a_setting) {
	ini.SetDoubleValue(a_section, a_name, (double)a_setting);
}

float IniHandler::ModSettingFloat(CSimpleIniA& ini, const char* a_section, const char* a_name, float a_modifier, float& a_setting)
{
	a_setting = max(0.0f, a_setting + ((float)ini.GetDoubleValue(a_section, a_name, 0.0f) * a_modifier));
	return a_setting;
}

int IniHandler::GetSettingInt(CSimpleIniA& ini, const char* a_section, const char* a_name, uint32_t& a_setting) {
	a_setting = (uint32_t)ini.GetLongValue(a_section, a_name, 1);
	return a_setting;
}

void IniHandler::SetSettingInt(CSimpleIniA& ini, const char* a_section, const char* a_name, uint32_t a_setting) {
	ini.SetLongValue(a_section, a_name, a_setting);
}

bool IniHandler::GetSettingBool(CSimpleIniA& ini, const char* a_section, const char* a_name, bool a_default, bool& a_setting)
{
	a_setting = ini.GetBoolValue(a_section, a_name, a_default);
	return a_setting;
}

void IniHandler::SetSettingBool(CSimpleIniA& ini, const char* a_section, const char* a_name, bool a_setting) {
	ini.SetBoolValue(a_section, a_name, a_setting);
}

float IniHandler::ModSettingInterpolation(CSimpleIniA& ini, const char* a_section, const char* a_name, float a_modifier, 
	float& a_setting)
{
	a_setting += ((float)ini.GetDoubleValue(a_section, a_name, 0.0f) * a_modifier);
	return a_setting;
}

void IniHandler::SetInterpolation(CSimpleIniA& ini, const char* a_section, const char* a_name, float a_modifier, 
	float& a_interpolation, float a_setting)
{
	a_interpolation += ((float)ini.GetDoubleValue(a_section, a_name, 0.0f) - a_setting);
}

void IniHandler::GetAllSettings(CSimpleIniA& ini, CameraNoiseManager* manager)
{
	GetSettingBool(ini, "Global", "bEnabled", true, manager->bEnabled);

	GetSettingFloat(ini, "FirstPerson", "fFrequency1", manager->FirstPerson.fFrequency1);
	GetSettingFloat(ini, "FirstPerson", "fFrequency2", manager->FirstPerson.fFrequency2);
	GetSettingFloat(ini, "FirstPerson", "fFrequency3", manager->FirstPerson.fFrequency3);
	GetSettingFloat(ini, "FirstPerson", "fAmplitude1", manager->FirstPerson.fAmplitude1);
	GetSettingFloat(ini, "FirstPerson", "fAmplitude2", manager->FirstPerson.fAmplitude2);
	GetSettingFloat(ini, "FirstPerson", "fAmplitude3", manager->FirstPerson.fAmplitude3);

	GetSettingFloat(ini, "ThirdPerson", "fFrequency1", manager->ThirdPerson.fFrequency1);
	GetSettingFloat(ini, "ThirdPerson", "fFrequency2", manager->ThirdPerson.fFrequency2);
	GetSettingFloat(ini, "ThirdPerson", "fFrequency3", manager->ThirdPerson.fFrequency3);
	GetSettingFloat(ini, "ThirdPerson", "fAmplitude1", manager->ThirdPerson.fAmplitude1);
	GetSettingFloat(ini, "ThirdPerson", "fAmplitude2", manager->ThirdPerson.fAmplitude2);
	GetSettingFloat(ini, "ThirdPerson", "fAmplitude3", manager->ThirdPerson.fAmplitude3);

	GetSettingInt(ini, "Interpolation", "iInterpolationX", manager->iInterpolationX);
	GetSettingInt(ini, "Interpolation", "iInterpolationY", manager->iInterpolationY);
}

void IniHandler::SetAllSettings(CSimpleIniA& ini, CameraNoiseManager* manager)
{
	SetSettingBool(ini, "Global", "bEnabled", manager->bEnabled);

	SetSettingFloat(ini, "FirstPerson", "fFrequency1", manager->FirstPerson.fFrequency1);
	SetSettingFloat(ini, "FirstPerson", "fFrequency2", manager->FirstPerson.fFrequency2);
	SetSettingFloat(ini, "FirstPerson", "fFrequency3", manager->FirstPerson.fFrequency3);
	SetSettingFloat(ini, "FirstPerson", "fAmplitude1", manager->FirstPerson.fAmplitude1);
	SetSettingFloat(ini, "FirstPerson", "fAmplitude2", manager->FirstPerson.fAmplitude2);
	SetSettingFloat(ini, "FirstPerson", "fAmplitude3", manager->FirstPerson.fAmplitude3);

	SetSettingFloat(ini, "ThirdPerson", "fFrequency1", manager->ThirdPerson.fFrequency1);
	SetSettingFloat(ini, "ThirdPerson", "fFrequency2", manager->ThirdPerson.fFrequency2);
	SetSettingFloat(ini, "ThirdPerson", "fFrequency3", manager->ThirdPerson.fFrequency3);
	SetSettingFloat(ini, "ThirdPerson", "fAmplitude1", manager->ThirdPerson.fAmplitude1);
	SetSettingFloat(ini, "ThirdPerson", "fAmplitude2", manager->ThirdPerson.fAmplitude2);
	SetSettingFloat(ini, "ThirdPerson", "fAmplitude3", manager->ThirdPerson.fAmplitude3);

	SetSettingInt(ini, "Interpolation", "iInterpolationX", manager->iInterpolationX);
	SetSettingInt(ini, "Interpolation", "iInterpolationY", manager->iInterpolationY);
}

void IniHandler::ModAllSettings(CSimpleIniA& ini, CameraNoiseManager* manager, float modifier) {
	ModSettingInterpolation(ini, "FirstPerson", "fFrequency1", modifier, manager->interpolation.first.fFrequency1);
	ModSettingInterpolation(ini, "FirstPerson", "fFrequency2", modifier, manager->interpolation.first.fFrequency2);
	ModSettingInterpolation(ini, "FirstPerson", "fFrequency3", modifier, manager->interpolation.first.fFrequency3);
	ModSettingInterpolation(ini, "FirstPerson", "fAmplitude1", modifier, manager->interpolation.first.fAmplitude1);
	ModSettingInterpolation(ini, "FirstPerson", "fAmplitude2", modifier, manager->interpolation.first.fAmplitude2);
	ModSettingInterpolation(ini, "FirstPerson", "fAmplitude3", modifier, manager->interpolation.first.fAmplitude3);

	ModSettingInterpolation(ini, "ThirdPerson", "fFrequency1", modifier, manager->interpolation.second.fFrequency1);
	ModSettingInterpolation(ini, "ThirdPerson", "fFrequency2", modifier, manager->interpolation.second.fFrequency2);
	ModSettingInterpolation(ini, "ThirdPerson", "fFrequency3", modifier, manager->interpolation.second.fFrequency3);
	ModSettingInterpolation(ini, "ThirdPerson", "fAmplitude1", modifier, manager->interpolation.second.fAmplitude1);
	ModSettingInterpolation(ini, "ThirdPerson", "fAmplitude2", modifier, manager->interpolation.second.fAmplitude2);
	ModSettingInterpolation(ini, "ThirdPerson", "fAmplitude3", modifier, manager->interpolation.second.fAmplitude3);
}
