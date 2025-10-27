//
// Created by nmila on 9/5/2025.
//

#include "../HeaderFiles/Plot.h"

#include "../HeaderFiles/InputFunctions.h"

using std::string, std::map, std::shared_ptr, std::set, std::make_unique;

Plot::Plot(const OrderedPair _position_coordinates) : Plot() {
    position_coordinates = _position_coordinates;
}

Plot::Plot(const OrderedPair _position_coordinates, const tile_type _port_type, const bool _universal_port) : Plot() {
    position_coordinates = _position_coordinates;
    port_type = _port_type;
    universal_port = _universal_port;
}

Plot::Plot() = default;

OrderedPair Plot::get_position_coordinates() const {
    return position_coordinates;
}

string Plot::get_icon() const {
    string to_return;
    map<tile_type, string> conversion = {
        {GRAIN, "G"}, {WOOL, "W"}, {BRICK, "B"},
        {ORE, "O"}, {LUMBER, "L"}, {TILE_EMPTY, "X"}};
    if (!icon_override.empty()) {
        to_return = icon_override;
        if (universal_port) {
            to_return += "|?";
        } else if (port_type != TILE_EMPTY) {
            to_return += "|" + conversion.at(port_type);
        } else {
            to_return = " " + to_return + " ";
        }
    } else if (is_occupied()){
        to_return = inhabitant->get_icon();
        if (universal_port) {
            to_return += "|?";
        } else if (port_type != TILE_EMPTY) {
            to_return += "|" + conversion.at(port_type);
        } else {
            to_return = " " + to_return + " ";
        }
    } else {
        if (universal_port) {
            to_return = "|?|";
        } else if (port_type != TILE_EMPTY) {
            to_return = "|" + conversion.at(port_type) + '|';
        } else {
            to_return = " . ";
        }
    }
    return to_return;
}

tile_type Plot::get_port_type() const {
    return port_type;
}
bool Plot::is_universal_port() const {
    return universal_port;
}

string Plot::get_icon_override() const {
    return icon_override;
}

map<int, shared_ptr<Plot>> Plot::get_adjacent_plots() const{
    return adjacent_plots;
}


bool Plot::is_occupied() const {
    return (inhabitant != nullptr);
}

// Loops through adjacent plots to determine if this plot has a building next to it
// This is used for building placement, as a building cannot be next to another, regardless of what color they are
bool Plot::next_to_building() const {
    for (auto &it : adjacent_plots) {
        if (it.second->is_occupied()) {
            return true;
        }
    }
    return false;
}

Building* Plot::get_inhabitant() const {
    return inhabitant.get();
}

set<OrderedPair> Plot::get_adjacent_tile_coords() const {
    return adjacent_tile_coords;
}

void Plot::add_to_adjacent_tiles(const OrderedPair coords) {
    adjacent_tile_coords.insert(coords);
}


void Plot::set_inhabitant(Building new_inhabitant) {
    inhabitant = make_unique<Building>(new_inhabitant);
}

void Plot::set_icon_override(const string _icon_override) {
    icon_override = _icon_override;
}

void Plot::add_to_adjacent_plots(const int key, const shared_ptr<Plot> new_plot) {
    adjacent_plots.insert({key, new_plot});
}


