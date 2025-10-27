//
// Created by nmila on 9/20/2025.
//

#include "../HeaderFiles/GameBank.h"
#include "../HeaderFiles/InputFunctions.h"
#include "../HeaderFiles/DevCard.h"

#include <iostream>

using std::cout, std::endl;
using std::shared_ptr, std::make_shared, std::vector, std::map, std::string, std::min;

GameBank& GameBank::instance(const int player_count_override) {
    static GameBank inst = GameBank(player_count_override);
    return inst;
}

void GameBank::reset() {
    instance().players.clear();
    instance().available_devcards.clear();
    instance().available_resources.clear();

    instance().available_resources = {
        {GRAIN, 19},
        {WOOL, 19},
        {BRICK, 19},
        {ORE, 19},
        {LUMBER, 19}};
    instance().setup_players(instance().players, instance().player_count, false);
    instance().setup_devcard_deck();
}

GameBank::GameBank(const int _player_count) : player_count(_player_count),
base_devcards(
    {{KNIGHT, 14},
        {ROAD_BUILDING, 2},
        {MONOPOLY, 2},
        {YEAR_OF_PLENTY, 2},
        {VICTORY_POINT, 5}}),
available_resources({
    {GRAIN, 19},
    {WOOL, 19},
    {BRICK, 19},
    {ORE, 19},
    {LUMBER, 19}}){

    bool print_player_setup = false;
    if (player_count == 0) {
        player_count = player_count_prompt();
        print_player_setup = true;
    }
    setup_players(players, player_count, print_player_setup);

    setup_devcard_deck();
}

DevCard GameBank::draw_devcard(){
    if (has_devcard()) {
        int index = rand() % available_devcards.size();
        devcard_type type = available_devcards.at(index);
        available_devcards.erase(available_devcards.begin() + index);
        return DevCard(type);
    }
    return {};
}

bool GameBank::has_devcard() const {
    return !available_devcards.empty();
}

void GameBank::setup_devcard_deck() {
    for (auto &it : base_devcards){
        for (int i = 0; i < it.second; i++) {
            available_devcards.push_back(it.first);
        }
    }
}

void GameBank::return_resource(const tile_type _type) {
    if (!resource_exists(_type)) {
        return;
    }
    available_resources[_type] = min(available_resources.at(_type) + 1, 19);

}
void GameBank::draw_resource(const tile_type _type){
    if (!has_resource(_type)) {
        return;
    }
    available_resources[_type] -= 1;
}

bool GameBank::has_resource(const tile_type _type) const{
    if (!resource_exists(_type)) {
        return false;
    }
    return available_resources.at(_type) > 0;
}

bool GameBank::resource_exists(const tile_type _type) const{
    for (auto &it : available_resources) {
        if (it.first == _type) {
            return true;
        }
    }
    return false;
}

map<tile_type, int> GameBank::get_available_resources() const {
    return available_resources;
}

map<int, shared_ptr<Player>> GameBank::get_players() const {
    return players;
}

shared_ptr<Player> GameBank::get_player(string color) const{
    // Conversion from string to color
    map<string, player_color> conversion =
        {{"red", RED}, {"yellow", YELLOW},
        {"green", GREEN}, {"blue", BLUE}};
    color = get_lowercase(color);
    // If the enetered color is valid or not
    bool valid_color = false;
    for (string valid : {"red", "yellow", "green", "blue"}) {
        if (color == valid) {
            valid_color = true;
        }
    }
    if (!valid_color) {
        return nullptr;
    }
    // Actuall retrieval
    for (auto &player : players) {
        if (player.second->get_color() == conversion.at(color)) {
            return player.second;
        }
    }
    return nullptr;
}

shared_ptr<Player> GameBank::get_player(const player_color color) const{
    for (auto &player : players) {
        if (player.second->get_color() == color) {
            return player.second;
        }
    }
    return nullptr;
}

shared_ptr<Player> GameBank::get_player(const int player_index) const {
    // Manually search incase an invalid index is provided
    for (auto &player : players) {
        if (player.first == player_index) {
            return player.second;
        }
    }
    return nullptr;
}

shared_ptr<Player> GameBank::get_longest_road_player() const {
    for (auto &p : players) {
        if (p.second->has_longest_road()) {
            return p.second;
        }
    }
    return nullptr;
}

shared_ptr<Player> GameBank::get_largest_army_player() const {
    for (auto &p : players) {
        if (p.second->has_largest_army()) {
            return p.second;
        }
    }
    return nullptr;
}

void GameBank::setup_players(map<int, shared_ptr<Player>> &player_map, const int player_count, const bool print){
    for (int i = 0; i < player_count; i++) {
        vector<player_color> colors = {RED, YELLOW, GREEN, BLUE};
        shared_ptr<Player> new_player = make_shared<Player>(i + 1, colors[i]);
        player_map.insert({i + 1, new_player});
        if (print) {
            cout << "Player " << i + 1 << " will be " << Player::get_color_as_text(colors[i]) << "." << endl;
        }
    }
}

std::vector<devcard_type> GameBank::get_available_devcards() const {
    return available_devcards;
}