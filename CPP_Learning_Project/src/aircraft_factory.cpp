#include "aircraft_factory.hpp"

[[nodiscard]] std::unique_ptr<Aircraft> AircraftFactory::create_aircraft(const AircraftType& type, Tower& tower)
{
    std::string flight_number;
    do 
    {
        flight_number = m_airlines[std::rand() % 8] + std::to_string(1000 + (rand() % 9000));
    }
    while (m_used_names.find(flight_number) != m_used_names.end());
    m_used_names.emplace(flight_number);
    
    const float angle       = (rand() % 1000) * 2 * 3.141592f / 1000.f; // random angle between 0 and 2pi
    const Point3D start     = Point3D { std::sin(angle), std::cos(angle), 0.f } * 3 + Point3D { 0.f, 0.f, 2.f };
    const Point3D direction = (-start).normalize();
    float fuel              = m_fuel_range(m_rengine);

    return std::make_unique<Aircraft> (type, flight_number, start, direction, tower, fuel);
}

[[nodiscard]] std::unique_ptr<Aircraft> AircraftFactory::create_random_aircraft(Tower& tower)
{
    return create_aircraft(*(m_aircraft_types[rand() % 3]), tower);
}