//
// Created by nmilazz on 9/4/2025.
//

#include "../HeaderFiles/Board.h"
#include "../HeaderFiles/Player.h"

#include <iostream>
#include <ostream>
#include <algorithm>

#include "../HeaderFiles/InputFunctions.h"

using std::cout, std::endl;
using std::vector, std::shared_ptr, std::string, std::map, std::set, std::make_shared;
using std::pair, std::rotate;

/* Store grid tile data in a polar format. Fancy, and probably impractical and very niche
* Ex:
*
* Ring 0 (Middle) has tile a.                                         Coords = 0a
* Ring 1 (Next out from middle) has tiles a, b, c, d, e, f.           Coords = 1a, 1b, 1c, 1d, 1e, 1f, 1g
* Ring 2 (Outer Ring) has tiles a, b, c, d, e, f, g, h, i, j, k, l    Coords = 2a, 2b,..., 2l
*
* Symbols and Meanings:
*	C = City, S = Settlement,
*
* Off Limits characters for coords: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, G, W, L, B, D, O, !, {}, ()
*
* Catan Board is a large hexagon made of 19 smaller hexagons (or more/less for custom boards)
* Example Board:
*
*     //8  spaces              S:?----- O
*     //10 spaces              /         \
*     //12 spaces     O ----- .   11(L)   O ----- O
*     //10 spaces    /         \         /         \
*           O ----- O   4 (B)   O ----- O   12(W)   O ----- O
*          /         \         /         \         /         \
*         O    (D)    O ----- O   6 (O)   O ----- O   9 (G)   O
*          \  {!R!}  /         \         /         \         /
*           O ----- O   3 (L)   O ----- O   5 (B)   O ----- O
*          /         \         /         \         /         \
*         O   8 (B)   O ----- O   11(G)   O ----- O   10(W)   O
*          \         /         \         /         \         /
*           O ----- O   10(W)   O ----- O   4 (L)   O ----- O
*          /         \         /         \         /         \
*         O   5 (O)   O ----- O   9 (W)   O ----- O   8 (G)   O
*          \         /         \         /         \         /
*           O ----- O   2 (G)   O ----- O   3 (S)   O ----- O
*                    \         /         \         /
*                     O ----- O   6 (L)   O ----- O
*                              \         /
*                               O ----- O
*

 */

Board::Board() : board_tiles({{'a', {}},{'b', {}},{'c', {}}}){
	create_board(available_tile_types, available_production_numbers);
}

