#pragma once

class State {
public:
    virtual std::unique_ptr<State> run();
};