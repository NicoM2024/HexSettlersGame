//
// Created by nmila on 9/4/2025.
//

#include "../HeaderFiles/Tile.h"
#include "../HeaderFiles/Board.h"

#include <iostream>

using std::cout, std::endl;
using std::string, std::map, std::shared_ptr, std::to_string;

Tile::Tile(
    const tile_type _type,
    const int _production_number,
    const string _position_id,
    const OrderedPair _position_coordinate,
    const map<int, shared_ptr<Plot>> _adjacent_plots,
    const map<int, shared_ptr<Path>> _adjacent_paths) {

    type = _type;
    if (type == DESERT) {
        // The robber always starts on the desert and only on the desert
        robber = true;
    } else {
        robber = false;
    }

    production_number = _production_number;
    position_id = _position_id;
    position_coordinate = _position_coordinate;

    adjacent_plots = _adjacent_plots;
    adjacent_paths = _adjacent_paths;

}

Tile::Tile() {
    type = TILE_EMPTY;
    production_number = 0;
}

bool Tile::is_valid() const {
    return type != TILE_EMPTY;
}

OrderedPair Tile::get_position_coordinate() const{
    return position_coordinate;
}

string Tile::get_position_id() const{
    return position_id;
}

tile_type Tile::get_type() const{
    return type;
}

// Returns a string which is the entire middle row within one hex, including production number and material type.
string Tile::get_main_display_row(const bool lowlight_row) const{

    string type_icon = "???";
    switch (type) {
        case DESERT:
            type_icon = "D";
            break;
        case GRAIN:
            type_icon = "G";
            break;
        case WOOL:
            type_icon = "W";
            break;
        case BRICK:
            type_icon = "B";
            break;
        case ORE:
            type_icon = "O";
            break;
        case LUMBER:
            type_icon = "L";
            break;
    }

    // Currently, with || true, disabled colored tile icons. Made the board too messy
    if (lowlight_row || true) {
        type_icon = "(" + type_icon + ")";
    } else {
        type_icon = "(" + get_resource_color_code(type) + type_icon + get_cancel_code() + ")";
    }


    string num_as_string = " ";
    if (type == DESERT) {
        // If the tile is a desert, it has no production number, but needs to be spaced properly still
        type_icon += " ";
    } else {
        num_as_string = to_string(production_number);
        if (num_as_string.length() == 1){
            num_as_string += " ";
        }
    }

    // combines number (if there is one) and icon
    string num_and_icon = num_as_string + type_icon;

    // returns num_and_icon with proper amount of space on either side
    if (lowlight_row) {
        return "\033[38;5;246m  " + num_and_icon + "  \033[0m";
    }
    return "  " + num_and_icon + "  ";
}

// The robber display row is beneath the middle row of a tile. This returns the contents of that
string Tile::get_robber_display_row() const {
    if (has_robber()) {
        return "   !R!   ";
    }
    return "         ";
}

string Tile::get_top_display_row(const bool display_position_id) const {
    if (display_position_id) {
        return "   \033[38;5;247m" + position_id + "\033[0m   ";
    }
    return "         ";
}

bool Tile::has_robber() const {
    return robber;
}

void Tile::set_robber(const bool _robber) {
    robber = _robber;
}

int Tile::get_production_number() const {
    return production_number;
}

const map<int, shared_ptr<Plot>> Tile::get_adjacent_plots() const {
    return adjacent_plots;
}

const map<int, shared_ptr<Path>> Tile::get_adjacent_paths() const {
    return adjacent_paths;
}

string Tile::type_to_string(const tile_type _type) {
    string to_return = "???";
    switch (_type) {
        case DESERT: to_return = "DESERT"; break;
        case GRAIN: to_return = "GRAIN"; break;
        case WOOL: to_return = "WOOL"; break;
        case BRICK: to_return = "BRICK"; break;
        case ORE: to_return = "ORE"; break;
        case LUMBER: to_return = "LUMBER"; break;
        case TILE_EMPTY: to_return = "???"; break;
    }
    return get_resource_color_code(_type) + to_return + get_cancel_code();
}

