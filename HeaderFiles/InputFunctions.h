//
// Created by nmila on 9/9/2025.
//

#ifndef INPUTFUNCTIONS_H
#define INPUTFUNCTIONS_H

#endif //INPUTFUNCTIONS_H

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "Enums.h"
#include "Plot.h"
#include "OrderedPair.h"
#include "Board.h"

class Player;

/**
* Requires: string
* Modifies: string
* Effects: returns the lowercase version of the string
*/
std::string get_lowercase(std::string s);

/**
* Requires: string
* Modifies: string
* Effects: returns capitalized version of string (hello -> Hello)
*/
std::string get_capitalized(std::string s);

/**
* Requires: vector and string value
* Modifies: nothing
* Effects: returns if the vector has the string
*/
bool vector_has_value(std::vector<std::string> vec, std::string val);

/**
* Requires: vector and int value
* Modifies: nothing
* Effects: returns if the vector has the int
*/
bool vector_has_value(std::vector<int> vec, int val);

/**
* Requires: include color (optional)
* Modifies: nothing
* Effects: returns the map of tile types to strings and includes color codes if include_color = true
*/
std::map<tile_type, std::string> get_resource_conversion_to_string(
	const bool include_color = true);

/**
* Requires: main_message, empty_message, invalid_message
* Modifies: nothing
* Effects: returns int given by user after input validation
*/
int generic_int_prompt(
	const std::string main_message,
	const std::string empty_message,
	const std::string invalid_message);

/**
* Requires: player, can_quit
* Modifies: nothing
* Effects: returns the string tile_id given by user after input validation
*/
std::string tile_id_prompt(
	const std::shared_ptr<Player> &player,
	const bool can_quit = true);

/**
* Requires: board, player, tile_id, require_adjacent_road, can_quit
* Modifies: nothing
* Effects: returns the plot at the chosen index from the chosen tile (only valid settlement locations)
*/
std::shared_ptr<Plot> placement_plot_prompt(
	Board &board,
	const std::shared_ptr<Player> &player,
	const std::string tile_id,
	const bool require_adjacent_road = true,
	const bool can_quit = true);

/**
* Requires: board, player, tile_id, can_quit
* Modifies: nothing
* Effects: returns the plot at the chosen index from the chosen tile (only on settlements)
*/
std::shared_ptr<Plot> placement_upgrade_prompt(
	Board &board,
	const std::shared_ptr<Player> &player,
	const std::string tile_id,
	const bool can_quit = true);

/**
* Requires: board, tile_id, require_adjacent_road, can_quit
* Modifies: nothing
* Effects: returns the path at the chosen index from the chosen tile
*/
std::shared_ptr<Path> placement_path_prompt(
	const Board &board,
	const std::string tile_id,
	const bool requires_adjacent_road = true,
	const bool can_quit = true);

/**
* Requires: board, coordinate, can_quit
* Modifies: nothing
* Effects: returns the path at the chosen index surrounding chosen plot
*/
std::shared_ptr<Path> placement_path_prompt(
	const Board &board,
	const OrderedPair building_position,
	const bool can_quit = true);

/**
* Requires: nothing
* Modifies: nothing
* Effects: returns number of players chosen by player
*/
int player_count_prompt();

/**
* Requires: board, player, steal_resource
* Modifies: nothing
* Effects: prompts user to move the robber to chosen tile
*/
void robber_movement_prompt(
	Board &board, std::shared_ptr<Player> &player, const bool steal_resource);

/**
* Requires: board, player, tile_id
* Modifies: nothing
* Effects: prompts the user to choose a player adjacent to given tile to steal from
*/
void adjacent_resource_theft_prompt(
	Board &board, std::shared_ptr<Player> &player, const std::string tile_id);

/**
* Requires: player, number
* Modifies: nothing
* Effects: prompts the user to discard the given number of resources
*/
void discard_resources_prompt(std::shared_ptr<Player> player, int number_of_resources);

/**
* Requires: player, consider_bank, consider_hand, can_quit, excluded resources, main_message_override
* Modifies: nothing
* Effects: prompts the user to choose a resource, restricted by a variety of factors
*/
tile_type resource_choice_prompt(
	const std::shared_ptr<Player> &player,
	const bool consider_remaining_bank_resources,
	const bool consider_resources_in_hand,
	const bool can_quit,
	const std::vector<tile_type> &exclusions,
	const std::string main_message_override);

/**
* Requires: board, player
* Modifies: nothing
* Effects: prompts the player to choose something to craft
*/
void craft_something_prompt(Board &board, const std::shared_ptr<Player> &player);

