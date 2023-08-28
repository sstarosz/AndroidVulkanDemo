#include "Vector3.hpp"

#include <cmath>

namespace st::math
{
	//Also know as magnitude
	float Vector3::length(const Vector3& Vec) //TODO make it constexpr
	{
		return std::sqrt(Vec.X * Vec.X + 
						  Vec.Y * Vec.Y +
						  Vec.Z * Vec.Z);
	}

	//divide vector by it magnitude to make it a unit vector
	Vector3 Vector3::normalize(const Vector3& Vec) //TODO make it constexpr
	{
		float norm = Vector3::length(Vec);
		
		if (norm > 10e-6) // TODO change to elipson
		{
			norm = 1.0F / norm;
		}
		else
		{
			norm = 0.0F;
		}

		return { Vec * norm };
	}

	float Vector3::dotProduct(const Vector3& v1, const Vector3& v2)
	{
		float dotProduct = v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
		return dotProduct;
	}


	Vector3 Vector3::reflect(const Vector3& incident, const Vector3& normal)
	{
		float dotProduct = Vector3::dotProduct(incident, normal);
		Vector3 reflection = incident - 2.0f * dotProduct * normal;
		return reflection;
	}
}


