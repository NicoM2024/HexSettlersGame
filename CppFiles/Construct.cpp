//
// Created by nmila on 9/5/2025.
//

#include "../HeaderFiles/Construct.h"

using std::map, std::string, std::shared_ptr;

Construct::Construct(shared_ptr<Player> _owner, map<tile_type, int> _recipe) : Craftable(_recipe){
    owner = _owner;
    color = owner->get_color();
}

Construct::Construct(shared_ptr<Player> _owner){
    owner = _owner;
    color = owner->get_color();
}

Construct::Construct() : owner(nullptr), color(COLOR_EMPTY) {}

player_color Construct::get_color() const {
    return color;
}

/*
* Information on ANSI Escape Codes for colored text was found on this website:
* https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
*
* I was only able to get it to work thanks to a fix from this website:
* https://stackoverflow.com/questions/32742850/how-to-show-colored-console-output-in-clion
*
* The fixed involved including Windows.h and using the folling line of code which I don't necessarily understand, but it works:
* system(("chcp "s + std::to_string(CP_UTF8)).c_str());
 */


string Construct::get_icon(string icon_override) const {
    return icon_override;
}

shared_ptr<Player> Construct::get_owner() const {
    return owner;
}