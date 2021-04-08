#pragma once

#include <vector>

#include "runway.hpp"

class AirportType
{
private:
    const Point3D m_crossing_pos;
    const Point3D m_gateway_pos;
    const std::vector<Point3D> m_terminal_pos;
    const std::vector<Runway> m_runways;

public:
    AirportType(const Point3D& crossing_pos_, const Point3D& gateway_pos_,
                std::initializer_list<Point3D> terminal_pos_, std::initializer_list<Runway> runways_) :
        m_crossing_pos { crossing_pos_ },
        m_gateway_pos { gateway_pos_ },
        m_terminal_pos { terminal_pos_ },
        m_runways { runways_ }
    {}

    std::vector<Terminal> create_terminals() const
    {
        return std::vector<Terminal> { m_terminal_pos.begin(), m_terminal_pos.end() };
    }

    WaypointQueue air_to_terminal(const Point3D& offset, const size_t runway_num,
                                  const size_t terminal_num) const
    {
        const Runway& runway = m_runways.at(runway_num);

        const auto runway_middle_pos = (runway.m_start + runway.end) * 0.5f;
        const auto runway_length     = (runway.end - runway.m_start) * 0.5f;

        const Waypoint before_in_air { offset + runway.m_start - runway_length + Point3D { 0.f, 0.f, .7f },
                                       wp_air };
        const Waypoint runway_middle { offset + runway_middle_pos, wp_ground };
        const Waypoint runway_end { offset + runway.end, wp_ground };
        const Waypoint crossing { offset + m_crossing_pos, wp_ground };

        WaypointQueue result { before_in_air, runway_middle, runway_end, crossing };

        if (terminal_num != 0)
        {
            result.emplace_back(m_gateway_pos, wp_ground);
        }

        result.emplace_back(m_terminal_pos.at(terminal_num), wp_terminal);
        return result;
    }

    WaypointQueue terminal_to_air(const Point3D& offset, const size_t runway_num,
                                  const size_t terminal_num) const
    {
        const Runway& runway = m_runways.at(runway_num);
        const float angle    = (rand() % 1000) * 2 * 3.141592f / 1000.f; // random angle between 0 and 2pi

        const auto runway_middle_pos = (runway.m_start + runway.end) * 0.5f;
        const auto runway_length     = (runway.end - runway.m_start) * 0.5f;

        const Waypoint crossing { offset + m_crossing_pos, wp_ground };
        const Waypoint runway_start { offset + runway.m_start, wp_ground };
        const Waypoint runway_middle { offset + runway_middle_pos, wp_ground };
        const Waypoint later_in_air { offset + runway.end + runway_length + Point3D { 0.f, 0.f, .7f }, wp_air };
        const Waypoint randomly_high {
            Point3D { std::sin(angle), std::cos(angle), 0.f } * 6 + Point3D { 0.f, 0.f, 2.f }, wp_air
        };

        WaypointQueue result { crossing, runway_start, runway_middle, later_in_air, randomly_high };

        if (terminal_num != 0)
        {
            result.emplace_front(m_gateway_pos, wp_ground);
        }

        return result;
    }
};

inline const AirportType one_lane_airport { Point3D { -.1f, -.3f, 0.f },
                                            Point3D { -.6f, .3f, 0.f },
                                            { Point3D { .3f, 0.f, 0.f }, Point3D { -.3f, .3f, 0.f },
                                              Point3D { 0.f, .55f, 0.f } },
                                            { Runway { Point3D { -.5f, -.75f, 0.f } } } };
