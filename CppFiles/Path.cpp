//
// Created by nmila on 9/5/2025.
//

#include "../HeaderFiles/Path.h"

using std::string, std::shared_ptr, std::make_unique, std::set;

Path::Path(const EndpointPair _endpoints, const OrderedPair anchor_tile_coords) {
    endpoints = _endpoints;
    direction = calculate_direction(endpoints.plot_a->get_position_coordinates(),
        endpoints.plot_b->get_position_coordinates(), anchor_tile_coords);
}

Path::Path() : direction(0) {}

EndpointPair Path::get_endpoints() const {
    return endpoints;
}

// This function returns the paths icon
string Path::get_icon() const {

    // If there is an icon override, make the icon that instead
    if (icon_override != "") {
        if (direction == 0) {
            // If the direction in 0, however, the icon needs to be 5 chars long, so append some chars to the start and end
            return "- " + icon_override + " -";
        }
        return icon_override;
    }

    // If there wasn't an override, we set up what the icon should be based on position
    string icon;
    int dir = get_direction();
    switch (dir) {
        case -1:
            // icon is 2 backslashes because it is an escape character
            icon = "\\";
            break;
        case 1:
            icon = "/";
            break;
        case 0:
            icon = "-----";
            break;
    }

    if (is_occupied()){
        // defers icon retrieval to the inhabitant if there is one which just adds color to it.
        return inhabitant->get_icon(icon);
    }
    return icon;
}

// Loops through adjacent paths to see if the path has a matching road of the given color
// This is used for determining if a road can be placed in a particular spot, as for most of the game they must be placed adjacent to another of the same color
bool Path::has_adjacent_matching_road(const player_color color) const {
    for (shared_ptr<Path> path : adjacent_paths) {
        if (!path->is_occupied()) {
            continue;
        }
        if (path->get_inhabitant()->get_color() == color) {
            return true;
        }
    }
    return false;
}

Road* Path::get_inhabitant() const {
    return inhabitant.get();
}

set<shared_ptr<Path>> Path::get_adjacent_paths() const {
    return adjacent_paths;
}

bool Path::is_occupied() const {
    return inhabitant != nullptr;
}

int Path::get_direction() const {
    return direction;
}

int Path::calculate_direction(
    OrderedPair endpointa, OrderedPair endpointb,
    const OrderedPair anchor_tile_coords) const {

    // Convert endpoint positions to local coordinates
    endpointa = endpointa - anchor_tile_coords;
    endpointb = endpointb - anchor_tile_coords;

    // Here, I am magnifying the points (-1, 0) and (1, 0) to (-2, 0) and (2, 0) respectively.
    // I am doing this because my hexagonal grid system is strange, and visually the slope between
    // (1, 1) and (1, 0) is negative, but mathematically is not. This operation insures visual accuracy
    if (endpointa.x % 2 != 0 && endpointa.y % 2 == 0) {
        endpointa.x *= 2;
    }

    if (endpointb.x % 2 != 0 && endpointb.y % 2 == 0) {
        endpointb.x *= 2;
    }

    // Use slope formula on endpoints to get the slope of the path as if it was a line in cartesian coordinates
    double slope_rise = endpointa.y - endpointb.y;
    double slope_run = endpointa.x - endpointb.x;
    double slope = slope_rise / slope_run;

    if (slope == 0) {
        return 0;
    }
    if (slope > 0) {
        return 1;
    }
    return -1;

}

void Path::set_inhabitant(Road new_inhabitant) {
    inhabitant = make_unique<Road>(new_inhabitant);
}

string Path::get_icon_override() const {
    return icon_override;
}

void Path::set_icon_override(const string _icon_override) {
    icon_override = _icon_override;
}

void Path::add_to_adjacent_paths(shared_ptr<Path> &new_path) {
    adjacent_paths.insert(new_path);
}


