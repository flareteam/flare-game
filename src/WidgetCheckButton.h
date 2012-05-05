#ifndef WIDGET_CHECKBUTTON_H
#define WIDGET_CHECKBUTTON_H

class WidgetCheckButton {
public:
    WidgetCheckButton   (const std::string & fname);
    ~WidgetCheckButton  ();

    bool    checkClick  ();
    bool    isChecked   () const;
    void    render      ();

    SDL_Rect    pos;    ///< This is the position of the button within the screen

private:
    std::string imgFileName;
    SDL_Surface * cb;
    bool checked;
    bool pressed;
};

#endif

