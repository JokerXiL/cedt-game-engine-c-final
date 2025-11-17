#pragma once

#include <memory>
#include <state.hpp>

namespace main_game {
class MainGame : public State {
public:
    std::unique_ptr<State> run() override;
};
}  // namespace main_game