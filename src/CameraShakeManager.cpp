#include "CameraShakeManager.h"
#include "mmath.h"

void SetCameraRotation(mmath::Rotation& rot, RE::PlayerCamera* camera) noexcept
{
	auto mtx = glm::rotate(glm::identity<glm::mat4>(), -mmath::half_pi, { 1.0f, 0.0f, 0.0f });
	mtx = glm::rotate(mtx, -rot.euler.x - mmath::half_pi, { 0.0f, 1.0f, 0.0f });
	mtx = glm::rotate(mtx, rot.euler.y - mmath::half_pi, { 0.0f, 0.0f, 1.0f });

	RE::NiMatrix3 cameraNiT;
	cameraNiT.entry[0][0] = mtx[0][0];
	cameraNiT.entry[0][1] = mtx[0][1];
	cameraNiT.entry[0][2] = mtx[0][2];
	cameraNiT.entry[1][0] = mtx[1][0];
	cameraNiT.entry[1][1] = mtx[1][1];
	cameraNiT.entry[1][2] = mtx[1][2];
	cameraNiT.entry[2][0] = mtx[2][0];
	cameraNiT.entry[2][1] = mtx[2][1];
	cameraNiT.entry[2][2] = mtx[2][2];

	RE::NiMatrix3 cameraNodeT;
	cameraNodeT.entry[0][1] = cameraNiT.entry[0][0];
	cameraNodeT.entry[0][2] = cameraNiT.entry[0][1];
	cameraNodeT.entry[0][0] = cameraNiT.entry[0][2];
	cameraNodeT.entry[1][1] = cameraNiT.entry[1][0];
	cameraNodeT.entry[1][2] = cameraNiT.entry[1][1];
	cameraNodeT.entry[1][0] = cameraNiT.entry[1][2];
	cameraNodeT.entry[2][1] = cameraNiT.entry[2][0];
	cameraNodeT.entry[2][2] = cameraNiT.entry[2][1];
	cameraNodeT.entry[2][0] = cameraNiT.entry[2][2];

	camera->cameraRoot->world.rotate = cameraNodeT;
	camera->cameraRoot->local.rotate = cameraNodeT;

	const auto tps = reinterpret_cast<RE::ThirdPersonState*>(camera->currentState.get());
	tps->rotation = rot.ToNiQuat();
}


void CameraShakeManager::UpdateCamera(RE::ThirdPersonState* a_state)
{
	auto camera = a_state->camera;
	timeElapsed += 0.0166 * FREQUENCY;

	mmath::Rotation cameraRotation;
	cameraRotation.SetQuaternion(a_state->rotation);
	cameraRotation.SetEuler(0.5, 0.1);
	cameraRotation.UpdateQuaternion();

	SetCameraRotation(cameraRotation, RE::PlayerCamera::GetSingleton());





	//camera->pos.x = perlin1.noise1D(timeElapsed);
	//camera->pos.y = perlin2.noise1D(timeElapsed);
	//camera->pos.z = perlin3.noise1D(timeElapsed);
	//camera->rotationInput.x = perlin4.noise1D(timeElapsed);
	//camera->rotationInput.y = perlin5.noise1D(timeElapsed);
	//camera->yaw = perlin6.noise1D(timeElapsed);
//	camera->translationInput.x += perlin1.noise1D(timeElapsed) * AMPLITUDE;
	//camera->translationInput.y += perlin2.noise1D(timeElapsed) * AMPLITUDE;
	//camera->translationInput.z += perlin3.noise1D(timeElapsed) * AMPLITUDE;
	//RE::NiPoint3 rotation;
	//auto node = camera->cameraRoot.get();
	//node->local.rotate.ToEulerAnglesXYZ(rotation);
	//node->local.rotate.SetEulerAnglesXYZ(
	//	5555, 
	//	5555, 
	//	5555
	//);
	//RE::NiUpdateData niupdate;
	//node->UpdateUpwardPass(niupdate);
	// 

	//node->local.rotate.SetEulerAnglesXYZ(
	//	rotation.x + perlin1.noise1D(timeElapsed) * 555555,
	//	rotation.y + perlin2.noise1D(timeElapsed) * 55555,
	//	rotation.z + perlin3.noise1D(timeElapsed) * 55555);
										
}
