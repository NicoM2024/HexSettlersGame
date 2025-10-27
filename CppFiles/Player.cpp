//
// Created by nmila on 9/9/2025.
//

#include "../HeaderFiles/Player.h"
#include "../HeaderFiles/Board.h"
#include "../HeaderFiles/InputFunctions.h"
#include "../HeaderFiles/Path.h"
#include "../HeaderFiles/GameBank.h"

#include <cmath>

#include <iostream>

using std::cout, std::endl;
using std::max, std::min, std::ceil, std::swap;
using std::shared_ptr, std::vector, std::set, std::pair, std::string, std::map, std::find;

Player::Player(const int _ID, const player_color _color) {
    ID = _ID;
    color = _color;

    reset_resources();

    longest_road = false;
    largest_army = false;
    played_devcard_this_turn = false;

    number_of_knights = 0;
    length_of_road = 0;

    remaining_cities = MAX_CITIES;
    remaining_roads = MAX_ROADS;
    remaining_settlements = MAX_SETTLEMENTS;
}

Player::Player() {
    ID = -1;
    color = COLOR_EMPTY;

    reset_resources();

    longest_road = false;
    largest_army = false;
    played_devcard_this_turn = false;

    number_of_knights = 0;
    length_of_road = 0;

    remaining_cities = 0;
    remaining_roads = 0;
    remaining_settlements = 0;
}

// determines the number of points the player has and returns it
int Player::get_points(const Board &board, const bool is_private) const{
    int total_points = 0;

    /* Points from buildings */
    total_points += get_number_of_placed_building(board, SETTLEMENT);
    if (total_points > 0) {
    }
    total_points += 2 * get_number_of_placed_building(board, CITY);

    /* Points from cards */
    if (is_private) {
        total_points += get_number_of_vp_devcards();
    }

    if (has_longest_road()){
        total_points += 2;
    }

    if (has_largest_army()){
        total_points += 2;
    }
    return total_points;
}

int Player::get_number_of_placed_building(const Board &board, const building_type type) const {
    int num = 0;
    for (const shared_ptr<Plot> &plot : board.get_board_plots()){
        // if the plot is not relavent to this player, continue
        if (!plot->is_occupied()){ continue; }
        if (plot->get_inhabitant()->get_color() != color){ continue; }

        if (plot->get_inhabitant()->get_building_type() == type){
            num += 1;
        }
    }
    return num;
}

int Player::get_number_of_vp_devcards() const {
    int num = 0;
    for (const DevCard &card : devcards) {
        if (card.get_type() == VICTORY_POINT) {
            num += 1;
        }
    }
    return num;
}

vector<tile_type> Player::get_ports(const Board &board) const {
    vector<tile_type> ports;
    for (const shared_ptr<Plot> &plot : board.get_board_plots()) {
        if (!plot->is_occupied()) {
            continue;
        }
        if (plot->get_inhabitant()->get_color() != color) {
            continue;
        }
        if (plot->get_port_type() == TILE_EMPTY) {
            continue;
        }
        bool has_it = false;
        for (tile_type t : ports) {
            if (plot->get_port_type() == t) {
                has_it = true;
            }
        }
        if (!has_it) {
            ports.push_back(plot->get_port_type());
        }
    }
    return ports;
}

bool Player::has_universal_port(const Board &board) const {
    for (const shared_ptr<Plot> &plot : board.get_board_plots()) {
        if (plot->is_occupied() && plot->get_inhabitant()->get_color() == color) {
            if (plot->is_universal_port()) {
                return true;
            }
        }
    }
    return false;
}

// determines whether the player has the longest road or largest army or not
bool Player::has_longest_road() const{
    return longest_road;
}

int Player::get_length_of_road() const {
    return length_of_road;
}

int Player::calculate_length_of_road(const Board &board) {
    int first_length = 0;
    int second_length = 0;

    // Calculate the lengths but only if the initial road is not the default value. If it is, it will probably cause an error...
    if (!(first_road_path.first == OrderedPair() && first_road_path.second == OrderedPair())) {
        shared_ptr<Path> start_path = board.get_path(first_road_path.first, first_road_path.second);
        first_length = road_length_helper(start_path);
    }
    if (!(second_road_path.first == OrderedPair() && second_road_path.second == OrderedPair())) {
        shared_ptr<Path> start_path = board.get_path(second_road_path.first, second_road_path.second);
        second_length = road_length_helper(start_path);
    }

    return max(first_length, second_length);
}

