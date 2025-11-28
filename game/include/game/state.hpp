#pragma once

#include <memory>

class State {
public:
    virtual ~State() = default;
    virtual std::unique_ptr<State> run() = 0;
};