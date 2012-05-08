#include "WidgetButton.h"
#include "WidgetCheckBox.h"
#include "WidgetSlider.h"
#include "GameStateConfig.h"
#include "GameStateTitle.h"
#include "SharedResources.h"


GameStateConfig::GameStateConfig ()
        : GameState(),
          child_widget(),
          ok_button(NULL),
          cancel_button(NULL),
          full_screen_lb(NULL),
          full_screen_cb(NULL)

{
    ok_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
    ok_button->label = msg->get("Ok");
    ok_button->pos.x = (VIEW_W_HALF / 2) - (ok_button->pos.w / 2);
    ok_button->pos.y = VIEW_H - ok_button->pos.h;
    ok_button->refresh();
    child_widget.push_back(ok_button);

    cancel_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
    cancel_button->label = msg->get("Cancel");
    cancel_button->pos.x = (VIEW_W_HALF / 2) * 3 - (cancel_button->pos.w / 2);
    cancel_button->pos.y = VIEW_H - cancel_button->pos.h;
    cancel_button->refresh();
    child_widget.push_back(cancel_button);

    full_screen_lb = new WidgetLabel();
    full_screen_lb->set(0, 0, JUSTIFY_LEFT, VALIGN_TOP, "Full Screen Mode", FONT_WHITE);
    child_widget.push_back(full_screen_lb);

    full_screen_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
    full_screen_cb->pos.x = full_screen_lb->bounds.w;
    full_screen_cb->pos.y = 0;
    child_widget.push_back(full_screen_cb);

    volume_music_lb = new WidgetLabel();
    volume_music_lb->set(0, 64, JUSTIFY_LEFT, VALIGN_TOP, "Music Volume", FONT_WHITE);
    child_widget.push_back(volume_music_lb);

    volume_music_sl = new WidgetSlider(mods->locate("images/menus/buttons/slider_default.png"));
    volume_music_sl->pos.x = volume_music_lb->bounds.w;
    volume_music_sl->pos.y = volume_music_lb->bounds.y;
    volume_music_sl->set(0,128,64);
    child_widget.push_back(volume_music_sl);
}


GameStateConfig::~GameStateConfig()
{
    for (unsigned int i = 0; i < child_widget.size(); i++) {
        delete child_widget[i];
    }
}


    void
GameStateConfig::logic ()
{
    if (ok_button->checkClick()) {
        requestedGameState = new GameStateTitle();
    } else if (cancel_button->checkClick()) {
        requestedGameState = new GameStateTitle();
    } else if (full_screen_cb->checkClick()) {
    } else if (volume_music_sl->checkClick()) {
    }
}

    void
GameStateConfig::render ()
{
    for (unsigned int i = 0; i < child_widget.size(); i++) {
        child_widget[i]->render();
    }
}

