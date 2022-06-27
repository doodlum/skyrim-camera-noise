#pragma once

#pragma warning(push)
#pragma warning(disable: 5105)
#pragma warning(disable: 4189)
#if defined(FALLOUT4)
#	include "F4SE/F4SE.h"
#	include "RE/Fallout.h"
#	define SKSE F4SE
#	define SKSEAPI F4SEAPI
#	define SKSEPlugin_Load F4SEPlugin_Load
#	define SKSEPlugin_Query F4SEPlugin_Query
#	define RUNTIME RUNTIME_1_10_163
#else
#	include "RE/Skyrim.h"
#	include "SKSE/SKSE.h"
#	if defined(SKYRIMAE)
#		define RUNTIME 0
#	elif defined(SKYRIMVR)
#		define RUNTIME SKSE::RUNTIME_VR_1_4_15_1
#	else
#		define RUNTIME SKSE::RUNTIME_1_5_97
#	endif
#endif

#include <magic_enum.hpp>

#   define GLM_FORCE_INLINE
#   define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#   define GLM_FORCE_INTRINSICS
#   define GLM_FORCE_LEFT_HANDED
#   define GLM_FORCE_DEPTH_ZERO_TO_ONE
#   define GLM_FORCE_QUAT_DATA_WXYZ
#   define GLM_ENABLE_EXPERIMENTAL
#   include <glm/glm.hpp>
#   include <glm/gtc/matrix_transform.hpp>
#   include <glm/ext/matrix_clip_space.hpp>
#   include <glm/gtc/quaternion.hpp>
#   include <glm/gtc/constants.hpp>
#   include <glm/gtx/easing.hpp>
#   include <glm/gtx/spline.hpp>
#   include <glm/gtx/norm.hpp>
#   include <glm/gtx/hash.hpp>

using vec2u = glm::vec<2, float, glm::highp>;
using vec3u = glm::vec<3, float, glm::highp>;
using vec4u = glm::vec<4, float, glm::highp>;

using vec2ui = glm::vec<2, int, glm::highp>;
using vec3ui = glm::vec<3, int, glm::highp>;
using vec4ui = glm::vec<4, int, glm::highp>;

#include <ShlObj_core.h>
#include <Windows.h>

#ifdef NDEBUG
#	include <spdlog/sinks/basic_file_sink.h>
#else
#	include <spdlog/sinks/msvc_sink.h>
#endif

#pragma warning(pop)

using namespace std::literals;

namespace stl
{
	using namespace SKSE::stl;

	template <class T>
	void write_thunk_call(std::uintptr_t a_src)
	{
		SKSE::AllocTrampoline(14);

		auto& trampoline = SKSE::GetTrampoline();
		T::func = trampoline.write_call<5>(a_src, T::thunk);
	}

	template <class F, std::size_t idx, class T>
	void write_vfunc()
	{
		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[0] };
		T::func = vtbl.write_vfunc(idx, T::thunk);
	}

	template <std::size_t idx, class T>
	void write_vfunc(REL::VariantID id)
	{
		REL::Relocation<std::uintptr_t> vtbl{ id };
		T::func = vtbl.write_vfunc(idx, T::thunk);
	}

}

namespace logger = SKSE::log;

namespace util
{
	using SKSE::stl::report_and_fail;
}

#define DLLEXPORT __declspec(dllexport)

#include "Plugin.h"

#define _MESSAGE logger::info
#define _DMESSAGE logger::debug
#define _ERROR logger::error
#define _FATALERROR logger::critical

#define UInt32 uint32_t

