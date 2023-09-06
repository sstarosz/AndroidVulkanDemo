#include "Camera.hpp"


namespace st::renderer
{
	Camera::Camera():
		m_currentState(Camera::Actions::NoAction),
		m_eye(0.0F, 0.0F, 2.0F),
		m_center(0.0F, 0.0F, 0.0F),
		m_up(0.0F, 1.0F, 0.0F),
		m_matrix(),
		m_mouseClickX(0.0F),
		m_mouseClickY(0.0F),
		m_cameraHeight(500),
		m_cameraWidth(800)
	{
		update();
	}


	inline float sign(float s)
	{
		return (s < 0.f) ? -1.f : 1.f;
	}


	void Camera::mousePressEvent(int64_t x, int64_t y, Actions action)
	{
		m_currentState = action;
		m_mouseClickX = static_cast<float>(x);
		m_mouseClickY = static_cast<float>(y);
	}

	void Camera::mouseMove(int64_t x, int64_t y)
	{
		float localX = static_cast<float>(x - m_mouseClickX) / static_cast<float>(m_cameraWidth);
		float localY = static_cast<float>(y - m_mouseClickY) / static_cast<float>(m_cameraHeight);

		switch (m_currentState)
		{
		case Camera::Actions::NoAction:
			return;
			break;
		case Camera::Actions::Orbit:
			orbit(localX, localY);
			break;
		case Camera::Actions::Zoom:
			dolly(localX, localY);
			break;
		case Camera::Actions::Pan:
			pan(localX, localY);
			break;
		default:
			break;
		}

		update();

		m_mouseClickX = static_cast<float>(x);
		m_mouseClickY = static_cast<float>(y);
	}

	void Camera::releaseMouseClick()
	{
		m_currentState = Camera::Actions::NoAction;
	}

	void Camera::orbit(float dx, float dy)
	{
		using namespace math;

		if (dx == 0 && dy == 0)
		{
			return;
		}

		// Full width will do a full turn
		dx *= 2 * std::numbers::pi_v<float>;
		dy *= 2 * std::numbers::pi_v<float>;

		// Get the camera
		Vector3 origin = m_center;
		Vector3 position = m_eye;

		// Get the length of sight
		Vector3 centerToEye { position - origin };
		float radius = Vector3::length(centerToEye);
		centerToEye = Vector3::normalize(centerToEye);


		// Find the rotation around the UP axis (Y)
		Vector3 axeZ(Vector3::normalize(centerToEye));
		//Problem hire
		Matrix4x4 rotY = Matrix4x4::rotationAroundAxis(-dx, m_up);

		// Apply the (Y) rotation to the eye-center vector
		Vector4 vect_temp = rotY * Vector4(centerToEye, 0.0F);
		centerToEye = Vector3(vect_temp.X, vect_temp.Y, vect_temp.Z);


		// Find the rotation around the X vector: cross between eye-center and up (X)
		Vector3 axeX = Vector3::crossProduct(m_up, axeZ);
		axeX = Vector3::normalize(axeX);
		Matrix4x4 rotX = Matrix4x4::rotationAroundAxis(-dy, axeX);

		// Apply the (X) rotation to the eye-center vector
		vect_temp = rotX * Vector4(centerToEye, 0.0F);
		Vector3 vectRot { vect_temp.X, vect_temp.Y, vect_temp.Z };

		if (sign(vectRot.X) == sign(centerToEye.X))
		{
			centerToEye = vectRot;
		}

		centerToEye *= radius;

		m_eye = centerToEye + origin;
	}


	void Camera::dolly(float dx, float dy)
	{
		using namespace math;
		Vector3 z = m_center - m_eye;
		const float lenght = Vector3::length(z);

		if (lenght < 0.000001F)
		{
			return;
		}

		const float dd = std::fabs(dx) > std::fabs(dx) ? dx : -dy;

		float factor = 3.0F * dd;

		// Adjust speed based on distance.
		if (factor >= 1.0F)
		{
			return;
		}
		z *= factor;

		m_eye = m_eye + z;
	}


	void Camera::pan(float dx, float dy)
	{
		using namespace math;

		Vector3 z(m_eye - m_center);
		float length = static_cast<float>(Vector3::length(z)) / 0.785f; // 45 degrees
		z = Vector3::normalize(z);
		Vector3 x = Vector3::crossProduct(m_up, z);
		x = Vector3::normalize(x);
		Vector3 y = Vector3::crossProduct(z, x);
		y = Vector3::normalize(y);
		x *= -dx * length;
		y *= dy * length;

		m_eye = m_eye + x + y;
		m_center = m_center + x + y;
	}


