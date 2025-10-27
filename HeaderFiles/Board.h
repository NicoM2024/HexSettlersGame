//
// Created by nmilazz on 9/4/2025.
//

#ifndef BOARD_H
#define BOARD_H

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <set>

#include "Tile.h"
#include "Enums.h"
#include "Plot.h"
#include "Path.h"

class Player;

class Board {

	// Some information on map syntax came from this website:
	// https://www.geeksforgeeks.org/cpp/map-associative-containers-the-c-standard-template-library-stl/

	// Some information on random number generation came from this website:
	// https://www.w3schools.com/cpp/cpp_howto_random_number.asp

    private:

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
        * There are 19 Tiles, 54 Plots and 72 Paths.
        *
        *                               O ----- O
        *                              /         \
        *                     O ----- O   11(L)   O ----- O
        *                    /         \         /         \
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
        *
        * The map will store 3 maps, each containing all of their tiles
        *
         */
        std::map<char, std::map<char, Tile>> board_tiles;

		// These sets store all the plots and paths on the map
		std::set<std::shared_ptr<Plot>> board_plots;
		std::set<std::shared_ptr<Path>> board_paths;

		// List of the available numbers to choose from for board creation at the start
		const std::vector<int> available_production_numbers = {2, 3, 3, 4, 4, 5, 5, 6, 6, 8, 8, 9, 9, 10, 10, 11, 11, 12};

		// List of the available tile types to choose from for board creation (each number represents how many of the tile there can be)
	    const std::map<tile_type, int> available_tile_types = {
	    	{GRAIN, 4},
	    	{WOOL, 4},
	    	{LUMBER, 4},
	    	{BRICK, 3},
	    	{ORE, 3},
	    	{DESERT, 1}};

		Tile invalid_tile;

		struct PortPair {
			tile_type port_type;
			bool universal_port;
			std::pair<OrderedPair, OrderedPair> plots;

			PortPair(const OrderedPair plota, const OrderedPair plotb, const tile_type _port_type, const bool _universal_port) {
				universal_port = _universal_port;
				plots.first = plota;
				plots.second = plotb;
				port_type = _port_type;
			}
			PortPair(const OrderedPair plota, const OrderedPair plotb) : universal_port(false), port_type(TILE_EMPTY) {
				plots.first = plota;
				plots.second = plotb;
			}
			PortPair() : universal_port(false), port_type(TILE_EMPTY) {}
		};

		std::vector<PortPair> ports;

    public:

		/**
		 * Requires: Nothing
		 * Modifies: board_tiles
		 * Effects: Calls create_board
		 */
		Board();

		/**
		 * Requires: map of available tiles types, vector of available production numbers
		 * Modifies: board_tiles
		 * Effects: Creates the board and populates board_tiles with tiles
		 */
        void create_board(
        	std::map<tile_type, int> _available_tile_types, std::vector<int> _available_production_numbers);

		/* Helper functions for create_board function */

		/**
		 * Requires: map of adjacent plots, position coordinates
		 * Modifies: board_plots, tiles in board_tiles
		 * Effects: fills the board_plots field with the plots on the board
		 * and fills the tiles with their adjacent plots
		 */
		void fill_adjacent_plots(
			std::map<int, std::shared_ptr<Plot>> &adjacent_plots,
			const OrderedPair &position_coordinates);

		/**
		 * Requires: coordinate
		 * Modifies: nothing
		 * Effects: returns the PortPair at the coordinate
		 */
		PortPair get_corresponding_portpair(const OrderedPair coord) const;

		/**
		 * Requires: nothing
		 * Modifies: ports
		 * Effects: Fills the ports vector with PortPairs
		 */
		void populate_port_pairs();

		/**
		 * Requires: adjacent_paths, adjacent_plots, coordinate
		 * Modifies: board_paths, tiles in board_tiles
		 * Effects: fills the board_paths field with the paths on the board
		 * and fills the tiles with their adjacent paths
		 */
		void fill_adjacent_paths(
			std::map<int, std::shared_ptr<Path>>
			&adjacent_paths,
			const std::map<int,
			std::shared_ptr<Plot>> &adjacent_plots,
			const OrderedPair &position_coordinates);

		/**
		 * Requires: bool to show the tiles' ids, vector of tile_id strings of tiles to highlight
		 * Modifies: nothing
		 * Effects: Prints the board formatted properly
		 */
        void display_board(
        	const bool show_tile_position_ids = false,
        	const std::vector<std::string> highlighted_tiles = {}) const;

		/* Helper functions for display_board function */
		// Each of these add some string to the print layers for displaying the board
		// The path and plot functions do so while also making sure the path or plot hasn't been added already
		/**
		 * Requires: coordinate, print_layers, layer_index, plots already added, whether there should be a space after
		 * Modifies: print_layers, added_plots
		 * Effects: Add plot's icon to the print layer
		 */
		void append_plot_to_print_layer(
			const OrderedPair &global_plot_coords,
			std::map<int, std::string> &print_layers,
			const int layer_index,
			std::set<std::shared_ptr<Plot>> &added_plots,
			const bool space_after) const;

		/**
		 * Requires: endpoint, direction, print_layers, layer_index, plots already added, whether there should be a space after
		 * Modifies: print_layers, added_paths
		 * Effects: Add path's icon to the print layer
		 */
		void append_path_to_print_layer(
			const std::shared_ptr<Plot> &endpoint,
			const int &direction,
			std::map<int, std::string> &print_layers,
			const int layer_index,
			std::set<std::shared_ptr<Path>> &added_paths,
			const bool space_after) const;