int Player::road_length_helper(const shared_ptr<Path> start_path) {

    struct sequence {
        // Fields
        vector<shared_ptr<Path>> seq;
        bool terminated;
        // Funcs
        void add(const shared_ptr<Path> &path) { seq.push_back(path); }
        sequence(shared_ptr<Path> initial) : terminated(false), seq({initial}) {}
        sequence(vector<shared_ptr<Path>> initial_sequence) : terminated(false), seq(initial_sequence) {}
        sequence() : terminated(false), seq({}) {}
    };

    vector<sequence> path_collection = {sequence(start_path)};

    while (true) {

        vector<sequence> new_sequences;

        for (auto &p : path_collection) {

            // Get the current sequence vector and declare its previous and previous previous entries
            vector<shared_ptr<Path>> &seq = p.seq;
            shared_ptr<Path> previous = !seq.empty() ? seq[seq.size() - 1] : nullptr;
            shared_ptr<Path> prev_prev = seq.size() >= 2 ? seq[seq.size() - 2] : nullptr;

            // If there was no previous entry, there was an issue. We should continue
            if (previous == nullptr) {
                continue;
            }

            set<shared_ptr<Path>> exclude;
            if (prev_prev != nullptr) {
                exclude = prev_prev->get_adjacent_paths();
                exclude.insert(prev_prev);
                for (shared_ptr<Path> has_seen : p.seq) {
                    if (exclude.find(has_seen) == exclude.end()) {
                        exclude.insert(has_seen);
                    }
                }
            }
            set<shared_ptr<Path>> current_adjacencies = get_valid_adjacent_road_paths(previous, exclude);

            if (current_adjacencies.empty()) {
                // If there are no valid adjacent paths from the previous one (cannot continue), then terminate
                p.terminated = true;
            } else {

                // Create a copy out here because by the time I need it, it may have been added to and I want it as-is.
                sequence copy_sequence(seq);
                bool has_seen_adj = false;

                // For each valid path adjacent to the previous one, if it is not already added and is not backtracking, consider it.
                for (auto &road_path : current_adjacencies) {

                    bool already_in_sequence = vector_has_path(seq, road_path);
                    bool is_backtracking = prev_prev != nullptr && (road_path == prev_prev || prev_prev->get_adjacent_paths().count(road_path) > 0);

                    if (!already_in_sequence && !is_backtracking) {
                        // if this is the first (or only) adjacent path, just add to the sequence
                        if (!has_seen_adj) {
                            p.add(road_path);
                            has_seen_adj = true;
                        } else {
                            // Otherwise, create a new sequence that is a copy of the other and add to that

                            // ¡Ay, caramba! I make another copy. Hear me out hear me out. I need this one too for sure.
                            // I need another one to add a new item to. If I added to the original copy, the item would be there for a future iteration
                            // To recap:
                            //    First copy was to avoid items getting added too early to seq, and copying a vector with too many items
                            //    Second copy is to add to directly and not affect future iterations that rely on the original copy. Capiche?
                            sequence copied_copy_seqeuence(copy_sequence.seq);
                            copied_copy_seqeuence.add(road_path);
                            new_sequences.push_back(copied_copy_seqeuence);

                        }
                    }

                }
            }
        }

        path_collection.insert(path_collection.end(),
            new_sequences.begin(), new_sequences.end());

        bool all_terminated = true;
        for (auto &p : path_collection) {
            if (!p.terminated) {
                all_terminated = false;
            }
        }
        if (all_terminated) {
            break;
        }
    }

    int max_length = 0;
    for (auto &p : path_collection) {
        if (p.seq.size() > max_length) {
            max_length = p.seq.size();
        }
    }

    return max_length;
}

set<shared_ptr<Path>> Player::get_valid_adjacent_road_paths(const shared_ptr<Path> path, const set<shared_ptr<Path>> &exclude) const {
    set<shared_ptr<Path>> roads;
    for (auto &p : path->get_adjacent_paths()) {
        if (p->is_occupied()) {
            if (p->get_inhabitant()->get_color() == get_color() && exclude.find(p) == exclude.end()) {
                roads.insert(p);
            }
        }
    }
    return roads;
}

