//
// Created by nmila on 9/20/2025.
//

#ifndef GAMEBANK_H
#define GAMEBANK_H

#include <map>
#include <vector>
#include <memory>
#include "Enums.h"
#include "Player.h"

/** FORWARD DEFINITION
* Requires: nothing
* Modifies: nothing
* Effects: returns number of players chosen by player
*/
int player_count_prompt();

class DevCard;

class GameBank {
	private:
    	std::map<tile_type, int> available_resources;
        const std::map<devcard_type, int> base_devcards;
        std::vector<devcard_type> available_devcards;

		std::map<int, std::shared_ptr<Player>> players;
		int player_count = 0;

		const int MAX_RESOURCE_COUNT = 19;

		/**
		 * Requires: player_count (optional)
		 * Modifies: player_count, base_devcards, available_resources
		 * Effects: Constructor for GameBank, sets player_count and card decks to default decks
		 */
		GameBank(const int _player_count = 0);

    public:

		/**
		 * Requires: player_count_override (optional)
		 * Modifies: internal static gamebank object
		 * Effects: Creates a gamebank static to this function. There can only be one gamebank.
		 * This returns that static gamebank if it is already a thing.
		 */
		static GameBank& instance(const int player_count_override = 0);

		/**
		 * Requires: nothing
		 * Modifies: internal static gamebank object
		 * Effects: Resets gamebank for testing.
		 */
		static void reset();

		/**
		 * Requires: nothing
		 * Modifies: available_devcards
		 * Effects: Returns a random devcard from the deck and removes it
		 */
        DevCard draw_devcard();

		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: returns if the deck has any devcards
		 */
        bool has_devcard() const;

		/**
		 * Requires: nothing
		 * Modifies: available_devcards
		 * Effects: adds all devcards to the vector deck of devcards
		 */
        void setup_devcard_deck();

		/**
		 * Requires: type
		 * Modifies: available_resources
		 * Effects: puts a resource back into the piles
		 */
        void return_resource(const tile_type _type);

		/**
		 * Requires: type
		 * Modifies: available_resources
		 * Effects: removes a resource from the piles
		 */
        void draw_resource(const tile_type _type);

		/**
		 * Requires: type
		 * Modifies: nothing
		 * Effects: returns if there are any resources of the given type left
		 */
        bool has_resource(const tile_type _type) const;

		/**
		 * Requires: type
		 * Modifies: nothing
		 * Effects: Returns if any resources of the given type exist
		 */
        bool resource_exists(const tile_type _type) const;

		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: returns players
		 */
		std::map<int, std::shared_ptr<Player>> get_players() const;

		/**
		 * Requires: color string
		 * Modifies: nothing
		 * Effects: returns player with color from given string
		 */
		std::shared_ptr<Player> get_player(std::string color) const;

		/**
		 * Requires: color
		 * Modifies: nothing
		 * Effects: returns player with given color
		 */
		std::shared_ptr<Player> get_player(const player_color color) const;

		/**
		 * Requires: index
		 * Modifies: nothing
		 * Effects: returns player with given index
		 */
		std::shared_ptr<Player> get_player(const int player_index) const;

		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: returns player with longest road
		 */
		std::shared_ptr<Player> get_longest_road_player() const;

		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: returns player with largest army
		 */
		std::shared_ptr<Player> get_largest_army_player() const;

		/**
		 * Requires: players, player_count, whether it should print
		 * Modifies: players
		 * Effects: creates players and puts them in map
		 */
		void setup_players(std::map<int, std::shared_ptr<Player>> &player_map, const int player_count, const bool print = true);

		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: returns remaining available resources
		 */
		std::map<tile_type, int> get_available_resources() const;

		/**
		 * Requires: nothing
		 * Modifies: nothing
		 * Effects: returns remaining available devcards
		 */
		std::vector<devcard_type> get_available_devcards() const;
};

#endif //GAMEBANK_H
