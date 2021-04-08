#pragma once

class Airport;

#include "aircraft_manager.hpp"
#include "aircraft_factory.hpp"

class TowerSimulation
{
private:
    bool m_help        = false;
    Airport* m_airport = nullptr;
    AircraftManager m_aircraft_manager;
    AircraftFactory m_aircraft_factory;

    TowerSimulation(const TowerSimulation&) = delete;
    TowerSimulation& operator=(const TowerSimulation&) = delete;

    void create_random_aircraft();

    void create_keystrokes();
    void display_help() const;

    void init_airport();

public:
    TowerSimulation(int argc, char** argv);
    ~TowerSimulation();

    void launch();
};
