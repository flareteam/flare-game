#ifndef WIDGET_H
#define WIDGET_H

/**
 * Base interface all widget needs to implement
 */
class Widget {
public:
    virtual void    render  () = 0;
private:
};

#endif

