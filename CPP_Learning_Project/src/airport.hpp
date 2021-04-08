#pragma once

#include "terminal.hpp"
#include "airport_type.hpp"
#include "aircraft_manager.hpp"

class Airport : public GL::Displayable, public GL::DynamicObject
{
private:
    const AirportType& m_type;
    const Point3D m_pos;
    const GL::Texture2D m_texture;
    std::vector<Terminal> m_terminals;
    Tower m_tower;

    float m_fuel_stock = 0.f;
    float m_ordered_fuel = 0.f;
    int m_next_refill_time = 0;

    const AircraftManager& _aircraft_manager;

    // reserve a terminal
    // if a terminal is free, return
    // 1. a sequence of waypoints reaching the terminal from the runway-end and
    // 2. the number of the terminal (used for liberating the terminal later)
    // otherwise, return an empty waypoint-vector and any number
    std::pair<WaypointQueue, size_t> reserve_terminal(Aircraft& aircraft)
    {
        const auto it =
            std::find_if(m_terminals.begin(), m_terminals.end(), [](const Terminal& t) { return !t.in_use(); });

        if (it != m_terminals.end())
        {
            it->assign_craft(aircraft);
            const auto term_idx = std::distance(m_terminals.begin(), it);
            return { m_type.air_to_terminal(m_pos, 0, term_idx), term_idx };
        }
        else
        {
            return { {}, 0u };
        }
    }

    WaypointQueue start_path(const size_t terminal_number)
    {
        return m_type.terminal_to_air(m_pos, 0, terminal_number);
    }

    Terminal& get_terminal(const size_t terminal_num) { return m_terminals.at(terminal_num); }

    void manage_fuel(double delta_time)
    {
        if(m_next_refill_time > 0.f)
        {
            m_next_refill_time -= delta_time;
            return;
        }
        
        float required_fuel = _aircraft_manager.get_required_fuel();
        if(required_fuel > m_fuel_stock)
        {
            required_fuel -= m_fuel_stock;
            m_ordered_fuel = required_fuel > GL::max_truck_load ? GL::max_truck_load : required_fuel;
            m_fuel_stock += m_ordered_fuel;
            std::cout << "Ordered " << m_ordered_fuel << " liters of fuel. Current fuel: " << m_fuel_stock << " liters." << std::endl;
            m_next_refill_time = 100.;
        }
    }

public:
    Airport(const AirportType& type_, const Point3D& pos_, const img::Image* image, const AircraftManager& aircraft_manager_, const float z_ = 1.0f) :
        GL::Displayable { z_ },
        m_type { type_ },
        m_pos { pos_ },
        m_texture { image },
        m_terminals { m_type.create_terminals() },
        m_tower { *this },
        _aircraft_manager { aircraft_manager_ }
    {
        GL::display_queue.emplace_back(this);
        GL::move_queue.emplace(this);
    }

    ~Airport()
    {
        const GL::Displayable* tmp = dynamic_cast<const Displayable*>(this);
        GL::display_queue.erase(std::remove(GL::display_queue.begin(), GL::display_queue.end(), tmp));
        GL::move_queue.erase(this);
    }

    Tower& get_tower() { return m_tower; }

    void display() const override { m_texture.draw(project_2D(m_pos), { 2.0f, 2.0f }); }

    void move(double delta_time) override
    {
        manage_fuel(delta_time);
        for (auto& t : m_terminals)
        {
            t.move(delta_time);
            t.refill_aircraft_if_needed(m_fuel_stock);
        }
    }

    inline bool is_out_of_sim() const override { return false; }

    friend class Tower;
};
