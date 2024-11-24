#pragma once
#include <SimpleIni.h>
#include "CameraNoiseManager.h"

class IniHandler
{
public:
	static IniHandler* GetSingleton()
	{
		static IniHandler singleton;
		return &singleton;
	}

	float GetSettingFloat(CSimpleIniA& ini, const char* a_section, const char* a_name, float& a_setting); 
	void SetSettingFloat(CSimpleIniA& ini, const char* a_section, const char* a_name, float a_setting);
	float ModSettingFloat(CSimpleIniA& ini, const char* a_section, const char* a_name, float a_modifier, float &a_setting);

	int GetSettingInt(CSimpleIniA& ini, const char* a_section, const char* a_name, uint32_t& a_setting);
	void SetSettingInt(CSimpleIniA& ini, const char* a_section, const char* a_name, uint32_t a_setting);
	
	bool GetSettingBool(CSimpleIniA& ini, const char* a_section, const char* a_name, bool a_default, bool& a_setting);
	void SetSettingBool(CSimpleIniA& ini, const char* a_section, const char* a_name, bool a_setting);

	float ModSettingInterpolation(CSimpleIniA& ini, const char* a_section, const char* a_name, float a_modifier, float& a_setting);
	void SetInterpolation(CSimpleIniA& ini, const char* a_section, const char* a_name, float a_modifier, float& a_interpolation,
		float a_setting);

	void GetAllSettings(CSimpleIniA& ini, CameraNoiseManager* manager);
	void SetAllSettings(CSimpleIniA& ini, CameraNoiseManager* manager);
	void ModAllSettings(CSimpleIniA& ini, CameraNoiseManager* manager, float modifier);

protected:
	IniHandler() = default;
	IniHandler(const IniHandler&) = delete;
	IniHandler(IniHandler&&) = delete;
	virtual ~IniHandler() = default;

	auto operator=(const IniHandler&) -> IniHandler& = delete;
	auto operator=(IniHandler&&) -> IniHandler& = delete;
};
