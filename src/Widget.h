#ifndef WIDGET_H
#define WIDGET_H

/**
 * Base interface all widget needs to implement
 */
class Widget {
public:
    Widget() {};
	virtual ~Widget() {};
	virtual void render() {};
private:
};

#endif

