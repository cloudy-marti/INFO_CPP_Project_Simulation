#include "tower_sim.hpp"

#include "airport.hpp"

using namespace std::string_literals;

TowerSimulation::TowerSimulation(int argc, char** argv) :
    m_help { (argc > 1) && (std::string { argv[1] } == "--help"s || std::string { argv[1] } == "-h"s) }
{
    MediaPath::initialize(argv[0]);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    GL::init_gl(argc, argv, "Airport Tower Simulation");

    create_keystrokes();
}

TowerSimulation::~TowerSimulation()
{
    delete m_airport;
}

void TowerSimulation::create_random_aircraft()
{
    assert(m_airport); // make sure the airport is initialized before creating aircraft
    m_aircraft_manager.add_aircraft(m_aircraft_factory.create_random_aircraft(m_airport->get_tower()));
}

void TowerSimulation::create_keystrokes()
{
    GL::keystrokes.emplace('q', []() { GL::exit_loop(); });
    GL::keystrokes.emplace('c', [this]() { create_random_aircraft(); });
    GL::keystrokes.emplace('+', []() { GL::change_zoom(0.95f); });
    GL::keystrokes.emplace('-', []() { GL::change_zoom(1.05f); });
    GL::keystrokes.emplace('f', []() { GL::toggle_fullscreen(); });
    GL::keystrokes.emplace('b', []() { GL::ticks_per_sec += 1; std::cout << "fps: " << GL::ticks_per_sec << std::endl; });
    GL::keystrokes.emplace('n', []() { if(GL::ticks_per_sec > 1) { GL::ticks_per_sec -= 1; std::cout << "fps: " << GL::ticks_per_sec << std::endl; } });
    GL::keystrokes.emplace('a', []() { GL::sim_speed += .1f; });
    GL::keystrokes.emplace('e', []() { GL::sim_speed -= .1f; });
    GL::keystrokes.emplace('m', [this]() { std::cout << m_aircraft_manager.count_crashed_aircrafts() << " aircrafts have crashed so far." << std::endl; });

    const auto& airlines = m_aircraft_factory.get_airlines();
    for(size_t index = 0; index < airlines.size() ; ++index)
    {
        GL::keystrokes.emplace('0' + index, [this, airlines, index]() {
            std::cout << "Airline " << airlines.at(index) << " is handling " << m_aircraft_manager.count_aircrafts_from_airline(airlines.at(index)) << " aircrafts." << std::endl;
        });
    }
}

void TowerSimulation::display_help() const
{
    std::cout << "This is an airport tower simulator" << std::endl
              << "the following keysstrokes have meaning:" << std::endl;

    // for (const auto& ks_pair : GL::keystrokes)
    // {
    //     std::cout << ks_pair.first << ' ';
    // }
    for(const auto& [key, value] : GL::keystrokes)
    {
        std::cout << key << ' ';
    }

    std::cout << std::endl;
}

void TowerSimulation::init_airport()
{
    m_airport = new Airport { one_lane_airport, Point3D { 0.f, 0.f, 0.f },
                            new img::Image { one_lane_airport_sprite_path.get_full_path() }, m_aircraft_manager };
}

void TowerSimulation::launch()
{
    if (m_help)
    {
        display_help();
        return;
    }

    init_airport();
    m_aircraft_factory.init_aircraft_types();

    GL::loop();
}
