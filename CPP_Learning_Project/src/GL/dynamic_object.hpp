#pragma once

#include <unordered_set>

namespace GL {

class DynamicObject
{
public:
    virtual ~DynamicObject() {}

    virtual void move(double delta_time) = 0;

    virtual bool is_out_of_sim() const = 0;
};

inline std::unordered_set<DynamicObject*> move_queue;

} // namespace GL
