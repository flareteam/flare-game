/*
Copyright © 2012 David Bariod

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/
#include <ostream>
#include <SDL.h>

#include "Utils.h"
#include "UtilsDebug.h"

using namespace std;


        ostream &
operator<< (ostream             & os,
            const SDL_Event     & evt)
{
    switch (evt.type) {
    case SDL_ACTIVEEVENT:
        os << reinterpret_cast<const SDL_ActiveEvent&>(evt);
        break;
    case SDL_KEYUP:
    case SDL_KEYDOWN:
        os << reinterpret_cast<const SDL_KeyboardEvent&>(evt);
        break;
    case SDL_MOUSEMOTION:
        os << reinterpret_cast<const SDL_MouseMotionEvent&>(evt);
        break;
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
        os << reinterpret_cast<const SDL_MouseButtonEvent&>(evt);
        break;
    case SDL_JOYAXISMOTION:
        os << reinterpret_cast<const SDL_JoyAxisEvent&>(evt);
        break;
    case SDL_JOYBALLMOTION:
        os << reinterpret_cast<const SDL_JoyBallEvent&>(evt);
        break;
    case SDL_JOYHATMOTION:
        os << reinterpret_cast<const SDL_JoyHatEvent&>(evt);
        break;
    case SDL_JOYBUTTONUP:
    case SDL_JOYBUTTONDOWN:
        os << reinterpret_cast<const SDL_JoyButtonEvent&>(evt);
        break;
    case SDL_QUIT:
        os << reinterpret_cast<const SDL_QuitEvent&>(evt);
        break;
    case SDL_SYSWMEVENT:
        os << reinterpret_cast<const SDL_SysWMEvent&>(evt);
        break;
    case SDL_VIDEORESIZE:
        os << reinterpret_cast<const SDL_ResizeEvent&>(evt);
        break;
    case SDL_VIDEOEXPOSE:
        os << reinterpret_cast<const SDL_ExposeEvent&>(evt);
        break;
    case SDL_USEREVENT:
        os << "User Event";
        break;
    default:
        os << "Unknown event: " << evt.type;
        return os;
    }

    return os;
}


    ostream &
operator<< (ostream                 & os,
            const SDL_ActiveEvent   & evt)
{
    os << "{SDL_ACTIVE_EVENT, gain = " << static_cast<uint16_t>(evt.gain)
            << ", state = " << static_cast<uint16_t>(evt.state) << "}";
    return os;
}


    ostream &
operator<< (ostream                 & os,
            const SDL_KeyboardEvent & evt)
{
    os << "{";
    if (SDL_KEYDOWN == evt.type) {
        os << "SDL_KEYDOWN";
    } else if (SDL_KEYUP == evt.type) {
        os << "SDL_KEYUP";
    } else {
        os << "Unexpected value type: " << evt.type << "}";
        return os;
    }
    os << ", state";
    if (SDL_PRESSED == evt.state) {
        os << " = SDL_PRESSED";
    } else if (SDL_RELEASED == evt.state) {
        os << " = SDL_RELEASED";
    } else {
        os << " = ??" << evt.state;
    }
    os << ", SDL_keysym: " << evt.keysym << "}";
    return os;
}


    ostream &
operator<< (ostream             & os,
            const SDL_keysym    & ks)

{
    os << "{scancode = " << static_cast<uint16_t>(ks.scancode)
            << ", sym = " << ks.sym << ", mod = " << ks.mod << ", unicode = " << ks.unicode << "}";
    return os;
}


    ostream &
operator<< (ostream                     & os,
            const SDL_MouseMotionEvent  & evt)
{
    os << "{SDL_MOUSEMOTION, state = " << static_cast<uint16_t>(evt.state)
            << ", (x,y) = (" << evt.x << "," << evt.y << ")"
            << ", (xrel,yrel) = (" << evt.xrel << "," << evt.yrel << ")}";
    return os;
}


    ostream &
operator << (ostream                    & os,
             const SDL_MouseButtonEvent & evt)
{
    os << "{SDL_MOUSEBUTTON, type = ";
    if (SDL_MOUSEBUTTONDOWN == evt.type) {
        os << "DOWN";
    } else if (SDL_MOUSEBUTTONUP == evt.type) {
        os << "UP";
    } else {
        os << "??" << evt.type << "}";
        return os;
    }
    os << ", button = " << static_cast<uint16_t>(evt.button) << ", state = ";
    if (SDL_PRESSED == evt.state) {
        os << "SDL_PRESSED";
    } else if (SDL_RELEASED == evt.state) {
        os << "SDL_RELEASED";
    } else {
        os << "??" << static_cast<uint16_t>(evt.state);
    }
    os << ", (x,y) = (" << evt.x << "," << evt.y << ")}";
    return os;
}


    ostream &
operator<< (ostream                 & os,
            const SDL_JoyAxisEvent  & evt)
{
    os << "{SDL_JOYAXIS, which = " << static_cast<uint16_t>(evt.which)
            << ", axis = " << static_cast<uint16_t>(evt.axis) << ", value = " << evt.value << "}";
    return os;
}


    ostream &
operator<< (ostream                 & os,
            const SDL_JoyBallEvent  & evt)
{
    os << "{SDL_JOYBALLMOTION, which = " << static_cast<uint16_t>(evt.which)
            << ", ball = " << static_cast<uint16_t>(evt.ball)
            << ", (xrel,yrel) = " << "(" << evt.xrel << "," << evt.yrel << ")}";
    return os;
}


    ostream &
operator<< (ostream                 & os,
            const SDL_JoyHatEvent   & evt)
{
    os << "{SDL_JOYHATEVENT, which = " << static_cast<uint16_t>(evt.which)
            << ", hat = " << static_cast<uint16_t>(evt.hat)
            << ", value = " << static_cast<uint16_t>(evt.value) << "}";
    return os;
}


    ostream &
operator<< (ostream                     & os,
            const SDL_JoyButtonEvent    & evt)
{
    if (SDL_JOYBUTTONDOWN == evt.type) {
        os << "{SDL_JOYBUTTONDOWN, ";
    } else if (SDL_JOYBUTTONUP == evt.type) {
        os << "{SDL_JOYBUTTONUP, ";
    } else {
        os << "{??unknown " << evt.type;
        return os;
    }
    os << "{SDL_JOYBUTTONEVENT, which = " << static_cast<uint16_t>(evt.which)
            << ", button = " << static_cast<uint16_t>(evt.button) << ", state = ";
    if (SDL_PRESSED == evt.state) {
        os << "SDL_PRESSED}";
    } else if (SDL_RELEASED == evt.state) {
        os << "SDL_RELEASED}";
    } else {
        os << "??" << static_cast<uint16_t>(evt.state) << "}";
    }
    return os;
}


    ostream &
operator<< (ostream & os, const SDL_QuitEvent &evt)
{
    os << "{SDL_QUITEVENT}";
    return os;
}

    ostream &
operator<< (ostream & os, const SDL_SysWMEvent &evt)
{
    os << "{SDL_SYSWMEVENT}";
    return os;
}

    ostream &
operator<< (ostream & os, const SDL_ResizeEvent &evt)
{
    os << "{SDL_RESIZE_EVENT, (w,h) = (" << evt.w << "," << evt.h << ")}";
    return os;
}

    ostream &
operator<< (ostream & os, const SDL_ExposeEvent &evt)
{
    os << "{SDL_EXPOSEEVENT}";
    return os;
}

	ostream &
operator<< (ostream & os, const SDL_Rect & rect)
{
	os << "(x,y,h,w) = (" << rect.x << "," << rect.y << "," << rect.h << "," << rect.w << ")";
	return os;
}

	ostream &
operator<< (ostream & os, const Point & p)
{
	os << "(x,y) = (" << p.x << "," << p.y << ")";
	return os;
}