/**
* Requires: board, player, players
* Modifies: nothing
* Effects: prompts the player to choose a type of trading
*/
void trade_prompt(
	const Board &board,
	const std::shared_ptr<Player> &subject_player,
	const std::map<int,std::shared_ptr<Player>> &players);

/**
* Requires: player, ports, universal_port?
* Modifies: nothing
* Effects: prompts the user to choose a resource to trade with the bank
*/
void maritime_trade(const std::shared_ptr<Player> &subject_player,
	const std::vector<tile_type> &typed_ports = {},
	const bool with_universal_port = false);

/**
* Requires: player, payment, gain
* Modifies: payment, gain
* Effects: prompts the user to choose how to modify the current trade
*/
void modify_trade_prompt(
	const std::shared_ptr<Player> &proposer,
	std::map<tile_type, int> &payment,
	std::map<tile_type, int> &gain);

/**
* Requires: side, change, target_aspect, resource, amount
* Modifies: side
* Effects: changes the side of the trade
*/
void modify_trade_side(
	std::map<tile_type, int> &side,
	const std::string change,
	const std::string target_aspect,
	const tile_type resource,
	const int amount);

/**
* Requires: proposer, target, payments
* Modifies: nothing
* Effects: prompts the players to confirm the trade
*/
bool trade_confirm_prompt(
	const std::shared_ptr<Player> &proposer,
	const std::shared_ptr<Player> &target_player,
	const std::map<tile_type, int> &proposers_payment,
	const std::map<tile_type, int> &targets_payment);

/**
* Requires: proposer, target, payment, gain
* Modifies: nothing
* Effects: prints the trade's info
*/
void display_trade(
	const std::shared_ptr<Player> &proposer,
	const std::shared_ptr<Player> &target_player,
	const std::map<tile_type, int> &payment,
	const std::map<tile_type, int> &gain);

/**
* Requires: payment
* Modifies: nothing
* Effects: returns if the side of the trade is valid
*/
bool valid_trade_side(
	const std::map<tile_type, int> &payment);

/**
* Requires: board, player,
* Modifies: nothing
* Effects: prompts the player to choose how they want their info
*/
void request_player_info_prompt(
	const Board &board,
	const std::shared_ptr<Player> &subject_player,
	const std::map<int, std::shared_ptr<Player>> &players);

/**
* Requires: board, player, players
* Modifies: nothing
* Effects: prompts the user to request info
*/
void request_info_prompt(
	const Board &board,
	const std::shared_ptr<Player> &player,
	const std::map<int, std::shared_ptr<Player>> &players);

/**
* Requires: board
* Modifies: nothing
* Effects: prompts the player to request info about the board
*/
void request_board_info_prompt(const Board &board);

/**
* Requires: board, player, require_adjacent_road, can_quit, spend_resources
* Modifies: nothing
* Effects: prompts the player to place a building
*/
OrderedPair place_building(
	Board &board,
	const std::shared_ptr<Player> &player,
	const bool require_adjacent_road = true,
	const bool can_quit = true,
	const bool spend_resources = true);

/**
* Requires: board, player, can_quit
* Modifies: nothing
* Effects: prompts the player to upgrade a building
*/
OrderedPair upgrade_building(
	Board &board,
	const std::shared_ptr<Player> &player,
	const bool can_quit);

/**
* Requires: board, player, requires_adjacent_road, can_quit, spend_resources
* Modifies: nothing
* Effects: prompts the player to place a road
*/
void place_road(
	Board &board,
	const std::shared_ptr<Player> &player,
	const bool requires_adjacent_road,
	const bool can_quit,
	const bool spend_resources);

/**
* Requires: board, player, building_position
* Modifies: nothing
* Effects: prompts the player to place their initial roads
*/
void initial_road_placement(
	const Board &board,
	const std::shared_ptr<Player> &player,
	const OrderedPair building_position);

/**
* Requires: board, player, can_quit
* Modifies: nothing
* Effects: prompts the player to play a devcard
*/
void play_devcard(
	Board &board,
	const std::shared_ptr<Player> &player,
	const bool can_quit);

/**
* Requires: board, player, gain_adjacent_resources
* Modifies: nothing
* Effects: Handles player board setup
*/
void initial_player_setup(
	Board &board,
	const std::shared_ptr<Player> &player,
	const bool gain_adjacent_resources);

/**
* Requires: nothing
* Modifies: nothing
* Effects: prints a help message
*/
void help_message();

/**
 * Requires: nothing
 * Modifies: nothing
 * Effects: prints welcome message
 */
void welcome_message();

/**
* Requires: nothing
* Modifies: nothing
* Effects: prints a message showing the recipes for craftables
*/
void recipe_message();
