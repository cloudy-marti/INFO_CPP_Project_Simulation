#pragma once

// runway positions relative to the airport position
struct Runway
{
    const Point3D m_start, end;

    Runway(const Point3D& start_, const float length = 1) :
        m_start { start_ }, end { start_ + Point3D { length, 0.f, 0.f } }
    {}
};
