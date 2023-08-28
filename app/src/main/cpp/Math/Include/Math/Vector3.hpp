#ifndef GEOMETRY_VECTOR3_HPP
#define GEOMETRY_VECTOR3_HPP

#include <compare>
#include <cassert>

namespace st::math
{

    /*! \brief Point in 3d space
         *         Brief description continued.
         *
         *  Detailed description starts here.
         */
    struct alignas(16) Vector3
    {
    public:
		constexpr Vector3() noexcept:
		X(0.0F),
		Y(0.0F),
		Z(0.0F)
		{}

		constexpr Vector3(float x, float y, float z) noexcept:
		X(x),
		Y(y),
		Z(z)
		{}


        constexpr float& operator[](const size_t index)
        {
            assert(index >= 0 && index < 4);
            return ((&X)[index]);   //TODO More safe version?
        }
         
        const float& operator[](const size_t index) const
        {
            assert(index >= 0 && index < 4);
            return ((&X)[index]); //TODO More safe version?
        }

        auto operator<=>(const Vector3&) const = default;

        //Operators with other Vector
		constexpr Vector3 operator+(const Vector3& Vec) const
        {
			return { X + Vec.X, Y + Vec.Y, Z + Vec.Z };
        }

        constexpr Vector3 operator-(const Vector3& Vec) const
        {
			return { X - Vec.X, Y - Vec.Y, Z - Vec.Z };
        }

        constexpr Vector3 operator*(const Vector3& Vec) const
        {
			return { X * Vec.X, Y * Vec.Y, Z * Vec.Z };
	    }



		constexpr Vector3 operator/(const Vector3& Vec) const
		{
			return { X / Vec.X, Y / Vec.Y, Z / Vec.Z };
		}

        //Assine operators
		constexpr Vector3& operator*=(float Scale)
        {
            X *= Scale;
            Y *= Scale;
            Z *= Scale;
            return *this;
	    }

        constexpr Vector3& operator/=(float Scale)
        {
			assert(Scale != 0.0F);
			const float ms = 1.0F / Scale;
            X *= ms;
            Y *= ms;
            Z *= ms;
            return *this;
	    }
       

        //Operator with Scale
		constexpr Vector3 operator*(float Scale) const
        {
			return { X * Scale, Y * Scale, Z * Scale };
        }

		constexpr Vector3 operator/(float Scale) const
		{
			assert(Scale != 0.0F);
			const float ms = 1.0F / Scale;
			return { X * ms, Y * ms, Z * ms };
		}

        friend constexpr Vector3 operator*(float scalar, const Vector3& Vec)
		{
			return { scalar * Vec.X, scalar * Vec.Y, scalar * Vec.Z };
		}


        //Negation
		inline constexpr Vector3 operator-() const
        {
			return { -X, -Y, -Z };
        }

        static float length(const Vector3& Vec);
		static Vector3 normalize(const Vector3& Vec);
		static Vector3 reflect(const Vector3& incident, const Vector3& normal);
		static float dotProduct(const Vector3& v1, const Vector3& v2);
		static constexpr Vector3 crossProduct(const Vector3& Vec1, const Vector3& Vec2)
		{
			Vector3 u;
			u.X = Vec1.Y * Vec2.Z - Vec1.Z * Vec2.Y;
			u.Y = Vec1.Z * Vec2.X - Vec1.X * Vec2.Z;
			u.Z = Vec1.X * Vec2.Y - Vec1.Y * Vec2.X;
			return u;
		}

    public:
        float X;
        float Y;
        float Z;
    };

  
}

#endif // !GEOMETRY_OBJECT3D_HPP
