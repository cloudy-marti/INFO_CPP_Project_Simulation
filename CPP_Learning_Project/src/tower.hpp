#pragma once

#include <map>

#include "waypoint.hpp"

class Airport;
class Aircraft;
class Terminal;

class Tower
{
private:
    using AircraftToTerminal      = std::map<const Aircraft*, size_t>;
    using AircraftAndTerminalIter = AircraftToTerminal::iterator;

    Airport& m_airport;
    // aircrafts may reserve a terminal
    // if so, we need to save the terminal number in order to liberate it when the craft leaves
    AircraftToTerminal m_reserved_terminals = {};

    WaypointQueue get_circle() const;

    // AircraftAndTerminalIter find_craft_and_terminal(const Aircraft& aircraft) const
    // {
    //     // return std::find_if(reserved_terminals.begin(), reserved_terminals.end(),
    //     //                     [&aircraft](const auto& x) { return x.first == &aircraft; });
    //     return reserved_terminals.find(aircraft);
    // }

public:
    Tower(Airport& airport_) : m_airport { airport_ } {}

    // produce instructions for aircraft
    WaypointQueue get_instructions(Aircraft& aircraft);
    void arrived_at_terminal(const Aircraft& aircraft);

    WaypointQueue reserve_terminal(Aircraft& aircraft);
};
