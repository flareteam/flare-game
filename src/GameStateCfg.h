#ifndef GAMESTATECFG_H
#define GAMESTATECFG_H

#include "GameState.h"

class WidgetButton;
class WidgetCheckButton;

class GameStateCfg : public GameState {
public:
    GameStateCfg    ();
    ~GameStateCfg   ();

    void    logic   ();
    void    render  ();

private:
    WidgetButton        * ok_button;
    WidgetButton        * cancel_button;
    WidgetCheckButton   * truc_button;
};

#endif

