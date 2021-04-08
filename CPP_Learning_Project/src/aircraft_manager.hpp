#pragma once

#include <memory>

#include "GL/dynamic_object.hpp"
#include "GL/displayable.hpp"

class Aircraft;

class AircraftManager : public GL::DynamicObject, public GL::Displayable
{
public:
    AircraftManager() 
        : GL::Displayable { 0 }
    {
        GL::move_queue.emplace(this);
        GL::display_queue.emplace_back(this);
    }
    ~AircraftManager() {}

    void add_aircraft(std::unique_ptr<Aircraft> aircraft);
    void display() const override;

    void move(double) override;
    bool is_out_of_sim() const override;

    int count_aircrafts_from_airline(const std::string& airline) const;
    int count_crashed_aircrafts() const { return m_crashed_aircrafts; }

    float get_required_fuel() const;

private:
    std::vector<std::unique_ptr<Aircraft>> m_aircrafts;
    int m_crashed_aircrafts = 0;
};