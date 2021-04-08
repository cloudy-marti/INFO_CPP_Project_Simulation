#include "aircraft_manager.hpp"
#include "aircraft.hpp"

void AircraftManager::add_aircraft(std::unique_ptr<Aircraft> aircraft)
{
    m_aircrafts.emplace_back(std::move(aircraft));
}

void AircraftManager::move(double delta_time)
{
    std::sort(m_aircrafts.begin(), m_aircrafts.end(),
        [](const std::unique_ptr<Aircraft>& a1,const std::unique_ptr<Aircraft>& a2) { return a1 < a2; });
        
    for(auto it = m_aircrafts.begin(); it != m_aircrafts.end();)
    {
        auto& aircraft = *it;
        try
        {
            aircraft->move(delta_time);
        }
        catch (const AircraftCrash& crash)
        {
            aircraft->crash();
            ++m_crashed_aircrafts;
            std::cerr << crash.what() << std::endl;
        }
        ++it;
    }

    m_aircrafts.erase(std::remove_if(m_aircrafts.begin(), m_aircrafts.end(), [](std::unique_ptr<Aircraft>& a) { return a->is_out_of_sim(); }),
        m_aircrafts.end());
}

bool AircraftManager::is_out_of_sim() const
{
    throw std::runtime_error( "AircraftManager::is_out_of_sim() is unsupported" );
}

void AircraftManager::display() const
{
    for(auto it = m_aircrafts.begin(); it != m_aircrafts.end(); ++it)
    {
        auto& aircraft = *it;
        aircraft->display();
    }
}

int AircraftManager::count_aircrafts_from_airline(const std::string& airline) const
{
    return std::count_if(m_aircrafts.begin(), m_aircrafts.end(), [airline](const std::unique_ptr<Aircraft>& aircraft) {
        return aircraft->get_flight_num().compare(0, airline.size(), airline) == 0; 
    });
}

[[nodiscard]] float AircraftManager::get_required_fuel() const
{
    float total_fuel = 0.f;
    auto filtered_aircrafts = m_aircrafts | std::views::filter([](const std::unique_ptr<Aircraft>& a) { return !a->has_left() && a->is_low_on_fuel(); });
    
    return std::accumulate(filtered_aircrafts.begin(), filtered_aircrafts.end(), 0.f,
        [](float sum, const std::unique_ptr<Aircraft>& aircraft) { return sum + (3000.f - aircraft->get_fuel()); });

    return total_fuel;
}