//
// Created by nmila on 9/4/2025.
//

#ifndef PLOT_H
#define PLOT_H

#include <string>
#include <memory>
#include <map>
#include <set>
#include "Building.h"
#include "OrderedPair.h"

/*
*
 * The Plot class represents a spot where a building can be placed on the board, and stores
 * whatever building object is in its spot, if any
 *
 * */

class Plot{
    private:
		// position in coordinates on the hexagonal grid
		OrderedPair position_coordinates;
        // Points to the Building which is in its position
        std::unique_ptr<Building> inhabitant;
		// Contains the override icon, mostly used when displaying the plots as selectable locations for buildings
		std::string icon_override;
		// Map of adjacent plots
		std::map<int, std::shared_ptr<Plot>> adjacent_plots;
		// Set of adjacent tile positions
		std::set<OrderedPair> adjacent_tile_coords;

		tile_type port_type = TILE_EMPTY;
		bool universal_port;

    public:
		/**
		* Requires: position coordinate
		* Modifies: position_coordinate
		* Effects: Constructor for Plot. Sets position coordinate
		*/
        Plot(const OrderedPair _position_coordinates);

		/**
		* Requires: position coordinate, port type, is universal port
		* Modifies: position_coordinate, port_type, universal_port
		* Effects: Constructor for Plot. Sets position coordinate, port type, and if it has a universal port or not
		*/
		Plot(const OrderedPair _position_coordinates, const tile_type _port_type, const bool _universal_port);

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: Constructor for Plot (=default). Sets all fields to default values
		*/
		Plot();

		/* Getters */

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns position_coordinates
		*/
		OrderedPair get_position_coordinates() const;

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns icon based on if there is a building on the plot or not, and
		* based on the icon override
		*/
		std::string get_icon() const;

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns the icon override
		*/
		std::string get_icon_override() const;

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns the building on this, if any
		*/
		Building* get_inhabitant() const;

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns if the plot is occupied
		*/
		bool is_occupied() const;

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns the port type of the plot
		*/
		tile_type get_port_type() const;

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns if the plot has a universal port
		*/
		bool is_universal_port() const;

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns a set of all tile coordinates adjacent to the plot
		*/
		std::set<OrderedPair> get_adjacent_tile_coords() const;

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns if the plot is adjacent to one with a building on it
		*/
		bool next_to_building() const;

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns the set of adjacent plots
		*/
		std::map<int, std::shared_ptr<Plot>> get_adjacent_plots() const;

        /* Setters */
		/**
		* Requires: new building
		* Modifies: inhabitant
		* Effects: sets the inhabitant to the provided building
		*/
        void set_inhabitant(Building new_inhabitant);

		/**
		* Requires: icon_override string
		* Modifies: icon_override
		* Effects: sets icon override to provided string
		*/
		void set_icon_override(const std::string _icon_override);

		/**
		* Requires: key, plot
		* Modifies: adjacent_plots
		* Effects: adds plot to adjacent plots with given key
		*/
		void add_to_adjacent_plots(const int key, const std::shared_ptr<Plot> new_plot);

		/**
		* Requires: coords
		* Modifies: adjacent_tile_coords
		* Effects: adds tile at coordinates to adjacent tile coords
		*/
		void add_to_adjacent_tiles(const OrderedPair coords);

};

#endif //PLOT_H
