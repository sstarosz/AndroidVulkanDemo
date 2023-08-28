#ifndef GEOMETRY_VECTOR4_HPP
#define GEOMETRY_VECTOR4_HPP

#include <compare>
#include <cassert>
#include "Vector3.hpp"
#include "Matrix4x4.hpp"

namespace st::math
{

    /*! \brief Point in 3d space
         *         Brief description continued.
         *
         *  Detailed description starts here.
         */
    class Vector4
    {
    public:
		constexpr explicit Vector4() noexcept:
		X(0.0F),
		Y(0.0F),
		Z(0.0F),
		W(0.0F)
		{}

		constexpr explicit Vector4(float x, float y, float z, float w) noexcept:
		X(x),
		Y(y),
		Z(z),
		W(w)
		{}

		constexpr explicit Vector4(Vector3 Vec, float w) noexcept:
		X(Vec.X),
		Y(Vec.Y),
		Z(Vec.Z),
		W(w)
		{}


        constexpr float& operator[](const size_t index)
        {
			assert(index >= 0 && index < 5);
			return ((&X)[index]); //TODO More safe version?
        }

        const float& operator[](const size_t index) const
        {
			assert(index >= 0 && index < 5);
			return ((&X)[index]); //TODO More safe version?
        }

        auto operator<=>(const Vector4&) const = default;


        constexpr Vector4 operator+(const Vector4& Vec) const
        {
			return Vector4 { X + Vec.X, Y + Vec.Y, Z + Vec.Z, W + Vec.W };
		}

        constexpr Vector4 operator-(const Vector4& Vec) const
		{
			return Vector4 { X - Vec.X, Y - Vec.Y, Z - Vec.Z, W - Vec.W };
		}


        //Operator with Scale
		constexpr Vector4 operator*(const float& Scale) const
		{
			return Vector4 { X * Scale, Y * Scale, Z * Scale, W * Scale };
		}


    public:
        float X;
		float Y;
		float Z;
		float W;

    };


    inline Vector4 operator*(const Matrix4x4& m, const Vector4 Vec)
    {
        Vector4 u;
        u[0] = m[0]  * Vec.X + m[1]  * Vec.Y + m[2]  * Vec.Z + m[3]  * Vec.Z;
        u[1] = m[4]  * Vec.X + m[5]  * Vec.Y + m[6]  * Vec.Z + m[7]  * Vec.Z;
        u[2] = m[8]  * Vec.X + m[9]  * Vec.Y + m[10] * Vec.Z + m[11] * Vec.Z;
        u[3] = m[12] * Vec.X + m[13] * Vec.Y + m[14] * Vec.Z + m[15] * Vec.Z;
        return u;
    }



}


#endif // !GEOMETRY_OBJECT3D_HPP
