#pragma once

#include <string_view>

#include "GL/displayable.hpp"

#include "geometry.hpp"
#include "tower.hpp"
#include "waypoint.hpp"
#include "aircraft_types.hpp"

class Aircraft : public GL::Displayable, public GL::DynamicObject
{
private:
    const AircraftType& m_type;
    const std::string m_flight_number;
    Point3D m_pos, m_speed; // note: the speed should always be normalized to length 'speed'
    WaypointQueue m_waypoints = {};
    Tower& m_control;

    bool m_landing_gear_deployed = false; // is the landing gear deployed?
    bool m_is_at_terminal        = false;
    bool m_has_landed            = false;
    bool m_has_crashed           = false;
    float m_fuel                 = 0.f;

    // turn the aircraft to arrive at the next waypoint
    // try to facilitate reaching the waypoint after the next by facing the
    // right way to this end, we try to face the point Z on the line spanned by
    // the next two waypoints such that Z's distance to the next waypoint is
    // half our distance so: |w1 - pos| = d and [w1 - w2].normalize() = W and Z
    // = w1 + W*d/2
    void turn_to_waypoint(double delta_time);
    void turn(Point3D direction);

    // select the correct tile in the plane texture (series of 8 sprites facing
    // [North, NW, W, SW, S, SE, E, NE])
    unsigned int get_speed_octant() const;
    // when we arrive at a terminal, signal the tower
    void arrive_at_terminal();
    // deploy and retract landing gear depending on next waypoints
    void operate_landing_gear();

    template <bool front>
    void add_waypoint(const Waypoint& wp);
    
    inline bool is_on_ground() const { return m_pos.z() < DISTANCE_THRESHOLD; }
    inline float max_speed() const { return is_on_ground() ? m_type.max_ground_speed : m_type.max_air_speed; }

    Aircraft(const Aircraft&) = delete;
    Aircraft& operator=(const Aircraft&) = delete;

public:
    Aircraft(const AircraftType& type_, const std::string_view& flight_number_, const Point3D& pos_,
             const Point3D& speed_, Tower& control_, float fuel_) :
        GL::Displayable { pos_.x() + pos_.y() },
        m_type            { type_ },
        m_flight_number   { flight_number_ },
        m_pos             { pos_ },
        m_speed           { speed_ },
        m_control         { control_ },
        m_fuel            { fuel_ }
    {
        m_speed.cap_length(max_speed());
    }

    ~Aircraft()
    {}

    inline const std::string& get_flight_num() const { return m_flight_number; }
    inline float distance_to(const Point3D& p) const { return m_pos.distance_to(p); }

    void display() const override;
    void move(double delta_time) override;

    inline bool is_out_of_sim() const override { return (m_has_landed && !m_is_at_terminal && m_waypoints.empty()) || m_has_crashed; }
    inline void crash() { m_has_crashed = true; }
    inline bool has_crashed() const { return m_has_crashed; }
    
    bool has_terminal() const;
    inline bool is_circling() const { return !m_has_landed && !has_terminal(); }
    inline bool is_at_terminal() const { return m_is_at_terminal; }
    inline bool has_left() const { return m_has_landed && !m_is_at_terminal; }

    bool operator<(const Aircraft&);

    inline bool is_low_on_fuel() const { return m_fuel < 400.f; }
    inline float get_fuel() const { return m_fuel; }
    void refill(float& fuel_stock);
    
    friend class Tower;
};