bool Player::vector_has_path(const vector<shared_ptr<Path>> &vec, const shared_ptr<Path> &path) const {
    for (const shared_ptr<Path> &p : vec) {
        if (p == path) {
            return true;
        }
    }
    return false;
}

int Player::get_number_of_placed_roads(const Board &board) const {
    int num = 0;
    for (const shared_ptr<Path> &path : board.get_board_paths()) {
        if (!path->is_occupied()) {
            continue;
        }
        if (path->get_inhabitant()->get_color() == get_color()){
            num += 1;
        }
    }
    return num;
}


void Player::set_length_of_road(const int length, const bool print_on_longest) {
    if (length >= 0) {
        length_of_road = length;
    }

    bool gained_title = false;
    shared_ptr<Player> longest_road_player = GameBank::instance().get_longest_road_player();
    if (longest_road_player == nullptr) {
        if (length_of_road >= 5) {
            set_longest_road(true);
            gained_title = true;
        }
    } else {
        if (longest_road_player != shared_from_this() && length_of_road > longest_road_player->get_length_of_road()) {
            longest_road_player->set_longest_road(false);
            set_longest_road(true);
            gained_title = true;
        }
    }

    if (gained_title && print_on_longest) {
        cout << get_name() << " now has the Longest Road!" << endl;
    }
}

void Player::add_initial_road_anchor(const OrderedPair endpoint_a, const OrderedPair endpoint_b) {
    if (first_road_path == pair<OrderedPair, OrderedPair>()) {
        // If the first road path is the default value, (0, 0) set it to the provided pair.
        // A path's endpoint can never actually be (0,0), so this is safe from being reset multiple times
        first_road_path = pair<OrderedPair, OrderedPair>(endpoint_a, endpoint_b);
    } else if (second_road_path == pair<OrderedPair, OrderedPair>()){
        second_road_path = pair<OrderedPair, OrderedPair>(endpoint_a, endpoint_b);
    }
    // If neither of them are default, do nothing
}

pair<OrderedPair, OrderedPair> Player::get_initial_road_path(const int number) const {
    if (number == 1) {
        return first_road_path;
    }
    if (number == 2){
        return second_road_path;
    }
    return {};
}

bool Player::has_largest_army() const{
    return largest_army;
}

void Player::set_longest_road(const bool _longest_road) {
    longest_road = _longest_road;
}

void Player::set_largest_army(const bool _largest_army) {
    largest_army = _largest_army;
}

void Player::gain_knight() {
    number_of_knights++;

    bool gained_title = false;
    shared_ptr<Player> largest_army_player = GameBank::instance().get_largest_army_player();
    if (largest_army_player == nullptr) {
        if (number_of_knights >= 3) {
            set_largest_army(true);
            gained_title = true;
        }
    } else {
        if (largest_army_player != shared_from_this() && number_of_knights > largest_army_player->get_number_of_knights()) {
            largest_army_player->set_largest_army(false);
            set_largest_army(true);
            gained_title = true;
        }
    }

    if (gained_title) {
        cout << get_name() << " now has the Largest Army!" << endl;
    }
}

int Player::get_number_of_knights() const {
    return number_of_knights;
}

void Player::play_devcard(Board &board, const int index){

    if (index < 0 || index >= devcards.size()) {
        return;
    }

    if (devcards.at(index).get_type() == VICTORY_POINT) {
        cout << "You cannot play a Victory Point Development Card." << endl;
        cout << "The point is added to your total as long as it is in your hand.\n" << endl;
        return;
    }
    if (!devcards.at(index).test_playability(board, shared_from_this())) {
        return;
    }

    // Play the card
    devcards[index].play(board, shared_from_this());
    // Remove the Card
    devcards.erase(devcards.begin() + index);
}

void Player::gain_devcard(const devcard_type devcard, const bool print){
    if (devcard != CARD_EMPTY) {
        devcards.emplace_back(DevCard(devcard));
        sort_devcards();
        if (print) {
            cout << get_name() << " has gained a Development Card" << endl;
        }
    }
}

