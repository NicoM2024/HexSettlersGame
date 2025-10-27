//
// Created by nmila on 9/4/2025.
//

#ifndef CRAFTABLE_H
#define CRAFTABLE_H

#include <map>
#include <string>
#include "Enums.h"

/*
*
 * The Craftable class handles anything that can be crafted in game,
 * storing a crafting cost which varies from object to object
 *
 * Costs:
 * - Road: 				1 Lumber, 1 Brick
 * - Settlement: 		1 Lumber, 1 Brick, 1 Wool, 1 Grain
 * - City: 				2 Grain,  3 Ore
 * - Development Card: 	1 Grain,  1 Wool,  1 Ore
 * */

class Craftable{

    private:
        std::map<tile_type, int> recipe;

	public:
		/**
		 * Requires: recipe
		 * Modifies: recipe
		 * Effects: Constructor for Craftable, sets recipe
		 */
		Craftable(std::map<tile_type, int> _recipe);

		/**
		 * Requires: nothing
		 * Modifies: recipe
		 * Effects: Constructor for Construct, sets recipe to default {}
		 */
		Craftable();

		/** STATIC
		 * Requires: string representation of thing (settlement, city, road, development card)
		 * Modifies: nothing
		 * Effects: returns the recipe of the given item
		 */
		static std::map<tile_type, int> get_recipe(std::string craftable_thing);

		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: returns the recipe of the current craftable
		 */
		virtual std::map<tile_type, int> get_recipe() const;

		/**
		 * Requires: recipe
		 * Modifies: recipe
		 * Effects: sets the recipe to the provided recipe
		 */
		void set_recipe(std::map<tile_type, int> _recipe);
};

#endif //CRAFTABLE_H
