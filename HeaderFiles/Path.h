//
// Created by nmila on 9/5/2025.
//

#ifndef PATH_H
#define PATH_H


#include <set>
#include <string>
#include <memory>
#include "Enums.h"
#include "Road.h"
#include "OrderedPair.h"
#include "EndpointPair.h"

/*
*
 * The path class represents any location where a road may be placed on the board, and
 * stores a reference to the road it holds, if any
 *
 * */

class Path : public std::enable_shared_from_this<Path>{
    private:
        // Points to the Road which this path contains
        std::unique_ptr<Road> inhabitant;
		// Holds the direction of path (-1 for angled down, 1 for angled up, 0 for flat)
		int direction;
		// endpoints Endpoint pair contains the two plots that act as endpoints for this path
		EndpointPair endpoints;

		std::string icon_override;

		std::set<std::shared_ptr<Path>> adjacent_paths;

    public:
		/**
		* Requires: endpoints, anchor_tile coordinates
		* Modifies: endpoints, direction
		* Effects: Constructor for Path, sets endpoints, sets direction to calculated direction
		*/
        Path(const EndpointPair _endpoints, const OrderedPair anchor_tile_coords);

		/**
		* Requires: nothing
		* Modifies: endpoints, direction
		* Effects: Constructor for OrderedPair. sets endpoints to default value,
		* sets direction to 0
		*/
		Path();

        /* Getters */

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns endpoints
		*/
		EndpointPair get_endpoints() const;

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns icon based on the direction (/, \, or -----),
		* or based on if there is a road on it
		*/
        std::string get_icon() const;

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns road, if there is one
		*/
        Road* get_inhabitant() const;

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns if the path is occupied by a road or not
		*/
        bool is_occupied() const;

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns the direction (-1, 0, or 1)
		*/
        int get_direction() const;

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns the icon_override for the paths icon
		*/
		std::string get_icon_override() const;

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns the set of adjacent paths
		*/
		std::set<std::shared_ptr<Path>> get_adjacent_paths() const;

		/**
		* Requires: color
		* Modifies: nothing
		* Effects: returns if there is a road adjacent to it which is of the provided color
		*/
		bool has_adjacent_matching_road(const player_color color) const;

		// Seperate function to calculate direction and store in direction field, as the direction never changes and the
		// function is a tad heavy. This is called once in the constructor to initialize the field 'direction'
		// It doesn't matter which adjacent tile the anchor_tile is because a path only has one direction no matter how you calculate it
		/**
		* Requires: 2 endpoint coordinates, anchor tile coordinate
		* Modifies: nothing
		* Effects: returns the direction based on the slop between the two coordinates. Slope calculation
		* slightly modified to work with hexagonal grid
		*/
		int calculate_direction(
			OrderedPair endpointa,
			OrderedPair endpointb,
			const OrderedPair anchor_tile_coords) const;


        /* Setters */
		/**
		* Requires: icon_override
		* Modifies: icon_override
		* Effects: sets the icon_override
		*/
		void set_icon_override(const std::string _icon_override);

		/**
		* Requires: Road
		* Modifies: inhabitant
		* Effects: sets the inhabitant to the provided one
		*/
        void set_inhabitant(Road new_inhabitant);

		/**
		* Requires: path
		* Modifies: adjacent_paths
		* Effects: adds path to adjacent paths
		*/
		void add_to_adjacent_paths(std::shared_ptr<Path> &new_path);

};

#endif //PATH_H
