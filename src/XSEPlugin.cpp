
#include "Hooks.h"
#include "CameraNoiseManager.h"

#include "ENB/ENBSeriesAPI.h"
ENB_API::ENBSDKALT1001* g_ENB;

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostLoad:
		g_ENB = reinterpret_cast<ENB_API::ENBSDKALT1001*>(ENB_API::RequestENBAPI(ENB_API::SDKVersion::V1001));
		if (g_ENB) {
			logger::info("Obtained ENB API");
			g_ENB->SetCallbackFunction([](ENBCallbackType calltype) {
				switch (calltype) {
				case ENBCallbackType::ENBCallback_PostLoad:
					CameraNoiseManager::GetSingleton()->RefreshUI();
					CameraNoiseManager::GetSingleton()->LoadINI();
					break;
				case ENBCallbackType::ENBCallback_PreSave:
					CameraNoiseManager::GetSingleton()->SaveINI();
					break;
				case ENBCallbackType::ENBCallback_PostReset:
					CameraNoiseManager::GetSingleton()->RefreshUI();
					break;
				}
			});
		} else
			logger::info("Unable to acquire ENB API");
		break;
	}
}
void Init()
{
	SKSE::GetMessagingInterface()->RegisterListener(MessageHandler);
	Hooks::Install();
}

bool LoadINI(RE::StaticFunctionTag*, RE::BSFixedString filepath)
{
	CameraNoiseManager::GetSingleton()->LoadCustomINI(filepath, false);
	return true;
}


bool UnloadINI(RE::StaticFunctionTag*, RE::BSFixedString filepath)
{
	CameraNoiseManager::GetSingleton()->LoadCustomINI(filepath, true);
	return true;
}

bool ResetINI(RE::StaticFunctionTag*)
{
	CameraNoiseManager::GetSingleton()->LoadINI();
	return true;
}

bool RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm)
{
	a_vm->RegisterFunction("LoadIni"sv, "CameraNoise"sv, LoadINI);
	a_vm->RegisterFunction("UnloadIni"sv, "CameraNoise"sv, UnloadINI);
	a_vm->RegisterFunction("ResetIni"sv, "CameraNoise"sv, ResetINI);
	return true;
}

void SaveNoiseData(SKSE::SerializationInterface* a_intfc)
{
	std::vector<float> _data = CameraNoiseManager::GetSingleton()->GetData();
	if (!a_intfc->OpenRecord('ARR_', 1)) {
		logger::error("Failed to open record for noise data.");
	} else {
		std::size_t size = _data.size();
		if (!a_intfc->WriteRecordData(size)) {
			logger::error("Failed to write size for noise data.");
		} else {
			for (float& value : _data) {
				if (!a_intfc->WriteRecordData(value)) {
					logger::error("Failed to write value(s) for noise data.");
					break;
				}
			}
		}
	}
}

void LoadNoiseData(SKSE::SerializationInterface* a_intfc)
{
	std::vector<float> _data;

	std::uint32_t type;
	std::uint32_t version;
	std::uint32_t length;

	while (a_intfc->GetNextRecordInfo(type, version, length)) {
		switch (type) {
		case 'ARR_':
			{
				std::size_t size;
				if (!a_intfc->ReadRecordData(size)) {
					logger::error("Failed to load size of noise data.");
				}
				for (std::uint32_t i = 0; i < size; i++) {
					float value;
					if (!a_intfc->ReadRecordData(value)) {
						logger::error("Failed to load value(s) of noise data.");
					} else {
						_data.push_back(value);
					}
				}
			}
		}
	}

	if (!_data.empty()) {
		CameraNoiseManager::GetSingleton()->Set_Data(_data);
	}
}

void InitializeLog()
{
#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		util::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= fmt::format("{}.log"sv, Plugin::NAME);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
	const auto level = spdlog::level::trace;
#else
	const auto level = spdlog::level::trace;
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
	log->set_level(level);
	log->flush_on(level);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%l] %v"s);
}

EXTERN_C [[maybe_unused]] __declspec(dllexport) bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
#ifndef NDEBUG
	while (!IsDebuggerPresent()) {};
#endif

	InitializeLog();

	logger::info("Loaded plugin");

	SKSE::Init(a_skse);

	Init();

	const auto papyrus = SKSE::GetPapyrusInterface();

	papyrus->Register(RegisterFuncs);

	auto serialization = SKSE::GetSerializationInterface();
	serialization->SetUniqueID('CNOI');
	serialization->SetSaveCallback(SaveNoiseData);
	serialization->SetLoadCallback(LoadNoiseData);

	return true;
}

EXTERN_C [[maybe_unused]] __declspec(dllexport) constinit auto SKSEPlugin_Version = []() noexcept {
	SKSE::PluginVersionData v;
	v.PluginName("PluginName");
	v.PluginVersion({ 1, 0, 0, 0 });
	v.UsesAddressLibrary(true);
	v.HasNoStructUse();
	return v;
}();

EXTERN_C [[maybe_unused]] __declspec(dllexport) bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* pluginInfo)
{
	pluginInfo->name = SKSEPlugin_Version.pluginName;
	pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
	pluginInfo->version = SKSEPlugin_Version.pluginVersion;
	return true;
}