// parameters passed by value, so I can make a copy and modify them in the function
void Board::create_board(
	map<tile_type, int> _available_tile_types, vector<int> _available_production_numbers){

	populate_port_pairs();

	// Map stores info for the three rings of the map, labeled 'a', 'b', and 'c'

	// Each ring label is a key in a map whose value is a string.
	// Each char in the string is the second half of a tile's position ID. The length of the string == # of tiles in the given ring
	map<char, string> ring_layers =
		{{'a', "a"}, {'b', "abcdef"}, {'c', "abcdefghijkl"}};

	// Outer loop loops through the three ring labels 'a', 'b', and 'c' in the map
	for (auto& m : ring_layers) {
		// This inner loop loops through the current ring's string, each character being provided to a tile as part of its position_id
		for (char ch : m.second) {
			// Chooses the type and number of the tile being created. Both are required for the Tile constructor
			tile_type chosen_type = get_random_tile_type(_available_tile_types);
			int chosen_number = -1;
			if (chosen_type != DESERT) {
				chosen_number =
					get_random_production_number(_available_production_numbers);
			}

			// The position_id is created and the map key, a comma, and the current string char are appended to it
			string tile_position_id;
			tile_position_id.push_back(m.first);
			tile_position_id.push_back(',');
			tile_position_id.push_back(ch);

			OrderedPair position_coordinates =
				get_tile_position_coordinates(tile_position_id);
			// create a set of shared pointers to the adjacent plots to the tile being created. This is required for the Tile constructor
			map<int, shared_ptr<Plot>> adjacent_plots;
			fill_adjacent_plots(adjacent_plots, position_coordinates);

			// create a set of shared pointers to the adjacent paths to the tile being created. This is required for the Tile constructor
			map<int, shared_ptr<Path>> adjacent_paths;
			fill_adjacent_paths(adjacent_paths, adjacent_plots, position_coordinates);

			// Create the Tile
			Tile new_tile =
				Tile(chosen_type, chosen_number,
					tile_position_id, position_coordinates,
					adjacent_plots, adjacent_paths);

			// Add the tile to the proper set in the set of all board tiles.
			board_tiles[m.first].insert({ch, new_tile});

		}
	}

	// Now that all plots, tiles, and paths have been created and stored away,
	// we need to make sure all plots have a reference to plots adjacent to them

	// Loop through all plots
	for (shared_ptr<Plot> current_plot : board_plots) {
		// Here is a vector of the two offset relations between a plot and the plots adjacent to it which is always the same
		vector<OrderedPair> adjacency_offsets = {OrderedPair(0, 1), OrderedPair(0, -1)};

		// The final pair is one which may be positive or negative depending on position in the hex grid
		OrderedPair current_pos = current_plot->get_position_coordinates();
		OrderedPair final_pair = OrderedPair(-2, 0);
		OrderedPair start = OrderedPair(-1, 0);

		// Here, I am just taking a known valid offset and final pair, and walking it to the current position.
		// The final pair mirrors every time we take a step. Looking at a hex grid, you may find this to be a valid assessment.
		// X pos walk
		while (start.x != current_pos.x) {
			if (start.x < current_pos.x) {
				start.x += 2;
				final_pair = final_pair * -1;
			} else if (start.x > current_pos.x) {
				start.x -= 2;
				final_pair = final_pair * -1;
			}
		}
		// Y pos walk
		while (start.y != current_pos.y) {
			if (start.y < current_pos.y) {
				start.y += 1;
				final_pair = final_pair * -1;
			} else if (start.y > current_pos.y) {
				start.y -= 1;
				final_pair = final_pair * -1;
			}
		}

		adjacency_offsets.push_back(final_pair);

		// For each offset, if there is a plot there, it is adjacent so add it to the current plot's adjacent plots
		for (int i = 0; i < adjacency_offsets.size(); i++) {
			OrderedPair new_pos = current_plot->get_position_coordinates() + adjacency_offsets[i];
			if (has_plot(new_pos)) {
				shared_ptr<Plot> new_plot = get_plot(new_pos);
				current_plot->add_to_adjacent_plots(i, get_plot(new_pos));
			}
		}
	}

	// Now make sure all paths have reference to those that are adjacent to them

	// Loop through all paths
	for (shared_ptr<Path> current_path : board_paths) {
		// Get the paths endpoints in a vector to iterate over
		vector<shared_ptr<Plot>> iterable_endpoints =
			{current_path->get_endpoints().plot_a, current_path->get_endpoints().plot_b};
		// For each endpoint (there are 2 of them) look at all adjacent plots and the paths they form with it
		for (shared_ptr<Plot> plot : iterable_endpoints) {
			for (auto &it : plot->get_adjacent_plots()) {
				shared_ptr<Plot> adjacent_plot = it.second;
				if (!has_path(plot->get_position_coordinates(),
					adjacent_plot->get_position_coordinates())) {
					continue;
				}
				shared_ptr<Path> adjacent_path_candidate =
					get_path(plot->get_position_coordinates(),
						adjacent_plot->get_position_coordinates());
				if (current_path == adjacent_path_candidate) {
					continue;
				}
				current_path->add_to_adjacent_paths(adjacent_path_candidate);
			}
		}
	}
}

// Helper function for create_board fills out the adjacent plots
void Board::fill_adjacent_plots(
	map<int, shared_ptr<Plot>> &adjacent_plots,
	const OrderedPair &position_coordinates) {
	// These x, y values make up all offset coordinates for plot positions.
	// I am iterating over a vector to guarantee they are added in the order I want
	vector<OrderedPair> pairs =
		{OrderedPair(-1, 1), OrderedPair(1, 1),
		OrderedPair(1, 0), OrderedPair(1, -1),
		OrderedPair(-1, -1), OrderedPair(-1, 0)};
	for (int i = 0; i < pairs.size(); i++) {

		// Creates the coordinate for the plot, which is the offset from the current tile + the current tile's coordinate
		OrderedPair new_position_coordinate = pairs[i] + position_coordinates;
		if (has_plot(new_position_coordinate)) {
			// If there is already a plot with this coordinate, get it and add it to adjacent plots set
			adjacent_plots.insert({i, get_plot(new_position_coordinate)});
			// Adds the tile position to the plot's adjacent tile set
			get_plot(new_position_coordinate)->add_to_adjacent_tiles(position_coordinates);
		} else {

			// Get a port_pair for the plot. if it is the default, it will not make a change
			PortPair port_pair = get_corresponding_portpair(new_position_coordinate);

			// If there is not already a plot with this coordinate, create it
			auto new_plot = make_shared<Plot>(new_position_coordinate, port_pair.port_type, port_pair.universal_port);
			// Adds the tile position to the plot's adjacent tile set
			new_plot->add_to_adjacent_tiles(position_coordinates);
			// Add it to adjacent plots set
			adjacent_plots.insert({i, new_plot});
			// Add it to all plots set
			board_plots.insert(new_plot);
		}

	}
}

