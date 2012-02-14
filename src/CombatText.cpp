#include "CombatText.h"
#include <iostream>
#include <sstream>

// Global static pointer used to ensure a single instance of the class.
CombatText* CombatText::m_pInstance = NULL;  
  
CombatText* CombatText::Instance() {
   if (!m_pInstance)
      m_pInstance = new CombatText;

   return m_pInstance;
}

void CombatText::setCam(Point location) {
    cam = location;
}

void CombatText::addMessage(std::string message, Point location, int displaytype) {
	Point p = map_to_screen(location.x - UNITS_PER_TILE, location.y - UNITS_PER_TILE, cam.x, cam.y);
    Combat_Text_Item *c = new Combat_Text_Item();
    WidgetLabel *label = new WidgetLabel();
    c->pos = p;
    c->label = label;
    c->text = message;
    c->lifespan = 30;
    c->displaytype = displaytype;
    combat_text.push_back(*c);
    delete c;
}

void CombatText::addMessage(int num, Point location, int displaytype) {
	Point p = map_to_screen(location.x - UNITS_PER_TILE, location.y - UNITS_PER_TILE, cam.x, cam.y);
    Combat_Text_Item *c = new Combat_Text_Item();
    WidgetLabel *label = new WidgetLabel();
    c->pos = p;
    c->label = label;
    
    std::stringstream ss;
    ss << num;
    c->text = ss.str();
    
    c->lifespan = 30;
    c->displaytype = displaytype;
    combat_text.push_back(*c);
    delete c;
}

void CombatText::render() {
	for(std::vector<Combat_Text_Item>::iterator it = combat_text.begin(); it != combat_text.end(); it++) {
        it->lifespan--;
        it->pos.y--;
        int type = it->displaytype;
        if (type == DISPLAY_DAMAGE)
            it->label->set(it->pos.x, it->pos.y, JUSTIFY_CENTER, VALIGN_BOTTOM, it->text, FONT_WHITE);
        else if (type == DISPLAY_CRIT || type == DISPLAY_MISS)
            it->label->set(it->pos.x, it->pos.y, JUSTIFY_CENTER, VALIGN_BOTTOM, it->text, FONT_RED);
        else if (type == DISPLAY_HEAL)
            it->label->set(it->pos.x, it->pos.y, JUSTIFY_CENTER, VALIGN_BOTTOM, it->text, FONT_GREEN);
        if (it->lifespan > 0)
		    it->label->render();
    }
    // delete expired messages
    while (combat_text.size() > 0 && combat_text.begin()->lifespan <= 0) {
        combat_text.erase(combat_text.begin());
    }
}
