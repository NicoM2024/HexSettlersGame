//
// Created by nmila on 9/5/2025.
//

#include "../HeaderFiles/Building.h"
#include "../HeaderFiles/Player.h"

using std::map, std::shared_ptr, std::string;

map<tile_type, int> Building::get_recipe() const {
    if (type == SETTLEMENT) {
        return {{GRAIN, 1}, {WOOL, 1}, {ORE, 1}};
    } if (type == CITY) {
        return {{GRAIN, 1}, {WOOL, 1}, {ORE, 1}};
    }
    return {{}};
}

Building::Building(
    const shared_ptr<Player> _owner, const building_type _type) : Construct(_owner) {
	type = _type;
    if (type == SETTLEMENT) {
        resource_production = 1;
        set_recipe(get_recipe());
    } else if (type == CITY) {
        resource_production = 2;
        set_recipe(get_recipe());
    } else {
        resource_production = 0;
    }
}

Building::Building(const building_type _type) : Construct(nullptr), resource_production(0) {
    type = _type;
    if (type == SETTLEMENT) {
        set_recipe(get_recipe());
    } else if (type == CITY) {
        set_recipe(get_recipe());
    }
}

Building::Building() : Construct(nullptr), type(SETTLEMENT), resource_production(0) {
    set_recipe(get_recipe());
}

building_type Building::get_building_type() const {
    return type;
}

// Returns the building icon, colored with the player color. The icon is dependent on the building type
string Building::get_icon(string icon_override) const {

    string icon = icon_override;
    if (icon == "") {
        if (type == SETTLEMENT) {
            icon = "S";
        } else if (type == CITY) {
            icon = "C";
        } else {
            icon = "?";
        }
    }

    // Returns early if the building is not of one of the valid types, also not giving the icon its color
    if (icon == "?") {
        return icon;
    }

    // gets color code and reset code, to return the string as the correct color
    string color_code = get_player_color_code(color);
    string reset_code = get_cancel_code();

    return color_code + icon + reset_code;

}

int Building::get_resource_production() const {
    return resource_production;
}

void Building::upgrade() {
    if (type != SETTLEMENT) {
        return;
    }

    type = CITY;
    resource_production = 2;
}
