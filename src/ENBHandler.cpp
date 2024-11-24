#include "ENB/ENBSeriesAPI.h"
#include "ENBHandler.h"
#include "CameraNoiseManager.h"

ENB_API::ENBSDKALT1001* g_ENB;

#define TWDEF "group = 'MOD:Camera Noise' precision = 2 step = 0.01 "
#define TWDEF2 "group = 'First Person' precision = 2 step = 0.01 "
#define TWDEF3 "group = 'Third Person' precision = 2 step = 0.01 "
#define TWDEF4 "group = 'Interpolation' precision = 2 step = 0.01 "

void ENBHandler::RefreshUI()
{
	auto camera_manager = CameraNoiseManager::GetSingleton();
	auto bar = g_ENB->TwGetBarByEnum(!REL::Module::IsVR() ? ENB_API::ENBWindowType::EditorBarEffects : ENB_API::ENBWindowType::EditorBarObjects);  // ENB misnames its own bar, whoops!

	g_ENB->TwAddVarRW(bar, "EnableCameraNoise", ETwType::TW_TYPE_BOOLCPP, &camera_manager->bEnabled, TWDEF);

	g_ENB->TwAddVarRW(bar, "1PFrequency1", ETwType::TW_TYPE_FLOAT, &camera_manager->FirstPerson.fFrequency1, TWDEF2 " label = 'fFrequency1 (Translation)'");
	g_ENB->TwAddVarRW(bar, "1PFrequency2", ETwType::TW_TYPE_FLOAT, &camera_manager->FirstPerson.fFrequency2, TWDEF2 " label = 'fFrequency2 (Rotation)'");
	g_ENB->TwAddVarRW(bar, "1PFrequency3", ETwType::TW_TYPE_FLOAT, &camera_manager->FirstPerson.fFrequency3, TWDEF2 " label = 'fFrequency3 (Rotation)'");
	g_ENB->TwAddVarRW(bar, "1PAmplitude1", ETwType::TW_TYPE_FLOAT, &camera_manager->FirstPerson.fAmplitude1, TWDEF2 " label = 'fAmplitude1 (Rotation)'");
	g_ENB->TwAddVarRW(bar, "1PAmplitude2", ETwType::TW_TYPE_FLOAT, &camera_manager->FirstPerson.fAmplitude2, TWDEF2 " label = 'fAmplitude2 (Rotation)'");
	g_ENB->TwAddVarRW(bar, "1PAmplitude3", ETwType::TW_TYPE_FLOAT, &camera_manager->FirstPerson.fAmplitude3, TWDEF2 " label = 'fAmplitude3 (Rotation)'");

	g_ENB->TwAddVarRW(bar, "3PFrequency1", ETwType::TW_TYPE_FLOAT, &camera_manager->ThirdPerson.fFrequency1, TWDEF3 " label = 'fFrequency1 (Translation)'");
	g_ENB->TwAddVarRW(bar, "3PFrequency2", ETwType::TW_TYPE_FLOAT, &camera_manager->ThirdPerson.fFrequency2, TWDEF3 " label = 'fFrequency2 (Rotation)'");
	g_ENB->TwAddVarRW(bar, "3PFrequency3", ETwType::TW_TYPE_FLOAT, &camera_manager->ThirdPerson.fFrequency3, TWDEF3 " label = 'fFrequency3 (Rotation)'");
	g_ENB->TwAddVarRW(bar, "3PAmplitude1", ETwType::TW_TYPE_FLOAT, &camera_manager->ThirdPerson.fAmplitude1, TWDEF3 " label = 'fAmplitude1 (Translation)'");
	g_ENB->TwAddVarRW(bar, "3PAmplitude2", ETwType::TW_TYPE_FLOAT, &camera_manager->ThirdPerson.fAmplitude2, TWDEF3 " label = 'fAmplitude2 (Rotation)'");
	g_ENB->TwAddVarRW(bar, "3PAmplitude3", ETwType::TW_TYPE_FLOAT, &camera_manager->ThirdPerson.fAmplitude3, TWDEF3 " label = 'fAmplitude3 (Rotation)'");

	g_ENB->TwAddVarRW(bar, "InterpolationX", ETwType::TW_TYPE_UINT32, &camera_manager->iInterpolationX, TWDEF4 " label = 'iInterpolationX (Interpolation)'");
	g_ENB->TwAddVarRW(bar, "InterpolationY", ETwType::TW_TYPE_UINT32, &camera_manager->iInterpolationY, TWDEF4 " label = 'iInterpolationY (Interpolation)'");

	g_ENB->TwDefine("EditorBarEffects/'First Person' group = 'MOD:Camera Noise'");
	g_ENB->TwDefine("EditorBarEffects/'Third Person' group = 'MOD:Camera Noise'");
	g_ENB->TwDefine("EditorBarEffects/'Interpolation' group = 'MOD:Camera Noise'");
	g_ENB->TwDefine("EditorBarEffects/'MOD:Camera Noise' opened=false");
}

void ENBHandler::MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostLoad:
		g_ENB = reinterpret_cast<ENB_API::ENBSDKALT1001*>(ENB_API::RequestENBAPI(ENB_API::SDKVersion::V1001));
		if (g_ENB) {
			logger::info("Obtained ENB API");
			g_ENB->SetCallbackFunction([](ENBCallbackType calltype) {
				switch (calltype) {
				case ENBCallbackType::ENBCallback_PostLoad:
					RefreshUI();
					CameraNoiseManager::GetSingleton()->LoadINI();
					break;
				case ENBCallbackType::ENBCallback_PreSave:
					CameraNoiseManager::GetSingleton()->SaveINI();
					break;
				case ENBCallbackType::ENBCallback_PostReset:
					RefreshUI();
					break;
				}
			});
		} else
			logger::info("Unable to acquire ENB API");
		break;
	}
}
