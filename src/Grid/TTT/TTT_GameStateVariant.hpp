#pragma once

#include "TTT_GameState.hpp"
#include "TTT_Bot.hpp"
#include <map>
#include <functional>

namespace TTT {
class GameStateBot : public GameState {
    public:

    using GameState::GameState;

    TTT_Bot* bot;
    std::string botName = "None";
    std::map<std::string, std::function<TTT_Bot*()>> botFactories;

    bool autoMode = false;

    void setBot(TTT_Bot* newBot) {
        bot = newBot;
    }

    GameStateBot(TTT_GridLogic& grid, TTT_GridView& view) : GameState(grid, view) {
        botFactories["Random"] = [&]() { return new RandomBot(); };
        botFactories["First"] = [&]() { return new FirstMoveBot(); };
        
        changeBot("Random");
    }

    void changeBot(const std::string& newBotName) {
        if (botFactories.find(newBotName) != botFactories.end()) {
            bot = botFactories[newBotName]();
            botName = newBotName;
        }
    }

    bool isBotPlayer(TTT_Shape shape);
    bool playBot();
    bool playTurn() override;
    void showParam() override;
};

} // namespace TTT