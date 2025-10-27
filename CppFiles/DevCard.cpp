//
// Created by nmila on 9/9/2025.
//

#include "../HeaderFiles/DevCard.h"
#include "../HeaderFiles/Player.h"
#include "../HeaderFiles/Board.h"
#include "../HeaderFiles/InputFunctions.h"
#include "../HeaderFiles/GameBank.h"

#include <iostream>

using std::cout, std::endl;
using std::map, std::shared_ptr, std::string, std::vector;

map<tile_type, int> DevCard::get_recipe() const {
    return {{GRAIN, 1}, {WOOL, 1}, {ORE, 1}};
}

DevCard::DevCard(const devcard_type _type) : Craftable(get_recipe()) {
    type = _type;
    bought_this_turn = true;
}

DevCard::DevCard() : Craftable(get_recipe()) {
    type = CARD_EMPTY;
    bought_this_turn = true;
}

bool DevCard::test_playability(Board &board, shared_ptr<Player> player) {
    if (player->has_played_devcard_this_turn()) {
        cout << player->get_name() << ", you can only play one Development Card per turn. " << endl;
        false;
    }

    if (bought_this_turn) {
        cout << "You may not play a Development Card on the same turn it was crafted. " << endl;
        return false;
    }
    switch (type) {
        case KNIGHT:
            return true;
        case ROAD_BUILDING:
            if (!board.can_place_road(player)) {
                cout << player->get_name() << ", there are currently no places for you to place a road." << endl;
                return false;
            }
            return true;
        case YEAR_OF_PLENTY: {
            map<tile_type, string> valid_resources = get_resource_conversion_to_string();
            for (auto &resource : GameBank::instance().get_available_resources()) {
                if (resource.second <= 0) {
                    valid_resources.erase(resource.first);
                }
            }
            // If there are no resources to choose from, return
            if (valid_resources.empty()) {
                cout << "There are currently no resources remaining to choose from." << endl;
                return false; }
            return true;
        }
        case MONOPOLY:
            return true;
        case VICTORY_POINT:
            cout << "Victory Point cards are not to be played." << endl;
            cout << "Their score is automatically added to your total as long as they are in your hand." << endl;
            return false;
    }
    return false;
}

void DevCard::play(Board &board, shared_ptr<Player> player) {
    switch (type) {
        case KNIGHT:
            player->gain_knight();
            robber_movement_prompt(board, player, true);
            return;
        case ROAD_BUILDING:
            if (player->get_road_pieces() <= 0) {
                cout << player->get_name() << ", you are out of road pieces. This card has no effect and is discarded." << endl;
                return;
            }
            for (int i = 0; i < 2; i++) {
                if (player->get_road_pieces() <= 0) {
                    cout << player->get_name() << ", you are out of road pieces and therefore are unable to place any more roads. " << endl;
                    return;
                }
                if (!board.can_place_road(player)) {
                    cout << player->get_name() << ", there is currently no place for you to place a road. " << endl;
                    return;
                }
                vector<string> numbers = {"first", "second"};
                cout << player->get_name() << ", placing " << numbers[i] << " road..." << endl;
                place_road(board, player, true, false, false);
            }
            return;
        case YEAR_OF_PLENTY: {
            // Prompt for resource 1
            tile_type resource_1 = resource_choice_prompt(player, true, false);
            if (resource_1 == TILE_EMPTY) {
                // If there are NO resources left at all, return
                return;
            }
            // prompt for resource 2
            tile_type resource_2 = resource_choice_prompt(player, true, false);
            if (resource_1 == resource_2) {
                // If they are the same, give them both to the player all at once
                player->gain_many_resources({{resource_1, 2}}, true);
                return;
            }
            if (resource_2 == TILE_EMPTY) {
                // If resource 2 is TILE_EMPTY, meaning there was only one resource left at the time of playing this devcard, just gain resource 1
                player->gain_many_resources({{resource_1, 1}}, true);
                return;
            }
            // Otherwise, just gain both resources choses as normal
            player->gain_many_resources({{resource_1, 1}, {resource_2, 1}}, true);
            return;
            }
        case MONOPOLY: {
            tile_type resource = resource_choice_prompt(player, false, false);
            int transfer_count = 0;
            for (auto &player_pair : GameBank::instance().get_players()) {
                int local_count = 0;
                if (player_pair.second == player) {
                    continue;
                }
                if (!player_pair.second->has_resource(resource)) {
                    continue;
                }
                local_count += player_pair.second->get_resources().at(resource);
                transfer_count += local_count;
                player_pair.second->discard_many_resources({{resource, local_count}}, "lost", false);
            }

            int previous_count = player->get_number_of_resources();
            player->gain_many_resources({{resource, transfer_count}}, false);
            if (previous_count == player->get_number_of_resources()) {
                cout << "Sorry, nobody had any " << get_resource_conversion_to_string(true).at(resource) << " to steal.\n" << endl;
            }
            return;
        }
    }
}

bool DevCard::is_valid() const {
    return type != CARD_EMPTY;
}

string DevCard::get_description(const devcard_type _type) {
    switch (_type) {
        case KNIGHT:
            return "Relocate the robber to a new tile, and steal a random resource from a player who owns a building adjacent to it.\nThis also adds a knight to your army.";
        case ROAD_BUILDING:
            return "Place two roads without paying the required resources.";
        case YEAR_OF_PLENTY:
            return "Gain two resources of your choice from the supply.";
        case MONOPOLY:
            return "Choose a resource. Steal all of that resource from all other players.";
        case VICTORY_POINT:
            return "1 Victory Point";
    }
    return "...none...";
}

devcard_type DevCard::get_type() const {
    return type;
}

void DevCard::set_bought_this_turn(const bool btt) {
    bought_this_turn = btt;
}

bool DevCard::was_bought_this_turn() const {
    return bought_this_turn;
}

string DevCard::get_description() const {
    return get_description(type);
}

string DevCard::get_name() const {
    string name = "";
    switch (type) {
        case KNIGHT:
            name += "Knight";
            break;
        case ROAD_BUILDING:
            name += "Road Building";
            break;
        case YEAR_OF_PLENTY:
            name += "Year Of Plenty";
            break;
        case MONOPOLY:
            name += "Monopoly";
            break;
        case VICTORY_POINT:
            name += "1 Victory Point!";
            break;
        case CARD_EMPTY:
            name += "...";
    }
    if (bought_this_turn) {
        name += " - (Crafted this turn)";
    }
    return name;
}

bool operator<(const DevCard &d1, const DevCard &d2) { return d1.type < d2.type; }
bool operator>(const DevCard &d1, const DevCard &d2) { return d1.type > d2.type; }
bool operator<=(const DevCard &d1, const DevCard &d2) { return (d1 < d2) || (d1 == d2); }
bool operator>=(const DevCard &d1, const DevCard &d2) { return (d1 > d2) || (d1 == d2); }
bool operator==(const DevCard &d1, const DevCard &d2) { return d1.type == d2.type; }
bool operator!=(const DevCard &d1, const DevCard &d2) { return !(d1 == d2); }