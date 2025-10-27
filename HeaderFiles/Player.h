//
// Created by nmila on 9/9/2025.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <set>
#include <memory>
#include <map>
#include <vector>
#include "Enums.h"
#include "DevCard.h"
#include "OrderedPair.h"

class Board;
class Path;

/** FORWARD DEFINITION
* Requires: player, number
* Modifies: nothing
* Effects: prompts the user to discard the given number of resources
*/
void discard_resources_prompt(std::shared_ptr<Player> player, int number_of_resources);

/*
 * Player class stores all the info about a particular player
 */

class Player : public std::enable_shared_from_this<Player>{

    private:
        int ID;
        player_color color;

        bool longest_road;
        bool largest_army;

        std::vector<DevCard> devcards;
        std::map<tile_type, int> resource_cards;

        // Paths stored as endpoint positions to avoid circular include nonsense.
        std::pair<OrderedPair, OrderedPair> first_road_path;
        std::pair<OrderedPair, OrderedPair> second_road_path;

        bool played_devcard_this_turn;

        int number_of_knights;
        int length_of_road;

        int remaining_roads;
        int remaining_settlements;
        int remaining_cities;

        const int MAX_ROADS = 15;
        const int MAX_SETTLEMENTS = 5;
        const int MAX_CITIES = 4;

    public:

        /**
        * Requires: ID, color
        * Modifies: ID, color, longest_road, largest_army, player_devcard_this_turn, number_of_knights,
        * length_of_road, remaining_cities, remaining_settlements, remaining_roads
        * Effects: Constructor for Player. Sets ID and color. Defaults everything else. resets resources
        */
        Player(const int _ID, const player_color color);

        /**
        * Requires: nothing
        * Modifies: ID, color, longest_road, largest_army, player_devcard_this_turn, number_of_knights,
        * length_of_road, remaining_cities, remaining_settlements, remaining_roads
        * Effects: Constructor for Player. Defaults everything. resets resources
        */
        Player();

        /**
        * Requires: board, is_private
        * Modifies: nothing
        * Effects: gets points, and includes victory point devcards if is_private = true
        */
        int get_points(const Board &board, const bool is_private = true) const;

        /**
        * Requires: board, type
        * Modifies: nothing
        * Effects: returns number of buildings this player has on the board
        */
        int get_number_of_placed_building(const Board &board, const building_type type) const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns the number of victory point devcards in hand
        */
        int get_number_of_vp_devcards() const;

        /**
        * Requires: board
        * Modifies: nothing
        * Effects: returns a vector of the types of ports this player has
        */
        std::vector<tile_type> get_ports(const Board &board) const;

        /**
        * Requires: board
        * Modifies: nothing
        * Effects: returns if the player has a universal port
        */
        bool has_universal_port(const Board &board) const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns if has the longest road
        */
        bool has_longest_road() const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns if has the largest army
        */
        bool has_largest_army() const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns the length of road
        */
        int get_length_of_road() const;

        /**
        * Requires: board
        * Modifies: nothing
        * Effects: calculates and returns the length of the road
        */
        int calculate_length_of_road(const Board &board);

        /**
        * Requires: board
        * Modifies: nothing
        * Effects: returns the number of roads this player has on the board
        */
        int get_number_of_placed_roads(const Board &board) const;

        /**
        * Requires: path, excluded paths
        * Modifies: nothing
        * Effects: returns a set of valid paths (is occupied by a road of this players color)
        */
        std::set<std::shared_ptr<Path>> get_valid_adjacent_road_paths(
            const std::shared_ptr<Path> path,
            const std::set<std::shared_ptr<Path>> &exclude = {}) const;

        /**
        * Requires: start_path
        * Modifies: nothing
        * Effects: runs a pathfinding algorithm to return the length of the longest
        * route between all paths connected to start_path
        */
        int road_length_helper(const std::shared_ptr<Path> start_path);

        /**
        * Requires: vector, path
        * Modifies: nothing
        * Effects: returns if the vector has the path in it
        */
        bool vector_has_path(
            const std::vector<std::shared_ptr<Path>> &vec,
            const std::shared_ptr<Path> &path) const;

        /**
        * Requires: length
        * Modifies: length_of_road
        * Effects: sets length_of_road to length
        */
        void set_length_of_road(const int length, const bool print_on_longest = true);

        /**
        * Requires: endpoint_a, endpoint_b
        * Modifies: first_road_path/second_road_path
        * Effects: sets the first_road_path first, and second one second. Sets it to the path
        * between the provided points
        */
        void add_initial_road_anchor(
            const OrderedPair endpoint_a,
            const OrderedPair endpoint_b);

        /**
        * Requires: number
        * Modifies: nothing
        * Effects: returns the initial road path given 1 or 2 for first or second
        */
        std::pair<OrderedPair, OrderedPair> get_initial_road_path(const int number) const;

        /**
        * Requires: number
        * Modifies: nothing
        * Effects: returns the player's "name" (for example: "Player 1 (Red)")
        */
        std::string get_name() const;

        /**
        * Requires: number
        * Modifies: nothing
        * Effects: returns the color
        */
        player_color get_color() const;

        /**
        * Requires: _longest_road bool
        * Modifies: longest_road
        * Effects: sets longest_road to provided bool
        */
        void set_longest_road(const bool _longest_road);

