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
    WidgetLabel         * volume_sound_lb;
    WidgetSlider        * volume_sound_sl;
    WidgetLabel         * mouse_move_lb;
    WidgetCheckBox	    * mouse_move_cb;
    WidgetLabel         * combat_text_lb;
    WidgetCheckBox	    * combat_text_cb;
    WidgetLabel         * hwsurface_lb;
    WidgetCheckBox	    * hwsurface_cb;
    WidgetLabel         * doublebuf_lb;
    WidgetCheckBox	    * doublebuf_cb;
    WidgetLabel         * enable_joystick_lb;
    WidgetCheckBox	    * enable_joystick_cb;
};

#endif

