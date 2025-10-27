//
// Created by nmila on 9/5/2025.
//

#include "../HeaderFiles/Road.h"
#include "../HeaderFiles/Player.h"

using std::map, std::string, std::shared_ptr;

map<tile_type, int> Road::get_recipe() const {
    return {{BRICK, 1}, {LUMBER, 1}};
}

Road::Road(const shared_ptr<Player> _owner) : Construct(_owner, get_recipe()) {}
Road::Road() : Construct(nullptr, get_recipe()) {}

// Returns the icon, but made the color of this road.
string Road::get_icon(string icon_override) const {

    string color_code = get_player_color_code(color);
    string reset_code = get_cancel_code();

    return color_code + icon_override + reset_code;
}