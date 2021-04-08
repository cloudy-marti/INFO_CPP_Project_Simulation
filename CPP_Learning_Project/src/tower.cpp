#include "tower.hpp"

#include "aircraft.hpp"
#include "airport.hpp"
#include "airport_type.hpp"

WaypointQueue Tower::get_circle() const
{
    return { { Point3D { -1.5f, -1.5f, .5f }, wp_air },
             { Point3D { 1.5f, -1.5f, .5f }, wp_air },
             { Point3D { 1.5f, 1.5f, .5f }, wp_air },
             { Point3D { -1.5f, 1.5f, .5f }, wp_air } };
}

WaypointQueue Tower::get_instructions(Aircraft& aircraft)
{
    if (!aircraft.m_is_at_terminal)
    {
        const auto path = reserve_terminal(aircraft);
        if(path.empty())
        {
            return get_circle();
        }
        else
        {
            return path;
        }
    }
    else
    {
        // get a path for the craft to start
        // const auto it = find_craft_and_terminal(aircraft);
        const auto it = m_reserved_terminals.find(&aircraft);
        assert(it != m_reserved_terminals.end());
        const auto terminal_num = it->second;
        Terminal& terminal      = m_airport.get_terminal(terminal_num);
        if(aircraft.has_crashed())
        {
            terminal.abort_service();
            m_reserved_terminals.erase(it);
            aircraft.m_is_at_terminal = false;
            return m_airport.start_path(terminal_num);
        }
        if (!terminal.is_servicing())
        {
            terminal.finish_service();
            m_reserved_terminals.erase(it);
            aircraft.m_is_at_terminal = false;
            return m_airport.start_path(terminal_num);
        }
        else
        {
            return {};
        }
    }
}

void Tower::arrived_at_terminal(const Aircraft& aircraft)
{
    const auto it = m_reserved_terminals.find(&aircraft);
    assert(it != m_reserved_terminals.end());
    m_airport.get_terminal(it->second).start_service(aircraft);
}

WaypointQueue Tower::reserve_terminal(Aircraft& aircraft)
{
    // if the aircraft is far, then just guide it to the airport vicinity
    if (aircraft.distance_to(m_airport.m_pos) < 5)
    {
        // try and reserve a terminal for the craft to land
        const auto vp = m_airport.reserve_terminal(aircraft);
        if (!vp.first.empty())
        {
            m_reserved_terminals.emplace(&aircraft, vp.second);
            return vp.first;
        }
    }
    return {};
}