void Player::gain_devcard(const DevCard devcard, const bool print) {
    if (devcard.is_valid()) {
        devcards.push_back(devcard);
        sort_devcards();
        if (print) {
            cout << get_name() << " has gained a Development Card" << endl;
        }
    }

}

void Player::sort_devcards() {
    int size = devcards.size();
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (devcards.at(i) > devcards.at(j + 1)) {
                swap(devcards.at(i), devcards.at(j + 1));
            }
        }
    }
}

bool Player::has_devcard(const devcard_type devcard) const{
    for (const DevCard &card : devcards) {
        if (card.get_type() == devcard) {
            return true;
        }
    }
    return false;
}

vector<DevCard> Player::get_devcards() const {
    return devcards;
}

void Player::set_played_devcard_this_turn(const bool _played) {
    played_devcard_this_turn = _played;
}

bool Player::has_played_devcard_this_turn() const {
    return played_devcard_this_turn;
}

void Player::start_new_turn() {
    for (DevCard& card : devcards) {
        card.set_bought_this_turn(false);
    }
}


void Player::reset_resources() {
    resource_cards = get_base_resource_map();
}

map<tile_type, int> Player::get_resources() const {
    return resource_cards;
}

map<tile_type, int> Player::get_base_resource_map() {
    return {{GRAIN, 0}, {WOOL, 0},{BRICK, 0}, {ORE, 0}, {LUMBER, 0}};
}

void Player::gain_resource(const tile_type resource) {
    if (!resource_exists(resource)) {
        return;
    }
    resource_cards[resource] += 1;
}

void Player::gain_many_resources(const map<tile_type, int> resources, const bool from_bank, const bool print) {
    // for each pair in the map, print a message and add the numnber of resources.

    for (auto &it : resources) {
        int gain = it.second;
        if (from_bank) {
            gain = min(gain, GameBank::instance().get_available_resources().at(it.first));
        }
        if (gain > 0 && print) {
            cout << get_name() << " gained " << gain << " " << Tile::type_to_string(it.first) << "." << endl;
        }
        for (int i = 0; i < gain; i++) {
            gain_resource(it.first);
            if (from_bank) {
                GameBank::instance().draw_resource(it.first);
            }
        }
    }
}

void Player::discard_resource(const tile_type resource) {
    if (!has_resource(resource)) {
        return;
    }
    resource_cards[resource] -= 1;
}

void Player::discard_many_resources(
    const map<tile_type, int> resources, const string purpose, const bool to_bank, const bool print) {
    // for each pair in the map, print a message and remove the numnber of resources.
    for (auto &resource_pair : resources) {
        int count = 0;
        for (int i = 0; i < resource_pair.second; i++) {
            if (!has_resource(resource_pair.first)) {
                break;
            }
            discard_resource(resource_pair.first);
            if (to_bank) {
                GameBank::instance().return_resource(resource_pair.first);
            }
            count++;
        }
        if (count > 0 && print) {
            cout << get_name() << " " << purpose << " " << count << " " << Tile::type_to_string(resource_pair.first) << "." << endl;
        }
    }
}

tile_type Player::get_random_resource() {
    srand(time(nullptr));
    vector<tile_type> resources;
    for (auto &resource_pair : resource_cards) {
        for (int i = 0; i < resource_pair.second; i++) {
            resources.push_back(resource_pair.first);
        }
    }
    if (resources.size() == 0) {
        return TILE_EMPTY;
    }
    return resources[rand() % resources.size()];
}

bool Player::has_resource(const tile_type resource) const {
    if (!resource_exists(resource)) {
        return false;
    }
    return resource_cards.at(resource) > 0;
}

bool Player::resource_exists(const tile_type resource) const {
    for (auto &it : resource_cards) {
        if (it.first == resource) {
            return true;
        }
    }
    return false;
}

int Player::get_number_of_resources() const {
    int number = 0;
    for (auto &resource : resource_cards) {
        number += resource.second;
    }
    return number;
}

int Player::get_amount_of_resource(const tile_type resource) const {
    for (auto &res : resource_cards) {
        if (res.first == resource) {
            return res.second;
        }
    }
    return 0;
}

