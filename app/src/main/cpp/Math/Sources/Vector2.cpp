#include "Vector2.hpp"

#include <cmath>

namespace st::math
{
	//Also know as magnitude
	float Vector2::length(const Vector2& Vec) //TODO make it constexpr
	{
		return std::sqrt(Vec.X * Vec.X + 
						  Vec.Y * Vec.Y);
	}

	//divide vector by it magnitude to make it a unit vector
	Vector2 Vector2::normalize(const Vector2& Vec) //TODO make it constexpr
	{
		float norm = Vector2::length(Vec);
		
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

	float Vector2::dotProduct(const Vector2& v1, const Vector2& v2)
	{
		float dotProduct = v1.X * v2.X + v1.Y * v2.Y;
		return dotProduct;
	}


	Vector2 Vector2::reflect(const Vector2& incident, const Vector2& normal)
	{
		float dotProduct = Vector2::dotProduct(incident, normal);
		Vector2 reflection = incident - 2.0f * dotProduct * normal;
		return reflection;
	}
}