Board::PortPair Board::get_corresponding_portpair(const OrderedPair coord) const {
	for (const PortPair &p : ports) {
		if (p.plots.first == coord || p.plots.second == coord) {
			return p;
		}
	}
	return {};
}

void Board::populate_port_pairs() {
	srand(time(nullptr));
	vector<pair<OrderedPair, OrderedPair>> port_positions = {
		pair(OrderedPair(-1, 5), OrderedPair(1, 5)),
		pair(OrderedPair(3, 3), OrderedPair(3, 4)),
		pair(OrderedPair(5, 1), OrderedPair(5, 0)),
		pair(OrderedPair(5, -2), OrderedPair(5, -3)),
		pair(OrderedPair(1, -4), OrderedPair(3, -4)),
		pair(OrderedPair(-1, -4), OrderedPair(-3, -4)),
		pair(OrderedPair(-5, -2), OrderedPair(-5, -3)),
		pair(OrderedPair(-5, 0), OrderedPair(-5, 1)),
		pair(OrderedPair(-3, 3), OrderedPair(-3, 4)),};

	vector<pair<tile_type, bool>> port_types = {
		pair(TILE_EMPTY, true), pair(GRAIN, false), pair(ORE, false),
		pair(TILE_EMPTY, true), pair(WOOL, false), pair(TILE_EMPTY, true),
		pair(TILE_EMPTY, true), pair(BRICK, false), pair(LUMBER, false)};
	int rotation_value = rand() % port_types.size();
	rotate(port_types.begin(), port_types.begin() + rotation_value, port_types.end());

	for (int i = 0; i < port_positions.size(); i++) {
		ports.emplace_back(
			PortPair(
			port_positions.at(i).first,
			port_positions.at(i).second,
			port_types.at(i).first,
			port_types.at(i).second));
	}
}

// Helper function for create_board fills out adjacent paths
void Board::fill_adjacent_paths(
	map<int, shared_ptr<Path>> &adjacent_paths,
	const map<int, shared_ptr<Plot>> &adjacent_plots,
	const OrderedPair &position_coordinates) {

	vector<OrderedPair> pairs =
		{OrderedPair(-1, 1), OrderedPair(1, 1),
		OrderedPair(1, 0), OrderedPair(1, -1),
		OrderedPair(-1, -1), OrderedPair(-1, 0)};
	for (int a = 0, b = 1; a < pairs.size(); a++, b = (b + 1) % pairs.size()) {
		OrderedPair global_coords_a = pairs[a] + position_coordinates;
		OrderedPair global_coords_b = pairs[b] + position_coordinates;

		if (has_path(global_coords_a, global_coords_b)) {
			adjacent_paths.insert(
				{a, get_path(global_coords_a, global_coords_b)});
		} else {
			EndpointPair new_endpoints =
				EndpointPair(get_plot(global_coords_a), get_plot(global_coords_b));

			auto new_path = make_shared<Path>(new_endpoints, position_coordinates);

			adjacent_paths.insert({a, new_path});
			board_paths.insert(new_path);
		}
	}
}