bool Player::can_afford(map<tile_type, int> recipe, const bool print) const {
    map<tile_type, string> conversion = get_resource_conversion_to_string();
    conversion.insert({TILE_EMPTY, "..."});
    bool affordable = true;
    for (auto &requirement : recipe) {

        if (!resource_exists(requirement.first)) {
            cout << "SOMETHING WENT WRONG WITH PLAYER RESOURCE MAP INITIALIZATION" << endl;
            return false;
        }

        if (resource_cards.at(requirement.first) < requirement.second) {
            affordable = false;
            break;
        }
    }
    if (print && !affordable) {
        cout << "Sorry, you can't afford that." << endl;
        for (auto &requirement : recipe) {
            cout << "You have " << resource_cards.at(requirement.first) << " / " << requirement.second;
            cout << " of the required " << conversion.at(requirement.first) << ". " << endl;
        }
    }
    return affordable;
}

string Player::get_color_as_text(const player_color color) {
    if (color < 0 || color > 3) {
        return "";
    }
    vector<string> colors_as_text {"Red", "Yellow", "Green", "Blue"};
    return colors_as_text[color];
}

player_color Player::get_color() const {
    return color;
}

// Brute force retuns name because there is only 4 options and using concatenation was causing problems for some reason
string Player::get_name() const {
    string cancel_code = get_cancel_code();
    string apply_code = get_player_color_code(color);
    switch (ID) {
        case 1:
            return "Player 1 (" + apply_code + "Red" + cancel_code + ")";
        case 2:
            return "Player 2 (" + apply_code + "Yellow" + cancel_code + ")";
        case 3:
            return "Player 3 (" + apply_code + "Green" + cancel_code + ")";
        case 4:
            return "Player 4 (" + apply_code + "Blue" + cancel_code + ")";
    }
    return "";
}

int Player::get_city_pieces() const {
    return remaining_cities;
}
int Player::get_road_pieces() const {
    return remaining_roads;
}
int Player::get_settlement_pieces() const {
    return remaining_settlements;
}
void Player::incdec_city_pieces(const int change) {
    remaining_cities += change;
}
void Player::incdec_road_pieces(const int change) {
    remaining_roads += change;
}
void Player::incdec_settlement_pieces(const int change) {
    remaining_settlements += change;
}

void Player::seven_rolled() {
    int total_resource_cards = get_number_of_resources();
    if (total_resource_cards <= 7) {
        cout << get_name() << " has less than or equal to 7 resource cards, so they are safe." << endl;
        return;
    }
    cout << get_name() << ", you have more than seven resource cards, and must discard half of them" << endl;
    int number_to_discard = ceil(total_resource_cards / 2.0);
    discard_resources_prompt(shared_from_this(), number_to_discard);
}

void Player::print_resources() const {
    cout << "Resources: " << endl;;
    for (auto &res : get_resource_conversion_to_string()) {
        cout << " - " << res.second << ": " << resource_cards.at(res.first) << endl;
    }
}

void Player::print_devcards() const {
    if (devcards.size() <= 0) {
        cout << "You do not have any Development Cards..." << endl;
    } else {
        cout << "Development Cards: " << endl;
        for (int i = 0; i < devcards.size(); i++) {
            cout << " " << i << " - " << devcards[i].get_name() << endl;
        }
    }
}

void Player::print_info(const Board &board, const bool show_private) const {
    cout << get_name() << ": " << endl;
    cout << "General info:" << endl;
    // Print points
    if (show_private) {
        cout << " - " << get_points(board, false) << " {" << get_points(board, true) << "} Victory Points" << endl;
    } else {
        cout << " - " << get_points(board, false) << " Victory Points" << endl;
    }
    // Print Road Length
    cout << " - Road length: " << get_length_of_road();
    if (has_longest_road()) {
        cout << " ~ LONGEST ROAD!" << endl;
    } else {
        cout << endl;
    }
    // Print Army Size
    cout << " - Army Size: " << get_number_of_knights();
    if (has_largest_army()) {
        cout << " ~ LARGEST ARMY!" << endl;
    } else {
        cout << endl;
    }
    if (show_private) {
        print_resources();
        print_devcards();
    } else {
        // Print resource count
        cout << " - " << get_number_of_resources() << " Resource Cards" << endl;
        // Print devcard count
        cout << " - " << devcards.size() << " Development Cards" << endl;
    }

}
