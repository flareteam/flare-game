#include "GameStateConfig.h"
#include "GameStateTitle.h"
#include "SharedResources.h"
#include "Settings.h"


GameStateConfig::GameStateConfig ()
        : GameState(),
          child_widget(),
          ok_button(NULL),
          cancel_button(NULL)

{
    tabControl = new WidgetTabControl(4);
    ok_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));
    cancel_button = new WidgetButton(mods->locate("images/menus/buttons/button_default.png"));

    for (unsigned int i = 0; i < 36; i++) {
         settings_lb[i] = new WidgetLabel();
    }

    for (unsigned int i = 0; i < 2; i++) {
         settings_sl[i] = new WidgetSlider(mods->locate("images/menus/buttons/slider_default.png"));
    }

    for (unsigned int i = 0; i < 6; i++) {
         settings_cb[i] = new WidgetCheckBox(mods->locate("images/menus/buttons/checkbox_default.png"));
    }

    for (unsigned int i = 0; i < 25; i++) {
         settings_key[i] = new WidgetInput();
    }

    //Load the menu configuration from file
    int x1;
    int y1;
    int x2;
    int y2;
    int setting_num;

    FileParser infile;
    if (infile.open(mods->locate("menus/config.txt"))) {
        while (infile.next()) {

            infile.val = infile.val + ',';
            x1 = eatFirstInt(infile.val, ',');
            y1 = eatFirstInt(infile.val, ',');
            x2 = eatFirstInt(infile.val, ',');
            y2 = eatFirstInt(infile.val, ',');

            setting_num = -1;

            if (infile.key == "fullscreen") setting_num = 1;
            else if (infile.key == "mouse_move") setting_num = 2;
            else if (infile.key == "combat_text") setting_num = 3;
            else if (infile.key == "hwsurface") setting_num = 4;
            else if (infile.key == "doublebuf") setting_num = 5;
            else if (infile.key == "enable_joystick") setting_num = 6;
            else if (infile.key == "music_volume") setting_num = 7;
            else if (infile.key == "sound_volume") setting_num = 8;
            else if (infile.key == "joystick_device") setting_num = 9;
            else if (infile.key == "resolution") setting_num = 10;
            else if (infile.key == "language")setting_num = 11;

            else if (infile.key == "cancel") setting_num = 12 + CANCEL;
            else if (infile.key == "accept") setting_num = 12 + ACCEPT;
            else if (infile.key == "up") setting_num = 12 + UP;
            else if (infile.key == "down") setting_num = 12 + DOWN;
            else if (infile.key == "left") setting_num = 12 + LEFT;
            else if (infile.key == "right") setting_num = 12 + RIGHT;
            else if (infile.key == "bar1") setting_num = 12 + BAR_1;
            else if (infile.key == "bar2") setting_num = 12 + BAR_2;
            else if (infile.key == "bar3") setting_num = 12 + BAR_3;
            else if (infile.key == "bar4") setting_num = 12 + BAR_4;
            else if (infile.key == "bar5") setting_num = 12 + BAR_5;
            else if (infile.key == "bar6") setting_num = 12 + BAR_6;
            else if (infile.key == "bar7") setting_num = 12 + BAR_7;
            else if (infile.key == "bar8") setting_num = 12 + BAR_8;
            else if (infile.key == "bar9") setting_num = 12 + BAR_9;
            else if (infile.key == "bar0") setting_num = 12 + BAR_0;
            else if (infile.key == "main1") setting_num = 12 + MAIN1;
            else if (infile.key == "main2") setting_num = 12 + MAIN2;
            else if (infile.key == "character") setting_num = 12 + CHARACTER;
            else if (infile.key == "inventory") setting_num = 12 + INVENTORY;
            else if (infile.key == "powers") setting_num = 12 + POWERS;
            else if (infile.key == "log") setting_num = 12 + LOG;
            else if (infile.key == "ctrl") setting_num = 12 + CTRL;
            else if (infile.key == "shift") setting_num = 12 + SHIFT;
            else if (infile.key == "delete") setting_num = 12 + DEL;

            if (setting_num != -1) {
                    settings_lb[setting_num-1]->setX((VIEW_W - 640)/2 + x1);
                    settings_lb[setting_num-1]->setY((VIEW_H - 480)/2 + y1);

                if (setting_num < 7) {
                    settings_cb[setting_num-1]->pos.x = (VIEW_W - 640)/2 + x2;
                    settings_cb[setting_num-1]->pos.y = (VIEW_H - 480)/2 + y2;
                } else if ((setting_num > 6) && (setting_num < 9)) {
                    settings_sl[setting_num-7]->pos.x = (VIEW_W - 640)/2 + x2;
                    settings_sl[setting_num-7]->pos.y = (VIEW_H - 480)/2 + y2;
                } else if ((setting_num > 8) && (setting_num < 12)) {
                    //settings_cmb[setting_num]->pos.x = (VIEW_W - 640)/2 + x2;
                    //settings_cmb[setting_num]->pos.y = (VIEW_H - 480)/2 + y2;
                } else if (setting_num > 11) {
                    settings_key[setting_num-12]->pos.x = (VIEW_W - 640)/2 + x2;
                    settings_key[setting_num-12]->pos.y = (VIEW_H - 480)/2 + y2;
                }
            }

          }
        }
        infile.close();

    // Initialize the tab control.
    tabControl->setMainArea((VIEW_W - 640)/2, (VIEW_H - 480)/2, 640, 480);

    // Define the header.
    tabControl->setTabTitle(0, msg->get("Video"));
    tabControl->setTabTitle(1, msg->get("Audio"));
    tabControl->setTabTitle(2, msg->get("Interface"));
    tabControl->setTabTitle(3, msg->get("Input"));
    tabControl->updateHeader();

    ok_button->label = msg->get("Ok");
    ok_button->pos.x = VIEW_W_HALF - ok_button->pos.w/2;
    ok_button->pos.y = VIEW_H - (cancel_button->pos.h*2);
    ok_button->refresh();
    child_widget.push_back(ok_button);

    cancel_button->label = msg->get("Cancel");
    cancel_button->pos.x = VIEW_W_HALF - cancel_button->pos.w/2;
    cancel_button->pos.y = VIEW_H - (cancel_button->pos.h);
    cancel_button->refresh();
    child_widget.push_back(cancel_button);

    settings_lb[0]->set(msg->get("Full Screen Mode"));
    child_widget.push_back(settings_lb[0]);
    optiontab[child_widget.size()-1] = 0;

    if (FULLSCREEN == true) settings_cb[0]->Check();
    child_widget.push_back(settings_cb[0]);
    optiontab[child_widget.size()-1] = 0;

    settings_lb[6]->set(msg->get("Music Volume"));
    child_widget.push_back(settings_lb[6]);
    optiontab[child_widget.size()-1] = 1;

    settings_sl[0]->set(0,128,MUSIC_VOLUME);
    child_widget.push_back(settings_sl[0]);
    optiontab[child_widget.size()-1] = 1;

    settings_lb[7]->set(msg->get("Sound Volume"));
    child_widget.push_back(settings_lb[7]);
    optiontab[child_widget.size()-1] = 1;

    settings_sl[1]->set(0,128,SOUND_VOLUME);
    child_widget.push_back(settings_sl[1]);
    optiontab[child_widget.size()-1] = 1;

    settings_lb[1]->set(msg->get("Move hero using mouse"));
    child_widget.push_back(settings_lb[1]);
    optiontab[child_widget.size()-1] = 3;

    if (MOUSE_MOVE == true) settings_cb[1]->Check();
    child_widget.push_back(settings_cb[1]);
    optiontab[child_widget.size()-1] = 3;

    settings_lb[2]->set(msg->get("Show combat text"));
    child_widget.push_back(settings_lb[2]);
    optiontab[child_widget.size()-1] = 2;

    if (COMBAT_TEXT == true) settings_cb[2]->Check();
    child_widget.push_back(settings_cb[2]);
    optiontab[child_widget.size()-1] = 2;

    settings_lb[3]->set(msg->get("Hardware surfaces"));
    child_widget.push_back(settings_lb[3]);
    optiontab[child_widget.size()-1] = 0;

    if (HWSURFACE == true) settings_cb[3]->Check();
    child_widget.push_back(settings_cb[3]);
    optiontab[child_widget.size()-1] = 0;

    settings_lb[4]->set(msg->get("Double buffering"));
    child_widget.push_back(settings_lb[4]);
    optiontab[child_widget.size()-1] = 0;

    if (DOUBLEBUF == true) settings_cb[4]->Check();
    child_widget.push_back(settings_cb[4]);
    optiontab[child_widget.size()-1] = 0;

    settings_lb[5]->set(msg->get("Use joystick"));
    child_widget.push_back(settings_lb[5]);
    optiontab[child_widget.size()-1] = 3;

    if (ENABLE_JOYSTICK == true) settings_cb[5]->Check();
    child_widget.push_back(settings_cb[5]);
    optiontab[child_widget.size()-1] = 3;

    // Add Key Binding objects
    for (unsigned int i = 11; i < 36; i++) {
         settings_lb[i]->set("Key");
         child_widget.push_back(settings_lb[i]);
         optiontab[child_widget.size()-1] = 3;
    }
    for (unsigned int i = 0; i < 25; i++) {
         child_widget.push_back(settings_key[i]);
         optiontab[child_widget.size()-1] = 3;
    }
}