// Returns an ordered pair depicted the position coordinates of a tile given its position_id
// Does so using mostly brute force unless I can come up with a clever way otherwise
OrderedPair Board::get_tile_position_coordinates(const string &position_id) const {
	if (position_id.length() != 3) {
		// The position ID is supposed to be in form #,# where # are both letters a-l
		return {};
	}

	if (position_id[0] == 'a') {
		// The only tile in ring 'a' is at position (0, 0)
		return {};
	}

	// Go through all 12 outer ring positions. Every other one is double an inner ring position, so if we
	// are in ring 'b', just take only every other one and take halve it.
	for (int i = 0; i < 12; i++) {
		// These are the outer ring positions in order clockwise starting at position_id c,a (c,a -> c,l)
		vector<OrderedPair> pairs = {OrderedPair(0, 4),
			OrderedPair(2, 3), OrderedPair(4, 2),
			OrderedPair(4, 0), OrderedPair(4, -2),
			OrderedPair(2, -3), OrderedPair(0, -4),
			OrderedPair(-2, -3), OrderedPair(-4, -2),
			OrderedPair(-4, 0), OrderedPair(-4, 2),
			OrderedPair(-2, 3)};
		string chars;

		switch (position_id[0]) {
			case 'b':
				// If we are in ring 'b', only look at every other value, as there are 6 instead of 12 tiles, and helve the
				// ordered pairs because they are half as fair from the origin (ring 'a')
				chars = "a_b_c_d_e_f_";
				if (chars[i] == '_') {
					continue;
				}
				if (position_id[2] == chars[i]) {
					return pairs[i] / 2.0;
				}
				break;
			case 'c':
				// If we are in ring 'c', look at all 12 letters and ordered pairs
				chars = "abcdefghijkl";
				if (position_id[2] == chars[i]) {
					return pairs[i];
				}
				break;
		}
	}

	return {};
}

