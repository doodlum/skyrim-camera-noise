#include "mmath.h"

bool mmath::IsInf(const float f) noexcept {
	return glm::isinf(f);
}
bool mmath::IsInf(const double f) noexcept {
	return glm::isinf(f);
}
bool mmath::IsInf(const glm::vec3& v) noexcept {
	return glm::isinf(v.x) || glm::isinf(v.y) || glm::isinf(v.z);
}
bool mmath::IsInf(const glm::vec4& v) noexcept {
	return glm::isinf(v.x) || glm::isinf(v.y) || glm::isinf(v.z) || glm::isinf(v.w);
}

bool mmath::IsNan(const float f) noexcept {
	return glm::isnan(f);
}
bool mmath::IsNan(const double f) noexcept {
	return glm::isnan(f);
}
bool mmath::IsNan(const glm::vec3& v) noexcept {
	return glm::isnan(v.x) || glm::isnan(v.y) || glm::isnan(v.z);
}
bool mmath::IsNan(const glm::vec4& v) noexcept {
	return glm::isnan(v.x) || glm::isnan(v.y) || glm::isnan(v.z) || glm::isnan(v.w);
}

bool mmath::IsValid(const float f) noexcept {
	return !mmath::IsInf(f) && !mmath::IsNan(f);
}
bool mmath::IsValid(const double f) noexcept {
	return !mmath::IsInf(f) && !mmath::IsNan(f);
}
bool mmath::IsValid(const glm::vec3& v) noexcept {
	return !mmath::IsInf(v) && !mmath::IsNan(v);
}
bool mmath::IsValid(const glm::vec4& v) noexcept {
	return !mmath::IsInf(v) && !mmath::IsNan(v);
}


void mmath::Rotation::SetEuler(float pitch, float yaw) noexcept {
	euler.x = pitch;
	euler.y = yaw;
	dirty = true;
}

void mmath::Rotation::SetQuaternion(const glm::quat& q) noexcept {
	quat = q;
	euler.x = glm::pitch(q) * -1.0f;
	euler.y = glm::roll(q) * -1.0f; // The game stores yaw in the Z axis
	dirty = true;
}

void mmath::Rotation::SetQuaternion(const RE::NiQuaternion& q) noexcept {
	SetQuaternion(glm::quat{ q.w, q.x, q.y, q.z });
}

void mmath::Rotation::CopyFrom(const RE::TESObjectREFR* ref) noexcept {
	SetEuler(ref->data.angle.x, ref->data.angle.z);
}

void mmath::Rotation::UpdateQuaternion() noexcept {
	quat = glm::quat(glm::vec3{ -euler.x, 0.0f, -euler.y });
}

glm::quat mmath::Rotation::InverseQuat() const noexcept {
	return glm::quat(glm::vec3{ euler.x, 0.0f, euler.y });
}

RE::NiQuaternion mmath::Rotation::InverseNiQuat() const noexcept {
	const auto q = InverseQuat();
	return { q.w, q.x, q.y, q.z };
}

RE::NiQuaternion mmath::Rotation::ToNiQuat() const noexcept {
	return { quat.w, quat.x, quat.y, quat.z };
}

RE::NiPoint2 mmath::Rotation::ToNiPoint2() const noexcept {
	return { euler.x, euler.y };
}

RE::NiPoint3 mmath::Rotation::ToNiPoint3() const noexcept {
	return { euler.x, 0.0f, euler.y };
}

glm::mat4 mmath::Rotation::ToRotationMatrix() noexcept {
	if (dirty) {
		mat = glm::identity<glm::mat4>();
		mat = glm::rotate(mat, -euler.y, { 0.0f, 0.0f, 1.0f }); // yaw
		mat = glm::rotate(mat, -euler.x, { 1.0f, 0.0f, 0.0f }); // pitch
		dirty = false;
	}
	return mat;
}
