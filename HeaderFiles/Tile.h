//
// Created by nmila on 9/4/2025.
//

#ifndef TILE_H
#define TILE_H

#include <string>
#include <map>
#include <memory>
#include "OrderedPair.h"
#include "Enums.h"
#include "Plot.h"
#include "Path.h"

/*
 * The Tile class store all information about one hex tile on the game board, including:
 * - a set of adjacent plots and paths,
 * - the number required for resource production,
 * - the type of tile/resource to be produced,
 * - the id which represents the tile's position on the board, used to access the tile
 *
 */

class Board;

class Tile{

    private:

        // The number which must be rolled for this tile to produce its resource
        int production_number;
        // The id value to acccess this tile through the board (represents position on the board in polar)
        std::string position_id;

        // Number position in the strange hexagonal grid
        OrderedPair position_coordinate;

        // Whether the tile has the robber or not
        bool robber;

        // The type of the tile
        tile_type type;

        // Sets which contain adjacent plots and paths (multiple tiles' sets may intersect)
        std::map<int, std::shared_ptr<Plot>> adjacent_plots;
        std::map<int, std::shared_ptr<Path>> adjacent_paths;

    public:

        /**
        * Requires: type, production_number, position_id, position_coordinate, set of adjacent plots and paths
        * Modifies: type, robber, production_number, position_id, position_coordinate, adjacent_plots, adjacent_paths
        * Effects: Constructor for Tile. Creates tile with provided parameters set as values for fields
        */
        Tile(
            const tile_type _type,
            const int _production_number,
            const std::string _position_id,
            const OrderedPair _position_coordinate,
            const std::map<int, std::shared_ptr<Plot>> _adjacent_plots,
            const std::map<int, std::shared_ptr<Path>> _adjacent_paths);

        /**
        * Requires: nothing
        * Modifies: type, robber, production_number, position_id, position_coordinate, adjacent_plots, adjacent_paths
        * Effects: Constructor for Tile. sets type to TILE_EMPTY and production number to 0. Everything else is default value.
        * Creates INVALID TILE
        */
        Tile();

        /* Getters */
        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns position_id string
        */
        std::string get_position_id() const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns position_coordinates
        */
        OrderedPair get_position_coordinate() const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns type
        */
        tile_type get_type() const;

        /**
        * Requires: whether the row should be faint or not (optional)
        * Modifies: nothing
        * Effects: returns main display row
        */
        std::string get_main_display_row(const bool lowlight_row = false) const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns the bottom display row
        */
        std::string get_robber_display_row() const;

        /**
        * Requires: whether the position_id string should be shown (optional)
        * Modifies: nothing
        * Effects: returns the top display row
        */
        std::string get_top_display_row(const bool display_position_id = false) const;

         /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns the production number
        */
        int get_production_number() const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns if the tile has the robber
        */
        bool has_robber() const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns the set of adjacent plots
        */
        const std::map<int, std::shared_ptr<Plot>> get_adjacent_plots() const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns the set of adjacent paths
        */
        const std::map<int, std::shared_ptr<Path>> get_adjacent_paths() const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns if the tile is valid (not TILE_TYPE == TILE_EMPTY)
        */
        bool is_valid() const;

        /**
        * Requires: type
        * Modifies: nothing
        * Effects: returns the type of tile as a string
        */
        static std::string type_to_string(const tile_type _type);

        /* Setters */
        /**
        * Requires: robber (yes or no)
        * Modifies: robber
        * Effects: sets whether there is a robber to provided bool
        */
        void set_robber(const bool _robber);

        /**
        * Requires: print (optional)
        * Modifies: nothing
        * Effects: provides resource to players adjacent to tile based on their production numbers from
        * their buildings and the tile type of this. If print = true, prints stuff
        */
        void produce_resource(const bool print = true) const;

        /**
        * Requires: plot
        * Modifies: adjacent_plots
        * Effects: adds plot to adjacent plots
        */
        void add_to_adjacent_plots(const int key, std::shared_ptr<Plot> plot);

        /**
        * Requires: path
        * Modifies: adjacent_paths
        * Effects: adds path to adjacent paths
        */
        void add_to_adjacent_paths(const int key, std::shared_ptr<Path> path);

        /**
        * Requires: board, color, whether it needs an adjacent road to be valid
        * Modifies: nothing
        * Effects: sets the override icon on the adjacent plots to X
        * if its invalid choice, and its index if it is valid
        */
        void override_plot_icons(
            const Board &board,
            const player_color color,
            const bool require_adjacent_road = true);

        /**
        * Requires: color
        * Modifies: nothing
        * Effects: sets the override icon on the adjacent plots to their index if they are
        * settlements
        */
        void override_settlement_icons(const player_color _color);

        /**
        * Requires: color, whether it needs an adjacent road to be valid
        * Modifies: nothing
        * Effects: sets the override icon on the adjacent paths to their index if they are valid choices
        */
        void override_path_icons(
            const player_color color,
            const bool require_adjacent_road = true);

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: sets all plot icon overrides to "", making them display their normal icon
        */

        void reset_plot_icon_overrides();

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: sets all path icon override to "", making them display their normal icon
        */
        void reset_path_icon_overrides();
};

#endif //TILE_H
