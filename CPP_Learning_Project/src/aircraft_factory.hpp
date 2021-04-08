#pragma once

#include <random>
#include <string_view>

#include "aircraft.hpp"

class AircraftFactory
{
private:
    static const size_t NUM_AIRCRAFT_TYPES = 3;
    static const size_t NUM_AIRLINES = 8;

public:
    AircraftFactory ()
        : m_rengine      { std::random_device{}() }
        , m_fuel_range   { 150.f, GL::max_fuel }
    {}

    inline void init_aircraft_types()
    {
        m_aircraft_types[0] = new AircraftType { .7f, .7f, .5f, MediaPath { "l1011_48px.png" } };
        m_aircraft_types[1] = new AircraftType { .7f, .7f, .5f, MediaPath { "b707_jat.png" } };
        m_aircraft_types[2] = new AircraftType { .9f, .9f, .5f, MediaPath { "concorde_af.png" } };
    }

    [[nodiscard]] std::unique_ptr<Aircraft> create_random_aircraft(Tower& tower);
    [[nodiscard]] inline const std::array<std::string, NUM_AIRLINES> get_airlines() const { return m_airlines; }

private:
    AircraftType* m_aircraft_types[NUM_AIRCRAFT_TYPES] {};
    const std::array<std::string, NUM_AIRLINES> m_airlines = { "AF", "LH", "EY", "DL", "KL", "BA", "AY", "EY" };

    std::mt19937 m_rengine;
    std::uniform_real_distribution<float> m_fuel_range;

    std::set<std::string> m_used_names; 

    [[nodiscard]] std::unique_ptr<Aircraft> create_aircraft(const AircraftType& type, Tower& tower);
};