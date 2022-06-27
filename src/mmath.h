#pragma once

namespace mmath {
	constexpr const float half_pi = 1.57079632679485f;

	typedef struct {
		float data[4][4];
	} NiMatrix44;

	bool IsInf(const float f) noexcept;
	bool IsInf(const double f) noexcept;
	bool IsInf(const glm::vec3& v) noexcept;
	bool IsInf(const glm::vec4& v) noexcept;

	bool IsNan(const float f) noexcept;
	bool IsNan(const double f) noexcept;
	bool IsNan(const glm::vec3& v) noexcept;
	bool IsNan(const glm::vec4& v) noexcept;

	bool IsValid(const float f) noexcept;
	bool IsValid(const double f) noexcept;
	bool IsValid(const glm::vec3& v) noexcept;
	bool IsValid(const glm::vec4& v) noexcept;

	// Pitch/Yaw rotation
	struct Rotation {
		public:
			glm::quat quat = glm::identity<glm::quat>();
			glm::vec2 euler = { 0.0f, 0.0f };

			// Set euler angles, call UpdateQuaternion to refresh the quaternion part
			void SetEuler(float pitch, float yaw) noexcept;

			// Set with a quaternion and update euler angles
			void SetQuaternion(const glm::quat& q) noexcept;
			void SetQuaternion(const RE::NiQuaternion& q) noexcept;

			// Copy rotation from a TESObjectREFR
			void CopyFrom(const RE::TESObjectREFR* ref) noexcept;

			// Compute a quaternion after setting euler angles
			void UpdateQuaternion() noexcept;

			// Get a quaternion pointing in the opposite direction (p/y inverted)
			glm::quat InverseQuat() const noexcept;
			RE::NiQuaternion InverseNiQuat() const noexcept;

			RE::NiQuaternion ToNiQuat() const noexcept;
			RE::NiPoint2 ToNiPoint2() const noexcept;
			RE::NiPoint3 ToNiPoint3() const noexcept;

			// Get a 4x4 rotation matrix
			glm::mat4 ToRotationMatrix() noexcept;

		private:
			glm::mat4 mat = glm::identity<glm::mat4>();
			bool dirty = true;
	};

	// Smooth changes in scalar overloads
	enum class Local {
		No,
		Yes
	};

}
