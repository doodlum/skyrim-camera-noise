#pragma once

#include <iostream>
#include "PerlinNoise.hpp"

#define AMPLITUDE 2.0
#define FREQUENCY 1.0

class CameraShakeManager
{
public:
	[[nodiscard]] static CameraShakeManager* GetSingleton()
	{
		static CameraShakeManager singleton;
		return std::addressof(singleton);
	}

	static void InstallHooks()
	{
		Hooks::Install();
	}

	const siv::PerlinNoise perlin1;
	const siv::PerlinNoise perlin2;
	const siv::PerlinNoise perlin3;
	const siv::PerlinNoise perlin4;
	const siv::PerlinNoise perlin5;
	const siv::PerlinNoise perlin6;

	double timeElapsed = 0.00;

	void UpdateCamera(RE::ThirdPersonState* a_state);

protected:

	struct Hooks
	{
		struct Camera_Update
		{
			static INT32 thunk(RE::ThirdPersonState* a_state)
			{
				INT32 ret = func(a_state);
				GetSingleton()->UpdateCamera(a_state);
				return ret;
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		static void Install()
		{
			stl::write_vfunc<0x3, Camera_Update>(RE::VTABLE_ThirdPersonState[0]);
		//	stl::write_thunk_call<Camera_Update>(REL::RelocationID(49852, 49852).address() + REL::Relocate(0x1A6, 0x1A6, 0x1A6));  // 1.5.97 1405FA1B0

		}
	};

private:
	constexpr CameraShakeManager() noexcept = default;
	CameraShakeManager(const CameraShakeManager&) = delete;
	CameraShakeManager(CameraShakeManager&&) = delete;

	~CameraShakeManager() = default;

	CameraShakeManager& operator=(const CameraShakeManager&) = delete;
	CameraShakeManager& operator=(CameraShakeManager&&) = delete;
};
