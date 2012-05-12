#ifndef GAMESTATECONFIG_H
#define GAMESTATECONFIG_H

#include <vector>
#include "GameState.h"
#include "WidgetButton.h"
#include "WidgetCheckBox.h"
#include "WidgetComboBox.h"
#include "WidgetSlider.h"
#include "WidgetTabControl.h"
#include "WidgetInput.h"

class GameStateConfig : public GameState {
public:
    GameStateConfig    ();
    ~GameStateConfig   ();

    void    logic   ();
    void    render  ();

private:
    int optiontab[74];
    SDL_Rect** video_modes;

    std::string * language_ISO;
    std::string * language_full;

    int getVideoModes(void);
    bool getLanguagesList(void);
    int getLanguagesNumber(void);

    std::vector<Widget*>      child_widget;
    WidgetTabControl    * tabControl;
    WidgetButton        * ok_button;
    WidgetButton        * cancel_button;

    WidgetLabel         * settings_lb[36];
    WidgetSlider        * settings_sl[2];
    WidgetCheckBox      * settings_cb[6];
    WidgetInput         * settings_key[25];
    WidgetComboBox      * settings_cmb[3];
};

#endif