// Displays the board to the console
void Board::display_board(
	const bool show_tile_position_ids,
	const vector<string> highlighted_tiles) const {

	// map of layers to print to the console labeled 0-20 because there are 21 layers to the board
	map<int, string> print_layers;

	// Populate map with all 21 layers of input.
	// A layer is one line, which on one tile might include two plots and a path, for example. See example drawing above for visual
	for (int i = 0; i < 21; i++) {
		vector<string> default_values = {"    ", "    ", "  ", "    "};
		// Default values represent the inherent zig zag of the sides of a hexagon.
		// All other spacing is handled within the tile printing
		print_layers.insert({i, default_values[i % default_values.size()]});
	}
	// Insert invalid index dump.
	// This layer will not be printed
	print_layers.insert({-1, ""});

	// Add initial spacing to parts of map that need it
	append_string_to_print_layer("  ", print_layers, 0, false);
	append_string_to_print_layer(" ", print_layers, 2, false);
	append_string_to_print_layer(" ", print_layers, 18, false);
	append_string_to_print_layer("          ", print_layers, 19, false);
	append_string_to_print_layer("             ", print_layers, 20, false);

	// Keep track of which paths have already been added to the print layers
	set<shared_ptr<Plot>> added_plots;
	set<shared_ptr<Path>> added_paths;

	// x is x-position, shift index holds the index of the shift vectors below. These values allow index to alternate to fit the hex grid
	for (int x = -4, shift_index = 0; x <= 4; x += 2, shift_index++) {
		// The value base y is the y position if looking at a square grid, but needs to be shifted to fit the alternating style of a hex grid.
		// Tile layer delta allows it to shift to account for all 21 print layers.
		for (int base_y = 4, tile_layer_delta = 0; base_y >= -4; base_y -= 2, tile_layer_delta += 4) {
			// local layer index 0-4 corresponds to the 5 layers of a tile's icons.
			for (int local_layer_index = 0; local_layer_index < 5; local_layer_index++) {

				vector<int> index_shifts = {0, 2}, y_val_shifts = {0, 1};
				int layer_index =
					local_layer_index + tile_layer_delta - index_shifts[shift_index % index_shifts.size()];
				if (layer_index < 0 || layer_index > 20) {
					// If the index is invalid, set it to go to the invalid index dump in the print_layers map
					layer_index = -1;
				}
				int y = base_y + y_val_shifts[shift_index % index_shifts.size()];

				// If the tile at the current position exists, we can begin to add it's qualities to the print layers
				if (has_tile(OrderedPair(x, y))) {

					// Current tile and its coordinates
					Tile current_tile = get_tile(OrderedPair(x, y));
					OrderedPair current_coords = current_tile.get_position_coordinate();

					bool lowlight_tile =
						!vector_has_value(
							highlighted_tiles,
							current_tile.get_position_id()) && highlighted_tiles.size() != 0;

					// Here, we go through the 5 layers of display for a tile, and add its paths, plots, or info bars in the correct order accordingly
					switch (local_layer_index) {
						case 0:
							// Adds the plot on the left to the layer
							append_plot_to_print_layer(
								current_coords + OrderedPair(-1, 1),
								print_layers, layer_index,
								added_plots, false);
							// Adds the path of direction 0 (-----) connected to previous point to the layer
							append_path_to_print_layer(
								get_plot(current_coords + OrderedPair(-1, 1)), 0,
								print_layers, layer_index, added_paths, false);
							// Adds the plot on the right to the layer
							append_plot_to_print_layer(
								current_coords + OrderedPair(1, 1),
								print_layers, layer_index,
								added_plots, false);
							break;
							// Resulting drawing: . ----- .
						case 1:
							// Adds the path on the left of the top info bar of the tile to the layer
							append_path_to_print_layer(
								get_plot(current_coords + OrderedPair(-1, 1)), 1,
								print_layers, layer_index, added_paths, false);
							// Adds the top layer info to layer
							append_string_to_print_layer(current_tile.get_top_display_row(
								show_tile_position_ids), print_layers,
								layer_index, false);
							// Adds the path on the right of the top info bar of the tile to the layer
							append_path_to_print_layer(
								get_plot(current_coords + OrderedPair(1, 1)), -1,
								print_layers, layer_index,
								added_paths, false);
							// resulting drawing example: /   b,c   \         .
							break;
						case 2:
							// Adds the plot on the left to the layer
							append_plot_to_print_layer(
								current_coords + OrderedPair(-1, 0),
								print_layers, layer_index,
								added_plots, false);
							// Adds the middle layer info to the layer
							append_string_to_print_layer(
								current_tile.get_main_display_row(lowlight_tile),
								print_layers, layer_index, false);
							// Adds the plot on the right to the layer
							append_plot_to_print_layer(
								current_coords + OrderedPair(1, 0),
								print_layers, layer_index,
								added_plots, false);
							break;
							// resulting drawing example: .   6 (L)   .
						case 3:
							// Adds the path on the left of the bottom info bar of the tile to the layer
							append_path_to_print_layer(
								get_plot(current_coords + OrderedPair(-1, 0)), -1,
								print_layers, layer_index,
								added_paths, false);
							// Adds the bottom layer info to the layer
							append_string_to_print_layer(
								current_tile.get_robber_display_row(),
								print_layers, layer_index, false);
							// Adds the path on the right of the bottom info bar of the tile to the layer
							append_path_to_print_layer(
								get_plot(current_coords + OrderedPair(1, 0)), 1,
								print_layers, layer_index, added_paths, false);
							break;
							// resulting drawing example: \  {!R!}  /
						case 4:
							// Adds the plot on the left to the layer
							append_plot_to_print_layer(
								current_coords + OrderedPair(-1, -1),
								print_layers, layer_index, added_plots, false);
							// Adds the path of direction 0 connected to previous point to the layer
							append_path_to_print_layer(
								get_plot(current_coords + OrderedPair(-1, -1)), 0,
								print_layers, layer_index, added_paths, false);
							// Adds the plot on the right to the layer
							append_plot_to_print_layer(
								current_coords + OrderedPair(1, -1),
								print_layers, layer_index,
								added_plots, false);
							break;
							// resulting drawing: . ----- .
					}
				} else {
					// This runs if there exists no tile at the given coordinates
					// The cases are for each lay of printing, which require different amounts of spacing
					switch (local_layer_index) {
						case 0:
							// If there is a tile above, we don't want to add spaces where that tile's icons would be
							if (has_tile(OrderedPair(x, y + 2))) { continue; }
							// Add appropriate number of spaces to layer
							append_string_to_print_layer(
								"       ", print_layers, layer_index, false);
							break;
						case 1:
							// Add appropriate number of spaces to layer
							append_string_to_print_layer(
								"          ", print_layers, layer_index, false);
							break;
						case 2:
							// Add appropriate number of spaces to layer
							append_string_to_print_layer(
								"           ", print_layers, layer_index, false);
							break;
						case 3:
							// Add appropriate number of spaces to layer
							append_string_to_print_layer(
								"          ", print_layers, layer_index, false);
							break;
						case 4:
							// If there is a tile bellow, we don't want to add spaces where that tile's icons would be
							if (has_tile(OrderedPair(x, y - 2))) { continue;}
							// Add appropriate number of spaces to layer
							append_string_to_print_layer(
								"       ", print_layers, layer_index, false);
							break;
					}
				}

			}
		}
	}

	// Prints all layers, first added a new line to avoid any conflicts with prints which didn't end in a newline
	cout << endl;
	for (auto &layer : print_layers) {
		if (layer.first != -1){ cout << layer.second << endl; }
	}
	cout << endl;
}

