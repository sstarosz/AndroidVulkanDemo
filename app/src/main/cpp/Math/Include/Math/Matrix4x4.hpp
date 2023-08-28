#ifndef GEOMETRY_MATRIX4X4_HPP
#define GEOMETRY_MATRIX4X4_HPP

#include <compare>
#include "Vector3.hpp"

namespace st::math
{

    /*! \brief Point in 3d space
         *         Brief description continued.
         *
         *  Detailed description starts here.
         */
    class Matrix4x4
    {
    public:
        Matrix4x4() noexcept;
        Matrix4x4(float xa, float xb, float xc, float xd,
                    float ya, float yb, float yc, float yd,
                    float za, float zb, float zc, float zd,
                    float wa, float wb, float wc, float wd) noexcept;




        Matrix4x4& operator*=(const Matrix4x4& rhs)
        {
            m_value[0] = m_value[0] * rhs[0] + m_value[1] * rhs[4] + m_value[2] * rhs[8]  + m_value[3] * rhs[12];
            m_value[1] = m_value[0] * rhs[1] + m_value[1] * rhs[5] + m_value[2] * rhs[9]  + m_value[3] * rhs[13];
            m_value[2] = m_value[0] * rhs[2] + m_value[1] * rhs[6] + m_value[2] * rhs[10] + m_value[3] * rhs[14];
            m_value[3] = m_value[0] * rhs[3] + m_value[1] * rhs[7] + m_value[2] * rhs[11] + m_value[3] * rhs[15];

            m_value[4] = m_value[4] * rhs[0] + m_value[5] * rhs[4] + m_value[6] * rhs[8]  + m_value[7] * rhs[12];
            m_value[5] = m_value[4] * rhs[1] + m_value[5] * rhs[5] + m_value[6] * rhs[9]  + m_value[7] * rhs[13];
            m_value[6] = m_value[4] * rhs[2] + m_value[5] * rhs[6] + m_value[6] * rhs[10] + m_value[7] * rhs[14];
            m_value[7] = m_value[4] * rhs[3] + m_value[5] * rhs[7] + m_value[6] * rhs[11] + m_value[7] * rhs[15];

            m_value[8]  = m_value[8] * rhs[0] + m_value[9] * rhs[4] + m_value[10] * rhs[8]  + m_value[11] * rhs[12];
            m_value[9]  = m_value[8] * rhs[1] + m_value[9] * rhs[5] + m_value[10] * rhs[9]  + m_value[11] * rhs[13];
            m_value[10] = m_value[8] * rhs[2] + m_value[9] * rhs[6] + m_value[10] * rhs[10] + m_value[11] * rhs[14];
            m_value[11] = m_value[8] * rhs[3] + m_value[9] * rhs[7] + m_value[10] * rhs[11] + m_value[11] * rhs[15];

            m_value[12] = m_value[12] * rhs[0] + m_value[13] * rhs[4] + m_value[14] * rhs[8]  + m_value[15] * rhs[12];
            m_value[13] = m_value[12] * rhs[1] + m_value[13] * rhs[5] + m_value[14] * rhs[9]  + m_value[15] * rhs[13];
            m_value[14] = m_value[12] * rhs[2] + m_value[13] * rhs[6] + m_value[14] * rhs[10] + m_value[15] * rhs[14];
            m_value[15] = m_value[12] * rhs[3] + m_value[13] * rhs[7] + m_value[14] * rhs[11] + m_value[15] * rhs[15];


            return *this;
        }

        constexpr float& operator[](const size_t index)
        {
            assert(index >= 0 && index <= 15);
            return m_value[index];
        }

        const float& operator[](const size_t index) const
        {
            assert(index >= 0 && index <= 15);
            return m_value[index];
        }

        auto operator<=>(const Matrix4x4&) const = default;
        Matrix4x4 operator*(const Matrix4x4& other) const;


        static Matrix4x4 indentityMatrix();
        void translate(const Vector3& vector);

        static Matrix4x4 projectionMatrix(float fieldOfView, float framebufferAspectRatio, float nearPlane, float farPlane);



        static Matrix4x4 rotationX(const float& theta);
        static Matrix4x4 rotationY(const float& theta);
        static Matrix4x4 rotationZ(const float& theta);
        static Matrix4x4 rotationAroundAxis(const float& theta, const Vector3& v);


        void convertToColumnMajor();

    private:
        float m_value[16];
    };



    inline Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
    {
        Matrix4x4 result;

        result[0]  = m_value[0]  * other[0] + m_value[1]  * other[4] + m_value[2] * other[8]   + m_value[3] * other[12];
        result[1]  = m_value[0]  * other[1] + m_value[1]  * other[5] + m_value[2] * other[9]   + m_value[3] * other[13];
        result[2]  = m_value[0]  * other[2] + m_value[1]  * other[6] + m_value[2] * other[10]  + m_value[3] * other[14];
        result[3]  = m_value[0]  * other[3] + m_value[1]  * other[7] + m_value[2] * other[11]  + m_value[3] * other[15];
                                                                                               
        result[4]  = m_value[4]  * other[0] + m_value[5]  * other[4] + m_value[6] * other[8]   + m_value[7] * other[12];
        result[5]  = m_value[4]  * other[1] + m_value[5]  * other[5] + m_value[6] * other[9]   + m_value[7] * other[13];
        result[6]  = m_value[4]  * other[2] + m_value[5]  * other[6] + m_value[6] * other[10]  + m_value[7] * other[14];
        result[7]  = m_value[4]  * other[3] + m_value[5]  * other[7] + m_value[6] * other[11]  + m_value[7] * other[15];
                                                                                          
        result[8]  = m_value[8]  * other[0] + m_value[9]  * other[4] + m_value[10] * other[8]  + m_value[11] * other[12];
        result[9]  = m_value[8]  * other[1] + m_value[9]  * other[5] + m_value[10] * other[9]  + m_value[11] * other[13];
        result[10] = m_value[8]  * other[2] + m_value[9]  * other[6] + m_value[10] * other[10] + m_value[11] * other[14];
        result[11] = m_value[8]  * other[3] + m_value[9]  * other[7] + m_value[10] * other[11] + m_value[11] * other[15];
                                                                                                 
        result[12] = m_value[12] * other[0] + m_value[13] * other[4] + m_value[14] * other[8]  + m_value[15] * other[12];
        result[13] = m_value[12] * other[1] + m_value[13] * other[5] + m_value[14] * other[9]  + m_value[15] * other[13];
        result[14] = m_value[12] * other[2] + m_value[13] * other[6] + m_value[14] * other[10] + m_value[15] * other[14];
        result[15] = m_value[12] * other[3] + m_value[13] * other[7] + m_value[14] * other[11] + m_value[15] * other[15];

        return result;
    }

}



#endif // !GEOMETRY_OBJECT3D_HPP