	math::Vector3 Camera::orbitTest(float dx, float dy)
	{
		using namespace math;

		if (dx == 0 && dy == 0)
		{
			return {};
		}

		// Full width will do a full turn
		dx *= 2 * std::numbers::pi_v<float>;
		dy *= 2 * std::numbers::pi_v<float>;

		// Get the camera
		Vector3 origin = m_center;
		Vector3 position = m_eye;

		// Get the length of sight
		Vector3 centerToEye { position - origin };
		float radius = Vector3::length(centerToEye);
		centerToEye = Vector3::normalize(centerToEye);


		// Find the rotation around the UP axis (Y)
		Vector3 axeZ(Vector3::normalize(centerToEye));
		//Problem hire
		Matrix4x4 rotY = Matrix4x4::rotationAroundAxis(-dx, m_up);

		// Apply the (Y) rotation to the eye-center vector
		Vector4 vect_temp = rotY * Vector4(centerToEye, 0.0F);
		centerToEye = Vector3(vect_temp.X, vect_temp.Y, vect_temp.Z);


		// Find the rotation around the X vector: cross between eye-center and up (X)
		Vector3 axeX = Vector3::crossProduct(m_up, axeZ);
		axeX = Vector3::normalize(axeX);
		Matrix4x4 rotX = Matrix4x4::rotationAroundAxis(-dy, axeX);

		// Apply the (X) rotation to the eye-center vector
		vect_temp = rotX * Vector4(centerToEye, 0.0F);
		Vector3 vectRot { vect_temp.X, vect_temp.Y, vect_temp.Z };

		if (sign(vectRot.X) == sign(centerToEye.X))
		{
			centerToEye = vectRot;
		}

		centerToEye *= radius;

		Vector3 newPosition = centerToEye + origin;
		m_eye = newPosition;
		return newPosition;
	}

	void Camera::update()
	{
		m_matrix = lookAt(m_eye, m_center, m_up);
	}

	math::Matrix4x4 Camera::getViewMatrix() const
	{
		return m_matrix;
	}

	math::Matrix4x4 Camera::lookAt(const math::Vector3& eye, const math::Vector3& center, const math::Vector3& up)
	{
		using namespace math;

		Matrix4x4 result;
		Vector3 x;
		Vector3 y;
		Vector3 z;

		//Z vector
		//TODO substract vectors
		z.X = eye.X - center.X;
		z.Y = eye.Y - center.Y;
		z.Z = eye.Z - center.Z;
		z = Vector3::normalize(z);


		//Y vector
		y.X = up.X;
		y.Y = up.Y;
		y.Z = up.Z;

		x = Vector3::crossProduct(y, z);

		y = Vector3::crossProduct(z, x);


		x = Vector3::normalize(x);
		y = Vector3::normalize(y);


		result[0] = x.X;
		result[1] = x.Y;
		result[2] = x.Z;
		result[3] = -x.X * eye.X - x.Y * eye.Y - x.Z * eye.Z;

		result[4] = y.X;
		result[5] = y.Y;
		result[6] = y.Z;
		result[7] = -y.X * eye.X - y.Y * eye.Y - y.Z * eye.Z;

		result[8] = z.X;
		result[9] = z.Y;
		result[10] = z.Z;
		result[11] = -z.X * eye.X - z.Y * eye.Y - z.Z * eye.Z;


		result[12] = 0.0F;
		result[13] = 0.0F;
		result[14] = 0.0F;
		result[15] = 1.0F;

		return result;
	}

	math::Matrix4x4 Camera::getProjectionMatrix(float fovy, float aspect, float nearPlane, float farPlane) const
	{
		math::Matrix4x4 result;

		float f = farPlane;
		float n = nearPlane;

		const float t = n * std::tan(fovy * (std::numbers::pi_v<float> / 180.0F) * 0.5F);
		const float b = -t;
		const float l = b * aspect;
		const float r = t * aspect;


		//Frist row
		result[0] = (2 * n) / (r - l);
		result[1] = 0.0F;
		result[2] = (r + l) / (r - l);
		result[3] = 0.0F;

		//Second row
		result[4] = 0.0F;
		result[5] = -(2 * n) / (t - b);
		result[6] = (t + b) / (t - b);
		result[7] = 0.0F;

		//Third row
		result[8] = 0.0F;
		result[9] = 0.0F;
		result[10] = -(f) / (f - n);
		result[11] = (f * n) / (n - f);

		//Fourth row
		result[12] = 0.0F;
		result[13] = 0.0F;
		result[14] = -1.0F;
		result[15] = 0.0F;

		return result;
	}
}
