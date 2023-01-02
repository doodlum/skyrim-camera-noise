
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
	std::vector<float> interp(12);
	std::fill(interp.begin(), interp.end(), 0.0f);
	CameraNoiseManager::GetSingleton()->SetData(interp, true);
	Hooks::Install();
}

bool LoadINI(RE::StaticFunctionTag*, RE::BSFixedString filepath)
{
	return CameraNoiseManager::GetSingleton()->LoadCustomINI(filepath, false);
}


bool UnloadINI(RE::StaticFunctionTag*, RE::BSFixedString filepath)
{
	return CameraNoiseManager::GetSingleton()->LoadCustomINI(filepath, true);
}

bool ResetINI(RE::StaticFunctionTag*)
{
	CameraNoiseManager::GetSingleton()->LoadINI();
	CameraNoiseManager::GetSingleton()->inis.clear();
	return true;
}

bool RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm)
{
	a_vm->RegisterFunction("LoadIni"sv, "CameraNoise"sv, LoadINI);
	a_vm->RegisterFunction("UnloadIni"sv, "CameraNoise"sv, UnloadINI);
	a_vm->RegisterFunction("ResetIni"sv, "CameraNoise"sv, ResetINI);
	return true;
}

void WriteNoiseData(SKSE::SerializationInterface* a_intfc, std::vector<float>& noise_data, uint32_t label) 
{
	if (!a_intfc->OpenRecord(label, 1)) {
		logger::error("Failed to open record for noise data.");
	} else {
		std::size_t size = noise_data.size();
		if (!a_intfc->WriteRecordData(size)) {
			logger::error("Failed to write size for noise data.");
		} else {
			for (float& value : noise_data) {
				if (!a_intfc->WriteRecordData(value)) {
					logger::error("Failed to write value(s) for noise data.");
					break;
				}
			}
		}
	}
}

void SaveNoiseData(SKSE::SerializationInterface* a_intfc)
{
	std::vector<float> _data = CameraNoiseManager::GetSingleton()->GetData();
	WriteNoiseData(a_intfc, _data, 'ARR_');

	if (CameraNoiseManager::GetSingleton()->bInterpolation) {
		if (!a_intfc->WriteRecord('ITB_', 1, CameraNoiseManager::GetSingleton()->bInterpolation)) {
			logger::error("Failed to write interpolation.");
		}
		std::vector<float> _interp_data = CameraNoiseManager::GetSingleton()->GetData(true);
		WriteNoiseData(a_intfc, _interp_data, 'ITR_');
	}

	std::unordered_set<std::string> inis = CameraNoiseManager::GetSingleton()->inis;
	if (!a_intfc->OpenRecord('INI_', 1)) {
		logger::error("Failed to open record for loaded inis.");
	} else {
		std::size_t size = inis.size();
		if (!a_intfc->WriteRecordData(size)) {
			logger::error("Failed to write size for loaded inis.");
		} else {
			const std::size_t c_size = sizeof(char);
			for (auto& a_ini : inis) {
				const char* c_data = a_ini.c_str();
				std::size_t iniLength = strlen(c_data);
				if (!a_intfc->WriteRecordData(iniLength)) {
					logger::error("Failed to write length of ini file path.");
				} else {
					if (!a_intfc->WriteRecordData(c_data, iniLength * c_size)) {
						logger::error("Failed to write ini.");
					}
				}
			}
		}
	}
}

void ReadNoiseData(SKSE::SerializationInterface* a_intfc, std::vector<float>& noise_data)
{
	std::size_t size;
	if (!a_intfc->ReadRecordData(size)) {
		logger::error("Failed to load size of noise data.");
	} else {
		for (std::uint32_t i = 0; i < size; i++) {
			float value;
			if (!a_intfc->ReadRecordData(value)) {
				logger::error("Failed to load value(s) of noise data.");
			} else {
				noise_data.push_back(value);
			}
		}
	}
}

void LoadNoiseData(SKSE::SerializationInterface* a_intfc)
{
	std::vector<float> _data;
	bool was_interpolating = false;
	std::vector<float> _interp_data;
	std::unordered_set<std::string> _inis;

	std::uint32_t type;
	std::uint32_t version;
	std::uint32_t length;
	while (a_intfc->GetNextRecordInfo(type, version, length)) {
		switch (type) {
		case 'ARR_':
			ReadNoiseData(a_intfc, _data);
			break;
		case 'ITB_':
			if (!a_intfc->ReadRecordData(was_interpolating)) {
				logger::error("Failed to load interpolation.");
			}
			break;
		case 'ITR_':
			ReadNoiseData(a_intfc, _interp_data);
			break;
		case 'INI_':
			std::size_t iniSize;
			if (!a_intfc->ReadRecordData(iniSize)) {
				logger::error("Failed to load size of loaded inis.");
			} else {
				const std::size_t c_size = sizeof(char);
				for (std::uint32_t i = 0; i < iniSize; i++) {
					std::size_t a_iniLength;
					if (!a_intfc->ReadRecordData(a_iniLength)) {
						logger::error("Failed to load length of ini file path.");
					} else {
						char* a_ini = new char[a_iniLength];
						if (!a_intfc->ReadRecordData(a_ini, a_iniLength * c_size)) {
							logger::error("Failed to load ini.");
						} else {
							a_ini[a_iniLength] = '\0';
							std::string str_ini = std::string(a_ini);
							delete[] a_ini;
							_inis.insert(str_ini);
						}
					}
				}
			}
			break;
		default:
			logger::error("Unrecognized signature type!");
			break;
		}
	}

	if (!_data.empty()) {
		CameraNoiseManager::GetSingleton()->SetData(_data);
	}

	if (was_interpolating) {
		CameraNoiseManager::GetSingleton()->bInterpolation = true;
		if (!_interp_data.empty()) {
			CameraNoiseManager::GetSingleton()->SetData(_interp_data, true);
		}
	}

	if (!_inis.empty()) {
		CameraNoiseManager::GetSingleton()->inis = std::move(_inis);
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