        /**
        * Requires: _largest_army bool
        * Modifies: largest_army
        * Effects: sets largest_army to provided bool
        */
        void set_largest_army(const bool _largest_army);

        /**
        * Requires: nothing
        * Modifies: number_of_knights
        * Effects: increments number_of_knights
        */
        void gain_knight();

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns number_of_knights
        */
        int get_number_of_knights() const;

        /**
        * Requires: board, index
        * Modifies: devcards
        * Effects: plays devcard at index and removes it
        */
        void play_devcard(Board &board, const int index);

        /**
        * Requires: devcard
        * Modifies: devcards
        * Effects: adds devcard to list from given devcard type
        */
        void gain_devcard(const devcard_type devcard, const bool print = true);

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: sorts vector of devcards (bubble sort)
        */
        void sort_devcards();

        /**
        * Requires: devcard
        * Modifies: devcards
        * Effects: adds devcard to list from given devcard
        */
        void gain_devcard(const DevCard devcard, const bool print = true);

         /**
        * Requires: devcard
        * Modifies: nothing
        * Effects: returns if has given devcard
        */
        bool has_devcard(const devcard_type devcard) const;

        /**
        * Requires: played bool
        * Modifies: player_devcard_this_turn
        * Effects: sets played_devcard_this_turn to bool
        */
        void set_played_devcard_this_turn(const bool _played);

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns if has played a devcard this turn
        */
        bool has_played_devcard_this_turn() const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns vector of devcards in hand
        */
        std::vector<DevCard> get_devcards() const;

         /**
        * Requires: nothing
        * Modifies: elements of devcards
        * Effects: handles effects triggered from the start of a new turn
        * (updating devcards 'bought this turn')
        */
        void start_new_turn();

        /**
        * Requires: nothing
        * Modifies: resource_cards
        * Effects: sets resource_cards to default (0 of all)
        */
        void reset_resources();

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns a map of the default resources in hand (0 of all)
        */
        static std::map<tile_type, int> get_base_resource_map();

        /**
        * Requires: resource
        * Modifies: resource_cards
        * Effects: adds one of the provided resource to resource_cards
        */
        void gain_resource(const tile_type resource);

        /**
        * Requires: resources, from_bank, print (optional)
        * Modifies: resource_cards
        * Effects: adds all provided resources, prints about it, and caps at number remaining in
        * bank if from_bank = true. Prints stuff id print is true
        */
        void gain_many_resources(
            const std::map<tile_type,
            int> resources,
            const bool from_bank,
            const bool print = true);

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns resource_cards
        */
        std::map<tile_type, int> get_resources() const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns the number of resources the player has
        */
        int get_number_of_resources() const;

        /**
        * Requires: resource
        * Modifies: nothing
        * Effects: returns the number of the given resource the player has
        */
        int get_amount_of_resource(const tile_type resource) const;

        /**
        * Requires: resource
        * Modifies: resource_cards
        * Effects: removes one of the provided resource
        */
        void discard_resource(const tile_type resource);

        /**
        * Requires: resources, purpose, to_bank
        * Modifies: resource_cards
        * Effects: removes all provided resources, prints about it with provided verb, and
        * puts them in bank if to_bank = true
        */
        void discard_many_resources(
            const std::map<tile_type, int> resources,
            const std::string purpose,
            const bool to_bank,
            const bool print = true);

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns a random resource from hand
        */
        tile_type get_random_resource();

        /**
        * Requires: resource
        * Modifies: nothing
        * Effects: returns if player has the provided resource (> 0 of it)
        */
        bool has_resource(const tile_type resource) const;

        /**
        * Requires: resource
        * Modifies: nothing
        * Effects: returns if resource exists in players hand (is in map)
        */
        bool resource_exists(const tile_type resource) const;

        /**
        * Requires: recipe, print
        * Modifies: nothing
        * Effects: returns if the player can afford the provided
        * recipe and prints about it if print = true
        */
        bool can_afford(std::map<tile_type, int> recipe, const bool print) const;

        /**
        * Requires: color
        * Modifies: nothing
        * Effects: returns the color as a string
        */
        static std::string get_color_as_text(const player_color color);

        /**
        * Requires: change
        * Modifies: number_of_cities
        * Effects: changes number_of_cities by change
        */
        void incdec_city_pieces(const int change);

        /**
        * Requires: change
        * Modifies: number_of_roads
        * Effects: changes_number_of_roads by change
        */
        void incdec_road_pieces(const int change);

        /**
        * Requires: change
        * Modifies: number_of_settlements
        * Effects: changes number_of_settlements by change
        */
        void incdec_settlement_pieces(const int change);

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns number of city pieces in possession
        */
		int get_city_pieces() const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns number of road pieces in possession
        */
        int get_road_pieces() const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: returns number of settlement pieces in possession
        */
        int get_settlement_pieces() const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: handles resources when a seven is rolled, prompting player to
        * discard half of them rounded up if they have more than 7
        */
        void seven_rolled();

        /* Info Prints */
        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: prints player's resources
        */
        void print_resources() const;

        /**
        * Requires: nothing
        * Modifies: nothing
        * Effects: prints player's devcards
        */
        void print_devcards() const;

        /**
        * Requires: board, show_private (optional)
        * Modifies: nothing
        * Effects: prints all info, showing private info (vp card points, devcards, and resources)
        * only if show_private = true
        */
        void print_info(const Board &board, const bool show_private = false) const;
};

#endif //PLAYER_H