GameStateConfig::~GameStateConfig()
{
    delete tabControl;

    for (unsigned int i = 0; i < child_widget.size(); i++) {
        delete child_widget[i];
    }
}


    void
GameStateConfig::logic ()
{
    tabControl->logic();

    if (ok_button->checkClick()) {
        saveSettings();
        requestedGameState = new GameStateTitle();
    } else if (cancel_button->checkClick()) {
        requestedGameState = new GameStateTitle();
    } else if (settings_cb[0]->checkClick()) {
        if (settings_cb[0]->isChecked()) FULLSCREEN=true;
        else FULLSCREEN=false;
    } else if (settings_sl[0]->checkClick()) {
        MUSIC_VOLUME=settings_sl[0]->getValue();
    } else if (settings_sl[1]->checkClick()) {
        SOUND_VOLUME=settings_sl[1]->getValue();
    } else if (settings_cb[1]->checkClick()) {
        if (settings_cb[1]->isChecked()) MOUSE_MOVE=true;
        else MOUSE_MOVE=false;
    } else if (settings_cb[2]->checkClick()) {
        if (settings_cb[2]->isChecked()) COMBAT_TEXT=true;
        else COMBAT_TEXT=false;
    } else if (settings_cb[3]->checkClick()) {
        if (settings_cb[3]->isChecked()) HWSURFACE=true;
        else HWSURFACE=false;
    } else if (settings_cb[4]->checkClick()) {
        if (settings_cb[4]->isChecked()) DOUBLEBUF=true;
        else DOUBLEBUF=false;
    } else if (settings_cb[5]->checkClick()) {
        if (settings_cb[5]->isChecked()) ENABLE_JOYSTICK=true;
        else ENABLE_JOYSTICK=false;
    }
}

    void
GameStateConfig::render ()
{
	// Tab control.
    tabControl->render();

    for (unsigned int i = 0; i < 2; i++) {
    child_widget[i]->render();
    }

    int active_tab = tabControl->getActiveTab();

    for (unsigned int i = 2; i < child_widget.size(); i++) {
         if (optiontab[i] == active_tab) child_widget[i]->render();
    }
}

