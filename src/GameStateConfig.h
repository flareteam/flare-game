#ifndef GAMESTATECONFIG_H
#define GAMESTATECONFIG_H

#include <vector>
#include "GameState.h"

class WidgetButton;
class WidgetCheckBox;
class WidgetSlider;

class GameStateConfig : public GameState {
public:
    GameStateConfig    ();
    ~GameStateConfig   ();

    void    logic   ();
    void    render  ();

private:
    std::vector<Widget*>      child_widget;
    WidgetButton	    * ok_button;
    WidgetButton	    * cancel_button;
    WidgetLabel		    * full_screen_lb;
    WidgetCheckBox	    * full_screen_cb;
    WidgetLabel         * volume_music_lb;
    WidgetSlider        * volume_music_sl;
};

#endif

