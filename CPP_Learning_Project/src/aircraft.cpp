#include "aircraft.hpp"

void Aircraft::turn_to_waypoint(double delta_time)
{
    if (!m_waypoints.empty())
    {
        Point3D target = m_waypoints[0];
        if (m_waypoints.size() > 1)
        {
            const float d   = (m_waypoints[0] - m_pos).length();
            const Point3D W = (m_waypoints[0] - m_waypoints[1]).normalize(d / 2.0f);
            target += W;
        }

        Point3D direction = (target - m_pos)*(1.f/delta_time);
        turn(direction - m_speed);
    }
}

void Aircraft::turn(Point3D direction)
{
    (m_speed += direction.cap_length(m_type.max_accel)).cap_length(max_speed());
}

unsigned int Aircraft::get_speed_octant() const
{
    const float speed_len = m_speed.length();
    if (speed_len > 0)
    {
        const Point3D norm_speed { m_speed * (1.0f / speed_len) };
        const float angle =
            (norm_speed.y() > 0) ? 2.0f * 3.141592f - std::acos(norm_speed.x()) : std::acos(norm_speed.x());
        // partition into NUM_AIRCRAFT_TILES equal pieces
        return (static_cast<int>(std::round((angle * NUM_AIRCRAFT_TILES) / (2.0f * 3.141592f))) + 1) %
               NUM_AIRCRAFT_TILES;
    }
    else
    {
        return 0;
    }
}

// when we arrive at a terminal, signal the tower
void Aircraft::arrive_at_terminal()
{
    // we arrived at a terminal, so start servicing
    m_control.arrived_at_terminal(*this);
    m_is_at_terminal = true;
    m_has_landed = true;
}

// deploy and retract landing gear depending on next waypoints
void Aircraft::operate_landing_gear()
{
    if (m_waypoints.size() > 1u)
    {
        const auto it            = m_waypoints.begin();
        const bool ground_before = it->is_on_ground();
        const bool ground_after  = std::next(it)->is_on_ground();
        // deploy/retract landing gear when landing/lifting-off
        if (ground_before && !ground_after)
        {
            std::cout << m_flight_number << " lift off" << std::endl;
        }
        else if (!ground_before && ground_after)
        {
            std::cout << m_flight_number << " is now landing..." << std::endl;
            m_landing_gear_deployed = true;
        }
        else if (!ground_before && !ground_after)
        {
            m_landing_gear_deployed = false;
        }
    }
}

template <bool front>
void Aircraft::add_waypoint(const Waypoint& wp)
{
    if constexpr (front)
    {
        m_waypoints.push_front(wp);
    }
    else
    {
        m_waypoints.push_back(wp);
    }
}

void Aircraft::move(double delta_time)
{
    m_fuel -= 0.5f;
    if(m_fuel <= 0.f)
    {
        throw AircraftCrash { m_flight_number, m_pos, m_speed, "out of fuel" };
    }

    if (m_waypoints.empty())
    {
        if(is_out_of_sim())
        {
            return;
        }
        // waypoints = control.get_instructions(*this);
        // auto front = false;
        for (const auto& wp: m_control.get_instructions(*this))
        {
            add_waypoint<false>(wp);
        }
    }

    if (!m_is_at_terminal)
    {
        if(is_circling())
        {
            auto path = m_control.reserve_terminal(*this);
            if(!path.empty())
            {
                m_waypoints = std::move(path);
            }    
        }

        turn_to_waypoint(delta_time);
        // move in the direction of the current speed
        m_pos += m_speed * delta_time;

        // if we are close to our next waypoint, stike if off the list
        if (!m_waypoints.empty() && distance_to(m_waypoints.front()) < DISTANCE_THRESHOLD)
        {
            if (m_waypoints.front().is_at_terminal())
            {
                arrive_at_terminal();
            }
            else
            {
                operate_landing_gear();
            }
            m_waypoints.pop_front();
        }

        if (is_on_ground())
        {
            if (!m_landing_gear_deployed)
            {
                using namespace std::string_literals;
                throw AircraftCrash { m_flight_number, m_pos, m_speed, "bad landing" };
            }
        }
        else
        {
            // if we are in the air, but too slow, then we will sink!
            const float speed_len = m_speed.length();
            if (speed_len < SPEED_THRESHOLD)
            {
                m_pos.z() -= SINK_FACTOR * (SPEED_THRESHOLD - speed_len);
            }
        }

        // update the z-value of the displayable structure
        GL::Displayable::z = m_pos.x() + m_pos.y();
    }
}

void Aircraft::display() const
{
    m_type.texture.draw(project_2D(m_pos), { PLANE_TEXTURE_DIM, PLANE_TEXTURE_DIM }, get_speed_octant());
}

bool Aircraft::has_terminal() const
{
    if(!m_waypoints.empty())
    {
        return m_waypoints.back().type == WaypointType::wp_terminal;
    }
    return false;
}

bool Aircraft::operator<(const Aircraft& other)
{
    if (has_terminal())
    {
        if(other.has_terminal())
        {
            return m_fuel < other.m_fuel;
        }
        return true;
    }
    else
    {
        if(other.has_terminal())
        {
            return false;
        }
        return m_fuel < other.m_fuel;
    }
}

void Aircraft::refill(float &fuel_stock)
{
    float fuel_needed = GL::max_fuel - m_fuel;
    float fuel_refilled = fuel_stock > fuel_needed ? fuel_needed : fuel_stock;
    if(fuel_stock > 0)
    {
        fuel_stock -= fuel_refilled;
        m_fuel += fuel_refilled;
        std::cout << "Refilling " << fuel_refilled << " liters of fuel to aircraft " << m_flight_number << "." << std::endl;
    }
}