#ifndef GAMESTATECONFIG_H
#define GAMESTATECONFIG_H

#include "GameState.h"

class WidgetButton;
class WidgetCheckBox;

class GameStateConfig : public GameState {
public:
    GameStateConfig    ();
    ~GameStateConfig   ();

    void    logic   ();
    void    render  ();

private:
    WidgetButton        * ok_button;
    WidgetButton        * cancel_button;
    WidgetCheckBox   * truc_button;
};

#endif

