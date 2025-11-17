#pragma once

#include <state.hpp>

namespace title_screen {
class TitleScreen : public State {
public:
    std::unique_ptr<State> run() override;
};
}  // namespace title_screen