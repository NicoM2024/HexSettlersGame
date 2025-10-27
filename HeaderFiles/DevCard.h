//
// Created by nmila on 9/9/2025.
//

#ifndef DEVCARD_H
#define DEVCARD_H

#include <memory>
#include <string>
#include <map>
#include <vector>

#include "Craftable.h"
#include "Enums.h"

class Player;
class Board;

/** FORWARD DEFINITION
* Requires: player, consider_bank, consider_hand, can_quit, excluded resources, main_message_override
* Modifies: nothing
* Effects: prompts the user to choose a resource, restricted by a variety of factors
*/
tile_type resource_choice_prompt(
    const std::shared_ptr<Player> &player,
    const bool consider_remaining_bank_resources,
    const bool consider_resources_in_hand,
    const bool can_quit = false,
    const std::vector<tile_type> &exclusions = {},
    const std::string main_message_override = "");

/** FORWARD DEFINITION
* Requires: board, player, requires_adjacent_road, can_quit, spend_resources
* Modifies: nothing
* Effects: prompts the player to place a road
*/
void place_road(
    Board &board,
    const std::shared_ptr<Player> &player,
    const bool requires_adjacent_road = true,
    const bool can_quit = true,
    const bool spend_resources = true);

/** FORWARD DEFINITION
* Requires: board, player, steal_resource
* Modifies: nothing
* Effects: prompts user to move the robber to chosen tile
*/
void robber_movement_prompt(
    const Board &board,
    std::shared_ptr<Player> &player,
    const bool steal_resource);

class DevCard : public Craftable {
    private:
        devcard_type type;

        bool bought_this_turn;

    public:
        /**
         * Requires: type
         * Modifies: type, bought_this_turn
         * Effects: Constructor for DevCard, sets type and sets btt to true
         */
        DevCard(const devcard_type _type);

        /**
         * Requires: nothing
         * Modifies: type, bought_this_turn
         * Effects: Constructor for DevCard, sets type to CARD_EMPTY and sets btt to true.
         * CREATES INVALID CARD
         */
        DevCard();

        /**
         * Requires: board, player
         * Modifies: depends on effect...
         * Effects: performs card effect based on type.
         */
        void play(Board &board, std::shared_ptr<Player> player);

        /**
         * Requires: board, player
         * Modifies: nothing
         * Effects: returns if the card is playable and prints why it isn't if it isn't
         */
        bool test_playability(Board &board, std::shared_ptr<Player> player);

        /**
         * Requires: nothing
         * Modifies: nothing
         * Effects: returns if the card is valid (type != CARD_EMPTY)
         */
        bool is_valid() const;

        /**
         * Requires: bought_this_turn
         * Modifies: bought_this_turn
         * Effects: sets bought_this_turn
         */
        void set_bought_this_turn(const bool btt);

        /**
         * Requires: nothing
         * Modifies: nothing
         * Effects: returns if it was bought this turn
         */
        bool was_bought_this_turn() const;

        /**
         * Requires: nothing
         * Modifies: nothing
         * Effects: returns type
         */
        devcard_type get_type() const;

        /**
         * Requires: nothing
         * Modifies: nothing
         * Effects: returns recipe
         */
        std::map<tile_type, int> get_recipe() const override;

        /** STATIC
         * Requires: type
         * Modifies: nothing
         * Effects: returns description for given card type
         */
        static std::string get_description(const devcard_type _type);

        /**
         * Requires: nothing
         * Modifies: nothing
         * Effects: returns description
         */
        std::string get_description() const;

        /**
         * Requires: nothing
         * Modifies: nothing
         * Effects: returns name
         */
        std::string get_name() const;

        /**
         * Requires: 2 devcards
         * Modifies: nothing
         * Effects: returns whether the first card is <, >, <=, >=, ==, and != to the second based
         * on the value of the type. (enum -> int)
         */
        friend bool operator<(const DevCard &d1, const DevCard &d2);
        friend bool operator>(const DevCard &d1, const DevCard &d2);
        friend bool operator<=(const DevCard &d1, const DevCard &d2);
        friend bool operator>=(const DevCard &d1, const DevCard &d2);
        friend bool operator==(const DevCard &d1, const DevCard &d2);
        friend bool operator!=(const DevCard &d1, const DevCard &d2);

};

#endif //DEVCARD_H