// Given an ordered pair, adds the plot at that location and data associated with it to the layer to be printed
void Board::append_plot_to_print_layer(
	const OrderedPair &global_plot_coords,
	map<int, string> &print_layers,
	const int layer_index,
	set<shared_ptr<Plot>> &added_plots,
	const bool space_after) const {

	// If the added plots already added this plot, don't try to add it again. Just return
	for (shared_ptr<Plot> plot : added_plots) {
		if (plot == get_plot(global_plot_coords)) { return; }
	}

	if (has_plot(global_plot_coords)) {
		shared_ptr<Plot> current_plot = get_plot(global_plot_coords);
		print_layers[layer_index] += current_plot->get_icon();
		if (space_after) {
			print_layers[layer_index] += " "; }
		added_plots.insert(current_plot);
	}
}

// Given an endpoint and direction, adds the corresponding path and its information to the layer to be printed
void Board::append_path_to_print_layer(
	const shared_ptr<Plot> &endpoint,
	const int &direction,
	map<int, string> &print_layers,
	const int layer_index,
	set<shared_ptr<Path>> &added_paths,
	const bool space_after) const {

	// If the added plots already added this plot, don't try to add it again. Just return
	for (shared_ptr<Path> path : added_paths) {
		if (path == get_path(endpoint, direction)) {
			return;
		}
	}

	if (has_path(endpoint, direction)) {
		shared_ptr<Path> current_path = get_path(endpoint, direction);
		print_layers[layer_index] += current_path->get_icon();
		if (space_after) {
			print_layers[layer_index] += " "; }
		added_paths.insert(current_path);
	} else {
		cout << endpoint->get_position_coordinates().x << "," <<
			endpoint->get_position_coordinates().y << " : " << direction << endl;
	}
}

// Adds plain string to layer to be printed
void Board::append_string_to_print_layer(
	const string &to_add,
	map<int, string> &print_layers,
	const int layer_index,
	const bool space_after) const {

	if (to_add.empty()) {
		// If the string is empty, don't bother doing anything else
		return; }
	print_layers[layer_index] += to_add;
	if (space_after) {
		print_layers[layer_index] += " "; }
}

// Returns a random tile type given a map of remaining tile types and how many of each kind are remaining
tile_type Board::get_random_tile_type(map<tile_type, int> &tile_types_remaining) {

	srand(time(nullptr));

	// Converts map of tile types and quantities into vector with the correct # of instances of each type in it for easier random picking
	vector<tile_type> tile_types_as_vector;
	for (auto it = tile_types_remaining.begin(); it != tile_types_remaining.end(); it++) {
		for (int i = 0; i < it->second; i++) {
			tile_types_as_vector.push_back(it->first);
		}
	}

	if (tile_types_as_vector.size() == 0) {
		return DESERT;
	}

	int chosen_index = rand() % tile_types_as_vector.size();
	tile_type chosen_type = tile_types_as_vector[chosen_index];
	tile_types_remaining[chosen_type] -= 1;

	return chosen_type;
}

// Returns a random production number given a vector of remaining production numbers
int Board::get_random_production_number(vector<int> &production_numbers_remaining) {

	srand(time(nullptr));

	if (production_numbers_remaining.size() == 0) {
		return -1;
	}

	int chosen_index = rand() % production_numbers_remaining.size();
	int chosen_number = production_numbers_remaining[chosen_index];
	production_numbers_remaining.erase(
		production_numbers_remaining.begin() + chosen_index);

	return chosen_number;
}

map<char, map<char, Tile>> Board::get_board_tiles() const {
	return board_tiles;
}

set<shared_ptr<Plot>> Board::get_board_plots() const {
	return board_plots;
}

set<shared_ptr<Path>> Board::get_board_paths() const {
	return board_paths;
}

bool Board::has_plot(const OrderedPair &position_coordinates) const {
	return get_plot(position_coordinates) != nullptr;
}

// Loops through the plots, and if it finds one with the given coords it returns it
shared_ptr<Plot> Board::get_plot(const OrderedPair &position_coordinates) const {
	for (shared_ptr<Plot> plot : board_plots) {
		if (plot->get_position_coordinates() == position_coordinates) {
			return plot;
		}
	}
	return nullptr;
}

