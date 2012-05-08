#ifndef WIDGET_SLIDER_H
#define WIDGET_SLIDER_H

class Widget;

class WidgetSlider : public Widget {
public:
    WidgetSlider (const std::string & fname);
    ~WidgetSlider ();

    bool checkClick ();
    void set (int min, int max, int val);
    int getValue () const;
    void render ();

    SDL_Rect pos; // This is the position of the slider base within the screen
    SDL_Rect pos_knob; // This is the position of the slider's knob within the screen

private:
    std::string imgFileName;
    SDL_Surface * sl;
    bool pressed;
    int minimum;
    int maximum;
    int value;
};

#endif

