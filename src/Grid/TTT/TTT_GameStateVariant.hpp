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
    std::map<std::string, std::function<TTT_Bot*()>> botFactories;

    bool autoMode = false;

    void setBot(TTT_Bot* newBot) {
        bot = newBot;
    }

    GameStateBot(TTT_GridView& view, TTT_GridLogic& grid) : GameState(view,grid) {
        botFactories["Random"] = [&]() { return new RandomBot(); };
        
        changeBot("Random");
    }

    void changeBot(const std::string& botName) {
        if (botFactories.find(botName) != botFactories.end()) {
            bot = botFactories[botName]();
        }
    }

    bool isBotPlayer(TTT_Shape shape);
    bool playBot();
    bool playTurn() override;
};

} // namespace TTT