bool Board::has_path(const EndpointPair &endpoints) const {
	return get_path(endpoints) != nullptr;
}

bool Board::has_path(const OrderedPair &endpointa, const OrderedPair &endpointb) const {
	EndpointPair plots = EndpointPair(get_plot(endpointa), get_plot(endpointb));
	return has_path(plots);
}

bool Board::has_path(const shared_ptr<Plot> &plot, const int direction) const {
	return get_path(plot, direction) != nullptr;
}

shared_ptr<Path> Board::get_path(const EndpointPair &endpoints) const {
	for (shared_ptr<Path> path : board_paths) {
		if (path->get_endpoints() == endpoints) {
			return path;
		}
	}
	return nullptr;
}

shared_ptr<Path> Board::get_path(const OrderedPair &endpointa, const OrderedPair &endpointb) const {
	EndpointPair plots = EndpointPair(get_plot(endpointa), get_plot(endpointb));
	return get_path(plots);
}

// Loops through paths, returning if it finds the path with the given direction that is connected to the given plot
shared_ptr<Path> Board::get_path(const shared_ptr<Plot> &plot, const int direction) const {
	for (shared_ptr<Path> path : board_paths) {
		if (path->get_direction() == direction && path->get_endpoints().has(plot)) {
			return path;
		}
	}
	return nullptr;
}

bool Board::has_tile(const OrderedPair &position_coordinates) const {
	return get_tile(position_coordinates).is_valid();
}

bool Board::has_tile(const string position_id) const {
	for (auto &ring : board_tiles) {
		for (auto &code_tile_pair : ring.second) {
			if (code_tile_pair.second.get_position_id() == position_id) {
				return true;
			}
		}
	}
	return false;
}

Tile Board::get_tile(const OrderedPair &position_coordinates) const {
	for (auto &ring : board_tiles) {
		for (auto &code_tile_pair : ring.second) {
			if (code_tile_pair.second.get_position_coordinate() == position_coordinates) {
				return code_tile_pair.second;
			}
		}
	}
	return {};
}

// Loops through board tiles to find tile with the given position coordinates
Tile& Board::get_tile(const OrderedPair &position_coordinates) {
	for (auto &ring : board_tiles) {
		for (auto &code_tile_pair : ring.second) {
			if (code_tile_pair.second.get_position_coordinate() == position_coordinates) {
				return code_tile_pair.second;
			}
		}
	}
	return invalid_tile;
}

// Loops through board tiles to find the tile with the given position coordinates
Tile& Board::get_tile(const string &position_id) {
	for (auto &ring : board_tiles) {
		for (auto &code_tile_pair : ring.second) {
			if (code_tile_pair.second.get_position_id() == position_id) {
				return code_tile_pair.second;
			}
		}
	}
	return invalid_tile;
}

bool Board::can_place_road(const shared_ptr<Player> player) const {
	player_color color = player->get_color();
	for (shared_ptr<Path> path : board_paths) {
		if (!path->is_occupied()) {
			// If the path is empty, we don't care.
			continue; }
		if (!path->get_inhabitant()->get_color() == color) {
			// If the path is not the right color, we don't care
			continue;}
		// At this point, we look at each path next to those that have a road owned by the player
		for (shared_ptr<Path> adj : path->get_adjacent_paths()) {
			if (!adj->is_occupied()) {
				// If there is an empty path adjacent to one with a road owned by the player, that is a legal placement. Return true
				return true;
			}
		}
	}
	// If we got here, we never returned true, so we have not found a spot for a road for this player.
	return false;
}

void Board::generate_resources(const int die_result) const {
	vector<string> highlighted_tiles;
	for (auto &ring_pair : board_tiles) {
		for (auto &tile_pair : ring_pair.second) {
			if (tile_pair.second.get_production_number() == die_result) {
				tile_pair.second.produce_resource();
				if (!tile_pair.second.has_robber()) {
					highlighted_tiles.push_back(tile_pair.second.get_position_id());
				}
			}
		}
	}
	display_board(false, highlighted_tiles);
}

Tile& Board::get_tile_with_robber() {
	for (auto &ring : board_tiles) {
		for (auto &tile_pair : ring.second) {
			if (tile_pair.second.has_robber()) {
				return tile_pair.second;
			}
		}
	}
	return invalid_tile;
}

void Board::set_robber_position(const string tile_id) {
	get_tile_with_robber().set_robber(false);
	get_tile(tile_id).set_robber(true);
}