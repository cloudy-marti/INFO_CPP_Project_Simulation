#pragma once

class Terminal : public GL::DynamicObject
{
private:
    unsigned int m_service_progress    = SERVICE_CYCLES;
    Aircraft* m_current_aircraft = nullptr;
    const Point3D m_pos;

    Terminal(const Terminal&) = delete;
    Terminal& operator=(const Terminal&) = delete;

public:
    Terminal(const Point3D& pos_) : m_pos { pos_ } {}

    bool in_use() const { return m_current_aircraft != nullptr; }
    bool is_servicing() const { return m_service_progress < SERVICE_CYCLES; }
    void assign_craft(Aircraft& aircraft) { m_current_aircraft = &aircraft; }

    void start_service(const Aircraft& aircraft)
    {
        assert(aircraft.distance_to(m_pos) < DISTANCE_THRESHOLD);
        std::cout << "now servicing " << aircraft.get_flight_num() << "...\n";
        m_service_progress = 0;
    }

    void abort_service()
    {
        std::cout << "Aborting servicing " << m_current_aircraft->get_flight_num() << " because it crashed\n";
        m_current_aircraft = nullptr; 
    }

    void finish_service()
    {
        if (!is_servicing())
        {
            std::cout << "done servicing " << m_current_aircraft->get_flight_num() << '\n';
            m_current_aircraft = nullptr;
        }
    }

    void move(double) override
    {
        if (in_use() && is_servicing() && !m_current_aircraft->is_low_on_fuel())
        {
            ++m_service_progress;
        }
    }

    inline bool is_out_of_sim() const override { return false; }

    void refill_aircraft_if_needed(float& fuel_stock)
    {
        if(m_current_aircraft == nullptr)
        {
            return;
        }
        if(m_current_aircraft->is_at_terminal() && m_current_aircraft->is_low_on_fuel())
        {
            m_current_aircraft->refill(fuel_stock);
        }
    }
};