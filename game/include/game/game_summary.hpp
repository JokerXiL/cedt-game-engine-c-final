#pragma once

#include <memory>

#include <game/state.hpp>

namespace game_summary {
class GameSummary : public State {
public:
    std::unique_ptr<State> run() override;
};
}  // namespace game_summary