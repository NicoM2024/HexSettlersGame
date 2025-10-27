//
// Created by nmila on 9/22/2025.
//

#include "../HeaderFiles/Craftable.h"

using std::map, std::string;

Craftable::Craftable(map<tile_type, int> _recipe){
    recipe = _recipe;
}

Craftable::Craftable(){
    recipe = {};
}

map<tile_type, int> Craftable::get_recipe() const{
    return recipe;
}

void Craftable::set_recipe(map<tile_type, int> _recipe) {
    recipe = _recipe;
}

map<tile_type, int> Craftable::get_recipe(string craftable_thing){
    for (int i = 0; i < craftable_thing.size(); i++) {
        craftable_thing[i] = tolower(craftable_thing[i]);
    }
    if (craftable_thing == "settlement" || craftable_thing == "settle" || craftable_thing == "s") {
        return {{GRAIN, 1}, {WOOL, 1}, {BRICK, 1}, {LUMBER, 1}};
    }
    if (craftable_thing == "city" || craftable_thing == "c") {
        return {{GRAIN, 2}, {ORE, 3}};
    }
    if (craftable_thing == "road" || craftable_thing == "r") {
        return {{BRICK, 1}, {LUMBER, 1}};
    }
    if (craftable_thing == "development card" || craftable_thing == "devcard" || craftable_thing == "d"){
        return {{GRAIN, 1}, {WOOL, 1}, {ORE, 1}};
    }
    return {};
}