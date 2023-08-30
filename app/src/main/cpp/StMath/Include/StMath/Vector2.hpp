#ifndef GEOMETRY_Vector2_HPP
#define GEOMETRY_Vector2_HPP

#include <compare>
#include <cassert>

namespace st::math
{

    /*! \brief Point in 3d space
         *         Brief description continued.
         *
         *  Detailed description starts here.
         */
    struct alignas(16) Vector2
    {
    public:
		constexpr Vector2() noexcept:
		X(0.0F),
		Y(0.0F)
		{}

		constexpr Vector2(float x, float y) noexcept:
		X(x),
		Y(y)
		{}


        constexpr float& operator[](const size_t index)
        {
            assert(index >= 0 && index < 2);
            return ((&X)[index]);   //TODO More safe version?
        }
         
        const float& operator[](const size_t index) const
        {
            assert(index >= 0 && index < 2);
            return ((&X)[index]); //TODO More safe version?
        }

        auto operator<=>(const Vector2&) const = default;

        //Operators with other Vector
		constexpr Vector2 operator+(const Vector2& Vec) const
        {
			return { X + Vec.X, Y + Vec.Y};
        }

        constexpr Vector2 operator-(const Vector2& Vec) const
        {
			return { X - Vec.X, Y - Vec.Y};
        }

        constexpr Vector2 operator*(const Vector2& Vec) const
        {
			return { X * Vec.X, Y * Vec.Y};
	    }



		constexpr Vector2 operator/(const Vector2& Vec) const
		{
			return { X / Vec.X, Y / Vec.Y};
		}

        //Assine operators
		constexpr Vector2& operator*=(float Scale)
        {
            X *= Scale;
            Y *= Scale;
            return *this;
	    }

        constexpr Vector2& operator/=(float Scale)
        {
			assert(Scale != 0.0F);
			const float ms = 1.0F / Scale;
            X *= ms;
            Y *= ms;
            return *this;
	    }
       

        //Operator with Scale
		constexpr Vector2 operator*(float Scale) const
        {
			return { X * Scale, Y * Scale};
        }

		constexpr Vector2 operator/(float Scale) const
		{
			assert(Scale != 0.0F);
			const float ms = 1.0F / Scale;
			return { X * ms, Y * ms};
		}

        friend constexpr Vector2 operator*(float scalar, const Vector2& Vec)
		{
			return { scalar * Vec.X, scalar * Vec.Y};
		}


        //Negation
		inline constexpr Vector2 operator-() const
        {
			return { -X, -Y};
        }

        static float length(const Vector2& Vec);
		static Vector2 normalize(const Vector2& Vec);
		static Vector2 reflect(const Vector2& incident, const Vector2& normal);
		static float dotProduct(const Vector2& v1, const Vector2& v2);

    public:
        float X;
        float Y;
    };

  
}

#endif // !GEOMETRY_OBJECT3D_HPP
