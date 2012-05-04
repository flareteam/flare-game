#include "WidgetButton.h"
#include "GameStateCfg.h"
#include "GameStateTitle.h"
#include "SharedResources.h"


GameStateCfg::GameStateCfg ()
        : GameState()
{
    ok_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
    ok_button->label = msg->get("Ok");
    ok_button->pos.x = (VIEW_W_HALF / 2) - (ok_button->pos.w / 2);
    ok_button->pos.y = VIEW_H - ok_button->pos.h;
    ok_button->refresh();

    cancel_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
    cancel_button->label = msg->get("Cancel");
    cancel_button->pos.x = (VIEW_W_HALF / 2) * 3 - (cancel_button->pos.w / 2);
    cancel_button->pos.y = VIEW_H - cancel_button->pos.h;
    cancel_button->refresh();
}


GameStateCfg::~GameStateCfg()
{
    delete ok_button;
    delete cancel_button;
}


    void
GameStateCfg::logic ()
{
    if (ok_button->checkClick()) {
        requestedGameState = new GameStateTitle();
    } else if (cancel_button->checkClick()) {
        requestedGameState = new GameStateTitle();
    }
}

    void
GameStateCfg::render ()
{
    ok_button->render();
    cancel_button->render();
}