// Produces the resource for each building adjacent to the tile, and provides it to the player who owns the building(s)
// Cities produce 2 resource, Settlements produce 1
void Tile::produce_resource(const bool print) const {
    if (has_robber()) {
        // If the robber is on the tile, don't produce any resources
        return;
    }
    // This whole rigamarole generates resources for each player one at a time and makes sure it doesn't print about their reosurce gains more than once.
    // For example, if a player gains 1 grain from two different tiles, it will just say 2 grain, not 1 grain twice.
    for (player_color color : {RED, YELLOW, GREEN, BLUE}) {

        map<tile_type, int> resource_gains = Player::get_base_resource_map();
        shared_ptr<Player> current_player = nullptr;

        for (auto &pair : adjacent_plots) {
            if (!pair.second->is_occupied()) {
                continue; }
            if (pair.second->get_inhabitant()->get_color() != color) {
                continue; }
            current_player = pair.second->get_inhabitant()->get_owner();
            resource_gains[type] += pair.second->get_inhabitant()->get_resource_production();
        }

        if (current_player == nullptr) {
            continue; }
        current_player->gain_many_resources(resource_gains, true, print);
    }
}

void Tile::add_to_adjacent_plots(const int key, shared_ptr<Plot> plot) {
    adjacent_plots.insert({key, plot});
}

void Tile::add_to_adjacent_paths(const int key, shared_ptr<Path> path) {
    adjacent_paths.insert({key, path});
}

// This sets the plot icon override, which is either nothing if it has a building, X if it is invalid, or its index if it is valid
void Tile::override_plot_icons(const Board &board, const player_color color, const bool require_adjacent_road) {
    for (auto &pair : adjacent_plots) {
        if (pair.second->is_occupied()) {
            continue;
        }

        bool conforms_to_road_rule = true;
        if (require_adjacent_road) {
            conforms_to_road_rule = false;
            for (auto &adj : pair.second->get_adjacent_plots()) {
                shared_ptr<Path> path = board.get_path(adj.second->get_position_coordinates(),
                    pair.second->get_position_coordinates());
                if (path == nullptr) {
                    continue;
                }

                if (path->is_occupied() && path->get_inhabitant()->get_color() == color) {
                    conforms_to_road_rule = true;
                }
            }
        }

        if (pair.second->next_to_building() || !conforms_to_road_rule) {
            pair.second->set_icon_override("X");
        } else {
            pair.second->set_icon_override("\033[1m" + to_string(pair.first) + "\033[1m");
        }
    }
}

// THis sets the poit icon override, but only if there is a valid settlement.
void Tile::override_settlement_icons(const player_color _color) {
    for (auto &pair : adjacent_plots) {
        if (!pair.second->is_occupied()) {
            continue;
        }

        if (pair.second->get_inhabitant()->get_building_type() != SETTLEMENT) {
            continue;
        }

        if (pair.second->get_inhabitant()->get_color() != _color) {
            continue;
        }

        pair.second->set_icon_override("\033[1m" + to_string(pair.first) + "\033[0m");
    }
}

// Overrides path icons, which will do nothing if the path has a road, will set it to X if the path is invalid, or will set it to its index if it is valid
void Tile::override_path_icons(const player_color color, const bool require_adjacent_road) {
    for (int i = 0; i < adjacent_paths.size(); i++) {
        shared_ptr<Path> path = adjacent_paths[i];
        if (adjacent_paths[i]->is_occupied()) {
            continue;
        }
        if (require_adjacent_road && !adjacent_paths[i]->has_adjacent_matching_road(color)) {
            adjacent_paths[i]->set_icon_override("X");
            continue;
        }
        adjacent_paths[i]->set_icon_override("\033[1m" + to_string(i) + "\033[1m");
    }
}

// Resets the icon overrides for all plots, making them display their typical icons
void Tile::reset_plot_icon_overrides() {
    for (int i = 0; i < adjacent_plots.size(); i++) {
        adjacent_plots[i]->set_icon_override("");
    }
}

// Resets the icon overrides for all paths, making them display their typical icons
void Tile::reset_path_icon_overrides() {
    for (int i = 0; i < adjacent_paths.size(); i++) {
        adjacent_paths[i]->set_icon_override("");
    }
}

