#pragma once

/*
* For modders: Copy this file into your own project if you wish to use this API
*/

namespace CameraNoise_API
{
	constexpr auto camera_noise_plugin_name = "CameraShake";

	enum class InterfaceVersion : uint8_t
	{
		V1
	};

	class CameraNoiseInterface
	{
	public:
		virtual bool LoadIni(std::string filepath) = 0;
		virtual bool UnloadIni(std::string filepath) = 0;
		virtual void ResetIni() = 0;
	};

	typedef void* (*_RequestPluginAPI)(const InterfaceVersion interface_version);

	[[nodiscard]] inline void* RequestPluginAPI(const InterfaceVersion a_interface_version = InterfaceVersion::V1) {
		const auto plugin_handle = GetModuleHandle(L"CameraShake.dll");
		if (const _RequestPluginAPI request_API_function = reinterpret_cast<_RequestPluginAPI>(GetProcAddress(plugin_handle, 
			"RequestPluginAPI"))) {
			return request_API_function(a_interface_version);
		}
		return nullptr;
	}
}
