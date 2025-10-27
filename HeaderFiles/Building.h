//
// Created by nmila on 9/4/2025.
//

#ifndef BUILDING_H
#define BUILDING_H

#include <memory>
#include <map>
#include <string>

#include "Construct.h"
#include "Enums.h"


/*
*
 * The Building class is a type of construct which can be placed in plots on the map and stores
 * logic for providing its owner wth resources when they are produced
 *
 * */

class Player;

class Building : public Construct {

    private:
        // amount of resources produced from any given collection when an adjacent tile's number is rolled
        // Settlements = 1; Cities = 2
        int resource_production;

		// Type of the building
		building_type type;

    public:

		/**
		 * Requires: player owner, building type
		 * Modifies: type, resource_production, recipe
		 * Effects: Constructor for Building. Sets player owner. Sets production number
		 * to 1 or 2 for Settlement or City. Also sets recipe based on type of building
		 */
        Building(const std::shared_ptr<Player> _owner, const building_type _type);

		/**
		 * Requires: building type
		 * Modifies: type, resource_production
		 * Effects: Constructor for Building. Sets owner to nullptr. Sets production number
		 * to 1 or 2 for Settlement or City. Also sets recipe based on type of building
		 */
		Building(const building_type _type);

		/**
		 * Requires: nothing
		 * Modifies: type, resource_production
		 * Effects: Constructor for Building. sets owner to nullptr. Sets production number
		 * to 0. This is default and creates an invalid building
		 */
		Building();

		/* Getters */
		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: returns the type of building
		 */
		building_type get_building_type() const;

		/**
		 * Requires: icon_override (optional)
		 * Modifies: nothing
		 * Effects: Returns icon for building, which may be override by provided string
		 */
	    std::string get_icon(std::string icon_override = "") const override;

		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: Returns recipe which is different based on building type
		 */
		std::map<tile_type, int> get_recipe() const override;

		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: returns the production based on if its a settlement or city
		 */
		int get_resource_production() const;

		/**
		 * Requires: nothing
		 * Modifies: resource_production, type
		 * Effects: converts building to city if it was a settlement
		 */
		void upgrade();

};
#endif //BUILDING_H
