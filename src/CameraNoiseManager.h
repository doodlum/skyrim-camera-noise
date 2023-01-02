#pragma once

#include <iostream>
#include <shared_mutex>
#include <unordered_set>

#include <SimpleINI.h>

#include "PerlinNoise.hpp"
#include "ENB/ENBSeriesAPI.h"

class CameraNoiseManager
{
public:
	[[nodiscard]] static CameraNoiseManager* GetSingleton()
	{
		static CameraNoiseManager singleton;
		return std::addressof(singleton);
	}

	static void InstallHooks()
	{
		Hooks::Install();
	}

	struct Settings
	{
		float fFrequency1 = 1.0f;
		float fFrequency2 = 1.0f;
		float fFrequency3 = 1.0f;

		float fAmplitude1 = 1.0f;
		float fAmplitude2 = 1.0f;
		float fAmplitude3 = 1.0f;
	};

	bool bEnabled = true;
	Settings FirstPerson;
	Settings ThirdPerson;

	bool bInterpolation = false;
	uint32_t interpolationCounter = 0;
	uint32_t iInterpolationX = 4;
	uint32_t iInterpolationY = 5;
	std::pair<Settings, Settings> interpolation;

	std::unordered_set<std::string> inis;
	
	std::vector<float> GetData(bool use_interpolation=false);
	void SetData(const std::vector<float>& _data, bool use_interpolation = false);

	const siv::PerlinNoise perlin1{ 1 };
	const siv::PerlinNoise perlin2{ 2 };
	const siv::PerlinNoise perlin3{ 3 };
	const siv::PerlinNoise perlin4{ 4 };
	const siv::PerlinNoise perlin5{ 5 };
	const siv::PerlinNoise perlin6{ 6 };
	const siv::PerlinNoise perlin7{ 7 };

	double timeElapsed1 = 0.00;
	double timeElapsed2 = 0.00;
	double timeElapsed3 = 0.00;

	std::shared_mutex fileLock;
	void LoadINI();
	bool LoadCustomINI(RE::BSFixedString a_filepath, bool a_isUnloading);
	bool CheckCustomINI(const std::string& strPath, bool a_isUnloading);
	void SaveINI();

	void SetInterpolationOffset();
	bool CheckInterpolation();
	float GetInterpolation(float i_value);
	void ApplyInterpolation(Settings& currSettings, Settings& currInterpolation, float Settings::*field);
	void Interpolate();

	void Update(RE::TESCamera* a_camera);

	// ENB UI

	void RefreshUI();

protected:
	struct Hooks
	{
		struct TESCamera_Update
		{
			static void thunk(RE::TESCamera* a_camera)
			{
				func(a_camera);
				GetSingleton()->Update(a_camera);
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		static void Install()
		{
			REL::Relocation<std::uintptr_t> hook1{ RELOCATION_ID(49852, 50784) };  // 84AB90, 876700
			stl::write_thunk_call<TESCamera_Update>(hook1.address() + REL::Relocate(0x1A6, 0x1A6));
		}
	};

private:
	CameraNoiseManager() {
		LoadINI();
	}
	CameraNoiseManager(const CameraNoiseManager&) = delete;
	CameraNoiseManager(CameraNoiseManager&&) = delete;

	~CameraNoiseManager() = default;

	CameraNoiseManager& operator=(const CameraNoiseManager&) = delete;
	CameraNoiseManager& operator=(CameraNoiseManager&&) = delete;
};
