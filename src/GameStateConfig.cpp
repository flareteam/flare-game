#include "WidgetButton.h"
#include "WidgetCheckBox.h"
#include "GameStateConfig.h"
#include "GameStateTitle.h"
#include "SharedResources.h"


GameStateConfig::GameStateConfig ()
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

    truc_button = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
    truc_button->pos.x = 50;
    truc_button->pos.y = 50;
}


GameStateConfig::~GameStateConfig()
{
    delete ok_button;
    delete cancel_button;
    delete truc_button;
}


    void
GameStateConfig::logic ()
{
    if (ok_button->checkClick()) {
        requestedGameState = new GameStateTitle();
    } else if (cancel_button->checkClick()) {
        requestedGameState = new GameStateTitle();
    } else if (truc_button->checkClick()) {
    }
}

    void
GameStateConfig::render ()
{
    ok_button->render();
    cancel_button->render();
    truc_button->render();
}

