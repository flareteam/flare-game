#include "WidgetButton.h"
#include "WidgetCheckBox.h"
#include "WidgetSlider.h"
#include "GameStateConfig.h"
#include "GameStateTitle.h"
#include "SharedResources.h"
#include "Settings.h"


GameStateConfig::GameStateConfig ()
        : GameState(),
          child_widget(),
          ok_button(NULL),
          cancel_button(NULL),
          full_screen_lb(NULL),
          full_screen_cb(NULL)

{
    ok_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
    cancel_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
    full_screen_lb = new WidgetLabel();
    full_screen_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
    volume_music_lb = new WidgetLabel();
    volume_music_sl = new WidgetSlider(mods->locate("images/menus/buttons/slider_default.png"));
    volume_sound_lb = new WidgetLabel();
    volume_sound_sl = new WidgetSlider(mods->locate("images/menus/buttons/slider_default.png"));
    mouse_move_lb = new WidgetLabel();
    mouse_move_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
    combat_text_lb = new WidgetLabel();
    combat_text_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
    hwsurface_lb = new WidgetLabel();
    hwsurface_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
    doublebuf_lb = new WidgetLabel();
    doublebuf_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
    enable_joystick_lb = new WidgetLabel();
    enable_joystick_cb = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));

    //Load the menu configuration from file
    FileParser infile;
    if (infile.open(mods->locate("menus/config.txt"))) {
        while (infile.next()) {
            if (infile.key == "ok_button"){

                infile.val = infile.val + ',';
                ok_button->pos.x = VIEW_W*eatFirstInt(infile.val, ',')/100;
                ok_button->pos.y = VIEW_H*eatFirstInt(infile.val, ',')/100;
            }
            else if (infile.key == "cancel_button"){

                infile.val = infile.val + ',';
                cancel_button->pos.x = VIEW_W*eatFirstInt(infile.val, ',')/100;
                cancel_button->pos.y = VIEW_H*eatFirstInt(infile.val, ',')/100;
            }
            else if (infile.key == "fullscreen"){

                infile.val = infile.val + ',';
                full_screen_lb->setX(VIEW_W*eatFirstInt(infile.val, ',')/100);
                full_screen_lb->setY(VIEW_H*eatFirstInt(infile.val, ',')/100);
                full_screen_cb->pos.x = VIEW_W*eatFirstInt(infile.val, ',')/100;
                full_screen_cb->pos.y = VIEW_H*eatFirstInt(infile.val, ',')/100;
            }
            else if (infile.key == "resolution"){

                /*infile.val = infile.val + ',';
                resolution_lb->setX(VIEW_W*eatFirstInt(infile.val, ',')/100);
                resolution_lb->setY(VIEW_H*eatFirstInt(infile.val, ',')/100);
                resolution_cmb->pos.x = VIEW_W*eatFirstInt(infile.val, ',')/100;
                resolution_cmb->pos.y = VIEW_H*eatFirstInt(infile.val, ',')/100;*/
            }
            else if (infile.key == "music_volume"){

                infile.val = infile.val + ',';
                volume_music_lb->setX(VIEW_W*eatFirstInt(infile.val, ',')/100);
                volume_music_lb->setY(VIEW_H*eatFirstInt(infile.val, ',')/100);
                volume_music_sl->pos.x = VIEW_W*eatFirstInt(infile.val, ',')/100;
                volume_music_sl->pos.y = VIEW_H*eatFirstInt(infile.val, ',')/100;
            }
            else if (infile.key == "sound_volume"){

                infile.val = infile.val + ',';
                volume_sound_lb->setX(VIEW_W*eatFirstInt(infile.val, ',')/100);
                volume_sound_lb->setY(VIEW_H*eatFirstInt(infile.val, ',')/100);
                volume_sound_sl->pos.x = VIEW_W*eatFirstInt(infile.val, ',')/100;
                volume_sound_sl->pos.y = VIEW_H*eatFirstInt(infile.val, ',')/100;
            }
            else if (infile.key == "mouse_move"){

                infile.val = infile.val + ',';
                mouse_move_lb->setX(VIEW_W*eatFirstInt(infile.val, ',')/100);
                mouse_move_lb->setY(VIEW_H*eatFirstInt(infile.val, ',')/100);
                mouse_move_cb->pos.x = VIEW_W*eatFirstInt(infile.val, ',')/100;
                mouse_move_cb->pos.y = VIEW_H*eatFirstInt(infile.val, ',')/100;
            }
            else if (infile.key == "combat_text"){

                infile.val = infile.val + ',';
                combat_text_lb->setX(VIEW_W*eatFirstInt(infile.val, ',')/100);
                combat_text_lb->setY(VIEW_H*eatFirstInt(infile.val, ',')/100);
                combat_text_cb->pos.x = VIEW_W*eatFirstInt(infile.val, ',')/100;
                combat_text_cb->pos.y = VIEW_H*eatFirstInt(infile.val, ',')/100;
            }
            else if (infile.key == "hwsurface"){

                infile.val = infile.val + ',';
                hwsurface_lb->setX(VIEW_W*eatFirstInt(infile.val, ',')/100);
                hwsurface_lb->setY(VIEW_H*eatFirstInt(infile.val, ',')/100);
                hwsurface_cb->pos.x = VIEW_W*eatFirstInt(infile.val, ',')/100;
                hwsurface_cb->pos.y = VIEW_H*eatFirstInt(infile.val, ',')/100;
            }
            else if (infile.key == "doublebuf"){

                infile.val = infile.val + ',';
                doublebuf_lb->setX(VIEW_W*eatFirstInt(infile.val, ',')/100);
                doublebuf_lb->setY(VIEW_H*eatFirstInt(infile.val, ',')/100);
                doublebuf_cb->pos.x = VIEW_W*eatFirstInt(infile.val, ',')/100;
                doublebuf_cb->pos.y = VIEW_H*eatFirstInt(infile.val, ',')/100;
            }
            else if (infile.key == "enable_joystick"){

                infile.val = infile.val + ',';
                enable_joystick_lb->setX(VIEW_W*eatFirstInt(infile.val, ',')/100);
                enable_joystick_lb->setY(VIEW_H*eatFirstInt(infile.val, ',')/100);
                enable_joystick_cb->pos.x = VIEW_W*eatFirstInt(infile.val, ',')/100;
                enable_joystick_cb->pos.y = VIEW_H*eatFirstInt(infile.val, ',')/100;
            }
            else if (infile.key == "joystick_device"){

                /*infile.val = infile.val + ',';
                joystick_device_lb->setX(VIEW_W*eatFirstInt(infile.val, ',')/100);
                joystick_device_lb->setY(VIEW_H*eatFirstInt(infile.val, ',')/100);
                joystick_device_cmb->pos.x = VIEW_W*eatFirstInt(infile.val, ',')/100;
                joystick_device_cmb->pos.y = VIEW_H*eatFirstInt(infile.val, ',')/100;*/
            }
            else if (infile.key == "language"){

                /*infile.val = infile.val + ',';
                language_lb->setX(VIEW_W*eatFirstInt(infile.val, ',')/100);
                language_lb->setY(VIEW_H*eatFirstInt(infile.val, ',')/100);
                language_cmb->pos.x = VIEW_W*eatFirstInt(infile.val, ',')/100;
                language_cmb->pos.y = VIEW_H*eatFirstInt(infile.val, ',')/100;*/
            }
        }
    }

    ok_button->label = msg->get("Ok");
    ok_button->refresh();
    child_widget.push_back(ok_button);

    cancel_button->label = msg->get("Cancel");
    cancel_button->refresh();
    child_widget.push_back(cancel_button);

    full_screen_lb->set(msg->get("Full Screen Mode"));
    child_widget.push_back(full_screen_lb);

    if (FULLSCREEN == true) full_screen_cb->Check();
    child_widget.push_back(full_screen_cb);

    volume_music_lb->set(msg->get("Music Volume"));
    child_widget.push_back(volume_music_lb);

    volume_music_sl->set(0,128,MUSIC_VOLUME);
    child_widget.push_back(volume_music_sl);

    volume_sound_lb->set(msg->get("Sound Volume"));
    child_widget.push_back(volume_sound_lb);

    volume_sound_sl->set(0,128,SOUND_VOLUME);
    child_widget.push_back(volume_sound_sl);

    mouse_move_lb->set(msg->get("Move hero using mouse"));
    child_widget.push_back(mouse_move_lb);

    if (MOUSE_MOVE == true) mouse_move_cb->Check();
    child_widget.push_back(mouse_move_cb);

    combat_text_lb->set(msg->get("Show combat text"));
    child_widget.push_back(combat_text_lb);

    if (COMBAT_TEXT == true) combat_text_cb->Check();
    child_widget.push_back(combat_text_cb);

    hwsurface_lb->set(msg->get("Hardware surfaces"));
    child_widget.push_back(hwsurface_lb);

    if (HWSURFACE == true) hwsurface_cb->Check();
    child_widget.push_back(hwsurface_cb);

    doublebuf_lb->set(msg->get("Double buffering"));
    child_widget.push_back(doublebuf_lb);

    if (DOUBLEBUF == true) doublebuf_cb->Check();
    child_widget.push_back(doublebuf_cb);

    enable_joystick_lb->set(msg->get("Use joystick"));
    child_widget.push_back(enable_joystick_lb);

    if (ENABLE_JOYSTICK == true) enable_joystick_cb->Check();
    child_widget.push_back(enable_joystick_cb);
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
        saveSettings();
        requestedGameState = new GameStateTitle();
    } else if (cancel_button->checkClick()) {
        requestedGameState = new GameStateTitle();
    } else if (full_screen_cb->checkClick()) {
        if (full_screen_cb->isChecked()) FULLSCREEN=true;
        else FULLSCREEN=false;
    } else if (volume_music_sl->checkClick()) {
        MUSIC_VOLUME=volume_music_sl->getValue();
    } else if (volume_sound_sl->checkClick()) {
        SOUND_VOLUME=volume_sound_sl->getValue();
    } else if (mouse_move_cb->checkClick()) {
        if (mouse_move_cb->isChecked()) MOUSE_MOVE=true;
        else MOUSE_MOVE=false;
    } else if (combat_text_cb->checkClick()) {
        if (combat_text_cb->isChecked()) COMBAT_TEXT=true;
        else COMBAT_TEXT=false;
    } else if (hwsurface_cb->checkClick()) {
        if (hwsurface_cb->isChecked()) HWSURFACE=true;
        else HWSURFACE=false;
    } else if (doublebuf_cb->checkClick()) {
        if (doublebuf_cb->isChecked()) DOUBLEBUF=true;
        else DOUBLEBUF=false;
    } else if (enable_joystick_cb->checkClick()) {
        if (enable_joystick_cb->isChecked()) ENABLE_JOYSTICK=true;
        else ENABLE_JOYSTICK=false;
    }
}

    void
GameStateConfig::render ()
{
    for (unsigned int i = 0; i < child_widget.size(); i++) {
        child_widget[i]->render();
    }
}

