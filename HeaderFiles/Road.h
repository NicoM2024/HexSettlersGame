//
// Created by nmila on 9/4/2025.
//

#ifndef ROAD_H
#define ROAD_H

#include <map>
#include <string>
#include <memory>
#include "Construct.h"
#include "Enums.h"

/*
*
 * The Road class is a type of construct which can be placed in paths on the board and
 * handles logic for:
 *  - getting its full length (includes all connected road, finds the longest, continuous road),
 *   - determining if buildings can be placed
 * 		(except for in the first two rounds of the game, buildings may only be placed connected to an owners road
 *
 * */

class Player;

class Road : public Construct{

    public:

		/**
		 * Requires: owner
		 * Modifies: nothing
		 * Effects: Constructor for Road. Creates road with given owner and color set to owner's color
		 */
        Road(const std::shared_ptr<Player> _owner);

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: Constructor for Road. Creates road with owner as nullptr and color to COLOR_EMPTY
		*/
		Road();

        /* Getters */

		/**
		* Requires: nothing
		* Modifies: nothing
		* Effects: returns the recipe for a road
		*/
		std::map<tile_type, int> get_recipe() const override;

		/**
		* Requires: icon_override (optional)
		* Modifies: nothing
		* Effects: returns icon which may be overriden by provided string
		*/
	    std::string get_icon(std::string icon_override = "") const override;

};

#endif //ROAD_H
