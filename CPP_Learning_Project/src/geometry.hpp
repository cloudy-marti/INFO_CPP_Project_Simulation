#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>

#include <algorithm>
#include <numeric>

template <size_t dim, typename T>
class Point
{
public:
    Point() {}

    template <typename ...U>
    Point(T first, U&&... args)
        : values { first, std::forward<U>(args)... }
    {
        static_assert(sizeof...(args) + 1 == dim, "Number of args does not match with the dimension");
    }

    std::array<T, dim> values;

    float& x() { return values[0]; }
    float x() const { return values[0]; }

    float& y() { static_assert(dim >= 2); return values[1]; }
    float y() const { static_assert(dim >= 2); return values[1]; }

    float& z() { static_assert(dim >= 3); return values[2]; }
    float z() const { static_assert(dim >= 3); return values[2]; }

    Point& operator*=(const Point& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::multiplies<float>());
        return *this;
    }

    Point operator+(const Point& other) const
    {
        Point result = *this;
        result += other;
        return result;
    }

    Point operator*(const Point& other) const
    {
        Point result = *this;
        result *= other;
        return result;
    }

    Point operator*(const T scalar) const
    {
        Point result = *this;
        result *= scalar;
        return result;
    }

    Point& operator+=(const Point& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::plus<float>());
        return *this;
    }

    Point& operator-=(const Point& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::minus<float>());
        return *this;
    }

    Point& operator*=(const T scalar)
    {
        std::transform(values.begin(), values.end(), values.begin(), [scalar](float v) { return v * scalar; });
        return *this;
    }

    Point operator-() const
    {
        Point result = *this;
        result *= (-1);
        return result;
    }

    Point operator-(const Point& other) const
    {
        Point result = *this;
        result -= other;
        return result;
    } 

    float length() const
    {
        return std::sqrt(std::reduce(values.begin(), values.end(), 0.f, [](T v1, T v2) { return v1 + (v2 * v2); }));
    }

    float distance_to(const Point& other) const { return (*this - other).length(); }

    Point& normalize(const T target_len = 1.0f)
    {
        const float current_len = length();
        if (current_len == 0)
        {
            throw std::logic_error("cannot normalize vector of length 0");
        }
        *this *= (target_len / current_len);
        return *this;
    }

    Point& cap_length(const T max_len)
    {
        assert(max_len > 0);

        const float current_len = length();
        if (current_len > max_len)
        {
            *this *= (max_len / current_len);
        }

        return *this;
    }

    std::string to_string() const
    {
        std::string str = "(";
        for(auto it = values.begin(); it != values.end(); ++it)
        {
            str += std::to_string(*it);
            if(it+1 != values.end())
            {
                str += ", ";
            }
        }
        return str + ")";
    }
private:
};

using Point3D = Point<3, float>;
using Point2D = Point<2, float>;

// our 3D-coordinate system will be tied to the airport: the runway is parallel to the x-axis, the z-axis
// points towards the sky, and y is perpendicular to both thus,
// {1,0,0} --> {.5,.5}   {0,1,0} --> {-.5,.5}   {0,0,1} --> {0,1}
inline Point2D project_2D(const Point<3, float>& p)
{
    return { .5f * p.x() - .5f * p.y(), .5f * p.x() + .5f * p.y() + p.z() };
}


inline void test_generic_points()
{
    Point<2, float> p1 { 2.f, 3.f };
    Point<2, float> p2 { 4.f, 6.f };
    auto p3 = p1 + p2;
    p1 += p2;
    p1 *= 3.f; // ou 3.f, ou 3.0 en fonction du type de Point
    std::cout << p3.to_string() << std::endl;
}