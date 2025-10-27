//
// Created by nmila on 9/4/2025.
//

#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#include <memory>
#include <string>
#include <map>
#include "Craftable.h"
#include "Player.h"
#include "Enums.h"

/*
*
 * Construct is a type of craftable which can be placed on the board and belongs to one
 * of the 4 player colors.
 *
 * Types of this class include roads and buildings
 *
 * */

class Construct : public Craftable {

    protected:
        // represents the constructs color
        player_color color;

		// Represents the player who owns the construct
		std::shared_ptr<Player> owner;

    public:

		/**
		 * Requires: owner, recipe
		 * Modifies: owner, color
		 * Effects: Constructor for Construct, sets color and owner
		 */
        Construct(std::shared_ptr<Player> _owner, std::map<tile_type, int> _recipe);
		/**
		 * Requires: owner
		 * Modifies: owner, color
		 * Effects: Constructor for Construct, sets owner and sets color to COLOR_EMPTY.
		 */
		Construct(std::shared_ptr<Player> _owner);
		/**
		 * Requires: nothing
		 * Modifies: owner, color
		 * Effects: Constructor for Construct, sets owner to nullptr and sets color to COLOR_EMPTY.
		 */
		Construct();

        /* Getters */
		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: Returns the color of the construct
		 */
        player_color get_color() const;

		// To be overriden by subclasses
		// Returns the icon which represents the building, which may be overriden with the provided string parameter
		/**
		 * Requires: icon_override (optional)
		 * Modifies: nothing
		 * Effects: returns the icon for the construct, which is different based on child classes.
		 * It can also be override by icon_override
		 */
		virtual std::string get_icon(std::string icon_override = "") const;

		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: Returns owner
		 */
		std::shared_ptr<Player> get_owner() const;

};

#endif //CONSTRUCT_H
