#include "Matrix4x4.hpp"
#include <cmath>

namespace st::math
{
	Matrix4x4::Matrix4x4() noexcept : m_value() { }

	Matrix4x4::Matrix4x4(float xa, float xb, float xc, float xd,
						 float ya, float yb, float yc, float yd,
						 float za, float zb, float zc, float zd,
						 float wa, float wb, float wc, float wd) noexcept
	{
		m_value[0]  = xa;
		m_value[1]  = xb;
		m_value[2]  = xc;
		m_value[3]  = xd;
		m_value[4]  = ya;
		m_value[5]  = yb;
		m_value[6]  = yc;
		m_value[7]  = yd;
		m_value[8]  = za;
		m_value[9]  = zb;
		m_value[10] = zc;
		m_value[11] = zd;
		m_value[12] = wa;
		m_value[13] = wb;
		m_value[14] = wc;
		m_value[15] = wd;
	}

	Matrix4x4 Matrix4x4::indentityMatrix()
	{
		return Matrix4x4(1.0, 0.0, 0.0, 0.0,
						 0.0, 1.0, 0.0, 0.0,
						 0.0, 0.0, 1.0, 0.0,
						 0.0, 0.0, 0.0, 1.0);
	}

	void Matrix4x4::translate(const Vector3& vector)
	{
		m_value[3]  += vector.X;
		m_value[7]  += vector.Y;
		m_value[11] += vector.Z;
	}

	Matrix4x4 Matrix4x4::projectionMatrix(float fieldOfView, float framebufferAspectRatio, float nearPlane, float farPlane)
	{
		//User se word in x right y up, z into screen
		Matrix4x4 result = Matrix4x4::indentityMatrix();

		result[5] = -1;
		result[10] = -1;

		Matrix4x4 projection{};
		projection[0] = std::pow(fieldOfView, -1) / std::tan(fieldOfView / 2.0F);
		projection[5] = 1 * std::tan(fieldOfView / 2.0F);
		projection[10] = farPlane / (farPlane - nearPlane);
		projection[11] = -nearPlane * (farPlane - nearPlane);
		projection[14] = 1;


		projection *= result;

		return projection;
	}

	Matrix4x4 Matrix4x4::rotationX(const float& theta)
	{
		return Matrix4x4();
	}

	Matrix4x4 Matrix4x4::rotationY(const float& theta)
	{
		return Matrix4x4();
	}

	Matrix4x4 Matrix4x4::rotationZ(const float& theta)
	{
		return Matrix4x4();
	}

	Matrix4x4 Matrix4x4::rotationAroundAxis(const float& theta, const Vector3& v)
	{
		const float cosT = std::cos(theta);
		const float sinT = std::sin(theta);

		float xx = v.X * v.X;
		float yy = v.Y * v.Y;
		float zz = v.Z * v.Z;
		float xy = v.X * v.Y;
		float xz = v.X * v.Z;
		float yz = v.Y * v.Z;


		Matrix4x4 result{};

		result[0]  = cosT + xx * (1 - cosT);
		result[1]  = xy * (1 - cosT) - v.Z * sinT;
		result[2]  = xz * (1 - cosT) + v.Y * sinT;

		result[4]  = xy * (1 - cosT) + v.Z * sinT;
		result[5]  = cosT + yy * (1 - cosT);
		result[6]  = yz * (1 - cosT) - v.X * sinT;

		result[8]  = xz * (1 - cosT) - v.Y * sinT;
		result[9]  = yz * (1 - cosT) + v.X * sinT;
		result[10] = cosT + zz * (1 - cosT);



		//4-th column
		result[3] = 0.0;
		result[7] = 0.0;
		result[11] = 0.0;

		//4-th row
		result[12] = 0;
		result[13] = 0;
		result[14] = 0;
		result[15] = 1;


		return result;
	}

	void Matrix4x4::convertToColumnMajor()
	{
		std::swap(m_value[1], m_value[4]);
		std::swap(m_value[2], m_value[8]);
		std::swap(m_value[3], m_value[12]);
		std::swap(m_value[6], m_value[9]);
		std::swap(m_value[7], m_value[13]);
		std::swap(m_value[11], m_value[14]);
	}

	

}