		/**
		 * Requires: strint to add, print_layers, layer_index, whether there should be a space after
		 * Modifies: print_layers
		 * Effects: Adds the provided string to the given print layer
		 */
		void append_string_to_print_layer(
			const std::string &to_add,
			std::map<int, std::string> &print_layers,
			const int layer_index,
			const bool space_after) const;


		/**
		 * Requires: nothing
		 * Modifies: ports
		 * Effects: Fills the ports vector with PortPairs
		 */
		// provided the available tile types, returns a random one and removes it from the list of available ones
	    tile_type get_random_tile_type(std::map<tile_type, int> &tile_types_remaining);

		/**
		 * Requires: remaining production numbers
		 * Modifies: remaining production numbers
		 * Effects: returns a random number from production_numbers_remaining
		 */
		//provided the available production numbers, returns a random one and removes it from the list of available ones
		int get_random_production_number(std::vector<int> &production_numbers_remaining);

		/* Getters */

		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: returns board_tiles
		 */
		std::map<char, std::map<char, Tile>> get_board_tiles() const;

		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: returns board_plots
		 */
		std::set<std::shared_ptr<Plot>> get_board_plots() const;

		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: returns board_paths
		 */
		std::set<std::shared_ptr<Path>> get_board_paths() const;

		/**
		 * Requires: coordinate
		 * Modifies: nothing
		 * Effects: returns plot at the given coordinate, if any
		 */
		// Returns the plot from the set of all plots that has the provided coordinates
		std::shared_ptr<Plot> get_plot(const OrderedPair &position_coordinates) const;


		/**
		 * Requires: endpoints
		 * Modifies: nothing
		 * Effects: returns path between given endpoints, if any
		 */
		// Returns the path between the given endpoints, or between the given two local coordinates, or with a given direction attached to the given plot.
		std::shared_ptr<Path> get_path(const EndpointPair &endpoints) const;

		/**
		 * Requires: 2 coordinates
		 * Modifies: nothing
		 * Effects: returns plot between given coordinates, if any
		 */
		std::shared_ptr<Path> get_path(const OrderedPair &endpointa, const OrderedPair &endpointb) const;

		/**
		 * Requires: plot and direction
		 * Modifies: nothing
		 * Effects: returns path connected to given plot extending in given direction, if any
		 */
		std::shared_ptr<Path> get_path(const std::shared_ptr<Plot> &plot, const int direction) const;

		// Returns the tile at the given coordinates or position_id

		/**
		 * Requires: coordinate
		 * Modifies: nothing
		 * Effects: returns reference to tile at coordinate
		 */
		Tile& get_tile(const OrderedPair &position_coordinates);

		/**
		 * Requires: coordinate
		 * Modifies: nothing
		 * Effects: returns value of tile at coordinate
		 */
		Tile get_tile(const OrderedPair &position_coordinates) const;

		/**
		 * Requires: position_id string
		 * Modifies: nothing
		 * Effects: returns reference to Tile with the given ID string
		 */
		Tile& get_tile(const std::string &position_id);

		// Returns whether there is a spot the provided player can place a road or not.

		/**
		 * Requires: player
		 * Modifies: nothing
		 * Effects: returns whether the player can place a road or not
		 */
		bool can_place_road(const std::shared_ptr<Player> player) const;

		// Will return the position coordinate of a tile given its position ID

		/**
		 * Requires: position_id string
		 * Modifies: nothing
		 * Effects: returns coordinate of tile with position_id
		 */
		OrderedPair get_tile_position_coordinates(const std::string &position_id) const;

		// Returns whether there is a plot at the given coordinates or not
		/**
		 * Requires: coordinate
		 * Modifies: nothing
		 * Effects: returns whether there is a plot at the coordinate or not
		 */
		bool has_plot(const OrderedPair &position_coordinates) const;

		// Returns whether there is a path between the given endpoints,
		// or between the given two local coordinates, or with a given plot and direction or not.
		/**
		 * Requires: endpoints
		 * Modifies: nothing
		 * Effects: returns whether there is a path between the given endpoints or not
		 */
		bool has_path(const EndpointPair &endpoints) const;

		/**
		 * Requires: 2 coordinates
		 * Modifies: nothing
		 * Effects: returns whether there is a path between the given coordinates or not
		 */
		bool has_path(const OrderedPair &endpointa, const OrderedPair &endpointb) const;

		/**
		 * Requires: plot and direction
		 * Modifies: nothing
		 * Effects: returns whether there is a path connected to the given plot and
		 * extending in the given direction or not.
		 */
		bool has_path(const std::shared_ptr<Plot> &plot, const int direction) const;

		// Returns whether there is a tile at the given global coordinates or not

		/**
		 * Requires: coordinate
		 * Modifies: nothing
		 * Effects: returns whether there is a tile at the given coordinate or not
		 */
		bool has_tile(const OrderedPair &position_coordinates) const;
		// returns whether there is a tile with the given position_id or not

		/**
		 * Requires: position_id string
		 * Modifies: nothing
		 * Effects: returns whethere there is a tile with the given ID string or not
		 */
		bool has_tile(const std::string position_id) const;

		/**
		 * Requires: die roll result (production number)
		 * Modifies: nothing
		 * Effects: prompts all tiles with the given production number to generate resources.
		 */
		void generate_resources(const int die_result) const;

		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: Returns a reference to the tile with the robber on it
		 */
		Tile& get_tile_with_robber();

		/**
		 * Requires: position_id string
		 * Modifies: tile at given position_id string, current tile with robber
		 * Effects: sets has_robber of given tile to true, and current robber tile to false
		 */
		void set_robber_position(const std::string tile_id);

};

#endif //BOARD_H
