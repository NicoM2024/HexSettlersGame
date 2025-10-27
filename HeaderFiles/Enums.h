//
// Created by nmila on 9/21/2025.
//

#ifndef ENUMS_H
#define ENUMS_H

#include <string>

// enum type for all available player colors
enum player_color{RED = 0, YELLOW = 1, GREEN = 2, BLUE = 3, COLOR_EMPTY = 4};

// Types a building can be
enum building_type{SETTLEMENT, CITY};

// Types a devcard can be
enum devcard_type {KNIGHT, ROAD_BUILDING, YEAR_OF_PLENTY, MONOPOLY, VICTORY_POINT, CARD_EMPTY};

// The types that a tile can be and the types of resources that exist
enum tile_type{DESERT, GRAIN, WOOL, BRICK, ORE, LUMBER, TILE_EMPTY};

/**
* Requires: color
* Modifies: nothing
* Effects: returns the ANSI escape code for the color
 */
inline std::string get_player_color_code(player_color color) {
	switch (color) {
		case RED:
			return "\033[91;1m";
		case BLUE:
			return "\033[94;1m";
		case GREEN:
			return "\033[92;1m";
		case YELLOW:
			return "\033[93;1m";
		case COLOR_EMPTY:
			return "";
	}
	return "";
}

/**
* Requires: tile_type
* Modifies: nothing
* Effects: returns the ANSI escape code for the text effect corresponding to the tile_type
*/
inline std::string get_resource_color_code(tile_type type) {
	switch (type) {
		case DESERT:
			return "\033[38;5;247m";
		case GRAIN:
			return "\033[38;2;223;197;123m";
		case WOOL:
			return "\033[92m";
		case BRICK:
			return "\033[38;2;228;8;10m";
		case ORE:
			return "\033[36m";
		case LUMBER:
			return"\033[32m";
		case TILE_EMPTY:
			return "";
	}
	return "";
}

/**
* Requires: nothing
* Modifies: nothing
* Effects: returns the ANSI escape code to cancel colors and effects
*/
inline std::string get_cancel_code() {
	return "\033[0m";
}

#endif //ENUMS_H
