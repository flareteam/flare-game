#ifndef WIDGET_CHECKBOX_H
#define WIDGET_CHECKBOX_H

class Widget;

class WidgetCheckBox : public Widget {
public:
    WidgetCheckBox (const std::string & fname);
    ~WidgetCheckBox ();

    void Check ();
    bool checkClick ();
    bool isChecked () const;
    void render ();

    SDL_Rect pos; // This is the position of the button within the screen

private:
    std::string imgFileName;
    SDL_Surface * cb;
    bool checked;
    bool pressed;
};

#endif

