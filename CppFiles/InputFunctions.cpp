//
// Created by nmila on 9/9/2025.
//

#include "../HeaderFiles/InputFunctions.h"
#include "../HeaderFiles/Player.h"
#include "../HeaderFiles/GameBank.h"

#include <iostream>
#include <sstream>

using std::cout, std::endl, std::cin;
using std::string, std::vector, std::shared_ptr, std::map, std::stringstream, std::to_string, std::max;

const int EXIT_CODE = -1324387596;

string get_capitalized(string s) {
    s[0] = toupper(s[0]);
    return s;
}

string get_lowercase(string s) {
    for (int i = 0; i < s.length(); i++) {
        s[i] = tolower(s[i]);
    }
    return s;
}

bool vector_has_value(vector<string> vec, string val) {
    for (string item : vec) {
        if (val == item) {
            return true;
        }
    }
    return false;
}

bool vector_has_value(vector<int> vec, int val) {
    for (int item : vec) {
        if (item == val) {
            return true;
        }
    }
    return false;
}

map<tile_type, string> get_resource_conversion_to_string(const bool include_color) {
    map<tile_type, string> res = {
        {GRAIN, "GRAIN"},
        {WOOL, "WOOL"},
        {BRICK, "BRICK"},
        {ORE, "ORE"},
        {LUMBER, "LUMBER"}};
    if (include_color) {
        for (auto &item : res) {
            item.second = get_resource_color_code(item.first) + item.second + get_cancel_code();
        }
    }
    return res;
}

// Gets input from the user for an int.
// Prints provided strings when the situation arises
int generic_int_prompt(
    const string main_message,
    const string empty_message,
    const string invalid_message) {

    // Int and string values for input
    int count = -1;
    string input;
    while (true) {
        stringstream ss;
        cout << "\n" << main_message << endl;
        getline(cin, input);

        string quit_command = "";
        // Lowercase the input to check quit command
        quit_command = get_lowercase(input);
        if (quit_command == "quit") {
            // If the entered string was "quit" return the exit code which is recognized as a quit code
            return EXIT_CODE;
        }
        // Is empty
        if (input.empty()) {
            cout << empty_message << endl;
            continue;
        }
        ss << input;
        // Is not an int
        if (!(ss >> count) || to_string(count).length() != input.length()){
            cout << invalid_message << endl;
            continue;
        }
        break;
    }
    return count;
}

// Prompts the user for a coordinate id to a tile (Ex. b,c or c,f)
string tile_id_prompt(const shared_ptr<Player> &player, const bool can_quit) {
    // If the player is not valid, just return we can't do anything further if thats the case.
    if (player == nullptr) {
        return "";
    }
    // prints initial message
    cout << "\n" << player->get_name() << ", please choose a tile (Ex. b,c)." << endl;
    string tile_id;
    while (true) {
        getline(cin, tile_id);

        // Empty input failure
        if (tile_id.empty()) {
            cout << "Your entry did not specify a tile." << endl;
            cout << "Please choose a tile by typing its ID letters shown on the grid seperated by a ','" << endl;
            continue;
        }

        // Quit prompt
        string quit_command = "";
        quit_command = get_lowercase(tile_id);
        if (quit_command == "quit") {
            if (can_quit) {
                return "quit";
            }
            cout << "Sorry, you cannot quit from this game action." << endl;
            cout << "\n" << player->get_name() << ", please choose a tile (Ex. b,c)." << endl;
            continue;
        }

        // I feel this is an important fail case to do seperately, as it is common to put parentheses in coords
        bool has_parentheses = false;
        for (char ch : tile_id) {
            if (ch == ')' || ch == '(') {
                has_parentheses = true;
                break;
            }
        }

        // Invalid Assumption failure
        if (has_parentheses) {
            cout << "In order to get the tile you desire, avoid including parentheses." << endl;
            cout << "Please enter a tile in the format *,* replacing * with the first and second letters of the tile" << endl;
            continue;
        }

        // Too long Failure
        if (tile_id.size() > 3) {
            cout << "Your entry was too long to specify a single tile." << endl;
            cout << "Please enter a 3 character tile key. (For example, b,c)" << endl;
            continue; }
        // Too Short Failure
        if (tile_id.size() < 3) {
            cout << "Your entry was too short to specify a tile." << endl;
            cout << "Please enter a 3 character tile key. (For example, b,c)" << endl;
            continue; }

        bool first_char_valid = false;
        bool second_char_valid = false;

        // If the first character is in the string aAbBcC, then it is a valid first character
        for (char ch : "aAbBcC") {
            if (ch == tile_id[0]) {
                first_char_valid = true;
                break;
            }
        }

        // If the first character was valid, check the second,
        if (first_char_valid) {
            // Set the first character to lowercase
            char first_char = tolower(tile_id[0]);
            map<char, string> valid_tiles_map = {{'a', "aA"}, {'b', "aAbBcCdDeEfF"}, {'c', "aAbBcCdDeEfFgGhHiIjJkKlL"}};
            for (char ch : valid_tiles_map[first_char]) {
                if (ch == tile_id[2]) {
                    second_char_valid = true;
                    break;
                }
            }
        }
        // If the input did not seperate characters with a comma, tell the player ot do that and try again
        if (tile_id[1] != ',') {
            cout << "Your entry was not in the proper format. Remember to separate the letters you enter with ','" << endl;
            cout << "Please enter a tile in the format *,* replacing * with the first and second letters of the tile" << endl;
            continue;
        }

        // If either of the characters on either side of the comma are invalid, true again and give a hint as to the correct type of input.
        if (!first_char_valid || !second_char_valid) {
            cout << "Either the first or second letters you entered we invalid." << endl;
            cout << "The choices for the first letter are a,b, or c.";
            cout << "The choices for the second letter depend on the first. take a look at the board's tiles and their IDs" << endl;
            cout << "Please enter a tile in the format *,* replacing * with the first and second letters of the tile" << endl;
            continue;
        }

        // String is valid, just make it lowercase
        tile_id = get_lowercase(tile_id);
        // Return the string ID
        return tile_id;

    }
}

// Prompts for the player to place a road at the chosen location
shared_ptr<Path> placement_path_prompt(
    Board &board, const string tile_id,
    const player_color color, const bool require_adjacent_road,
    const bool can_quit) {
    // If there is no tile there, return
    if (!board.has_tile(tile_id)) {
        return nullptr;
    }

    // Gets the tile and displays the board with the available path locations around that tile
    Tile tile = board.get_tile(tile_id);
    tile.override_path_icons(color, require_adjacent_road);
    board.display_board(false, {tile.get_position_id()});
    tile.reset_path_icon_overrides();

    // Constructs valid index message and vector of valid indexes for input
    vector<int> valid_indexes;
    string valid_index_message = "(";
    for (auto &it : tile.get_adjacent_paths()) {
        // If the path already has a road, it is not a valid spot for a road
        if (it.second->is_occupied()){
            continue;
        }
        // If the path requires an adjacent road (most of the game does) and it doesn't, it is not a valid spot
        if (require_adjacent_road && !it.second->has_adjacent_matching_road(color)) {
            continue;
        }
        valid_indexes.push_back(it.first);
        if (valid_index_message.length() > 1) {
            valid_index_message += ", ";
        }
        valid_index_message += to_string(it.first);
    }
    valid_index_message += ")";

    string int_prompt_message;
    if (valid_indexes.empty()) {
        int_prompt_message = "There are no valid locations adjacent to the tile you selected.\n"
                             "Please enter [Quit] to go back.";
    } else {
        int_prompt_message = "Please enter an index for the desired path " + valid_index_message + ". ";
    }

    int path_index = -1;
    while (true) {
        // This loop just handles the int prompt and gettings the path at the provided int.
        path_index = generic_int_prompt(
            int_prompt_message,
            "Sorry, it doesn't seem like you entered anything.",
            "Sorry, it seems you entered something which is not a valid index.");
        if (path_index == EXIT_CODE) {
            if (can_quit) {
                return nullptr;
            }
            cout << "Sorry, you cannot quit from this game action." << endl;
            continue;
        }
        if (!vector_has_value(valid_indexes, path_index)) {
            cout << "Sorry, it seems the index you entered does not exist." << endl;
            continue;
        }
        return tile.get_adjacent_paths().at(path_index);
    }
}

// Handles prompt for placing something on a path, but takes in a building position.
// This is used in the beginning of the game where a player must place a road adjacent to the building they just place
shared_ptr<Path> placement_path_prompt(const Board &board, const OrderedPair building_position, const bool can_quit) {
    if (!board.has_plot(building_position)) {
        return nullptr;
    }

    shared_ptr<Plot> current_plot = board.get_plot(building_position);
    map<int, shared_ptr<Path>> adjacent_paths;

    // Looks at adjacent plots to the building_position plot and uses that to construct the adjacent path locations
    for (auto &it : current_plot->get_adjacent_plots()) {
        if (board.has_path(building_position, it.second->get_position_coordinates())) {
            adjacent_paths.insert(
                {it.first,
                    board.get_path(building_position,
                        it.second->get_position_coordinates())});
        }
    }

    // Set the overrides only for the paths adjacent to the building just placed
    for (auto &it : adjacent_paths) {;
        if (it.second->is_occupied()) {
            it.second->set_icon_override("X");
            continue;
        }
        it.second->set_icon_override("\033[1m" + to_string(it.first) + "\033[0m");
    }

    // Highlights the three adjacent tiles to the just-placed building, for clarity when choosing where the road should go.
    vector<string> highlighted_tiles;
    for (auto coord : current_plot->get_adjacent_tile_coords()) {
        if (board.has_tile(coord)) {
            highlighted_tiles.push_back(board.get_tile(coord).get_position_id());
        }
    }
    // Display the board
    board.display_board(false, highlighted_tiles);
    // Reset the overrides
    for (auto &it : adjacent_paths) {
        it.second->set_icon_override("");
    }

    // Constructs string and vector of valid indexes for input validation and prompt printing
    vector<int> valid_indexes;
    string valid_index_message = "(";
    for (auto &it : adjacent_paths) {
        if (it.second->is_occupied()) {
            continue;
        }
        valid_indexes.push_back(it.first);
        if (valid_index_message.length() > 1) {
            valid_index_message += ", ";
        }
        valid_index_message += to_string(it.first);
    }
    valid_index_message += ")";

    string int_prompt_message;
    if (valid_indexes.empty()) {
        int_prompt_message = "There are no valid locations adjacent to the tile you selected.\n"
                             "Please enter [Quit] to go back.";
    } else {
        int_prompt_message = "Please enter an index for the desired path " + valid_index_message + ". ";
    }

    int plot_index = -1;
    while (true) {
        // This loop handles the int input, validation, and exit code.
        plot_index = generic_int_prompt(
            int_prompt_message,
            "Sorry, it doesn't seem like you entered anything.",
            "Sorry, it seems you entered something which is not a valid index.");
        if (plot_index == EXIT_CODE) {
            if (can_quit) {
                return nullptr;
            }
            cout << "Sorry, you cannot quit from this game action." << endl;
            continue;
        }
        if (!vector_has_value(valid_indexes, plot_index)) {
            cout << "Sorry, it seems the index you entered does not exist." << endl;
            continue;
        }
        return adjacent_paths.at(plot_index);
    }
}

// Prompts the user to place a city on a plot which already has one of their settlements
shared_ptr<Plot> placement_upgrade_prompt(
    Board &board, const shared_ptr<Player> &player,
    const string tile_id, const bool can_quit) {
    // If there is not tile there, return
    if (!board.has_tile(tile_id)) {
        return nullptr;
    }

    // Gets the tile, then displays the board showing the indexes for the available plots adjacent to it
    // A plot will only be available if it already has a settlement of the color.
    Tile& tile = board.get_tile(tile_id);
    tile.override_settlement_icons(player->get_color());
    board.display_board(false, {tile.get_position_id()});
    tile.reset_plot_icon_overrides();

    vector<int> valid_indexes;
    string valid_index_message = "(";
    for (auto &it : tile.get_adjacent_plots()) {
        if (!it.second->is_occupied()) {
            continue;
        }
        if (it.second->get_inhabitant()->get_building_type() != SETTLEMENT) {
            continue;
        }
        if (it.second->get_inhabitant()->get_color() != player->get_color()) {
            continue;
        }

        valid_indexes.push_back(it.first);
        if (valid_index_message.length() > 1) {
            valid_index_message += ", ";
        }
        valid_index_message += to_string(it.first);
    }

    string int_prompt_message;
    if (valid_indexes.empty()) {
        int_prompt_message = "There are no valid locations adjacent to the tile you selected.\n"
                             "Please enter [Quit] to go back.";
    } else {
        int_prompt_message = "Please enter an index for the desired plot " + valid_index_message + ". ";
    }

    int plot_index = -1;
    while (true) {
        // This loop handles the int input, validation, and exit code.
        plot_index = generic_int_prompt(
            int_prompt_message,
            "Sorry, it doesn't seem like you entered anything.",
            "Sorry, it seems you entered something which is not a valid index.");
        if (plot_index == EXIT_CODE) {
            if (can_quit) {
                return nullptr;
            }
            cout << "Sorry, you cannot quit from this game action." << endl;
            continue;
        }
        if (!vector_has_value(valid_indexes, plot_index)) {
            cout << "Sorry, it seems the index you entered does not exist." << endl;
            continue;
        }
        return tile.get_adjacent_plots().at(plot_index);
    }
}

// Prompts the user to place a building on a plot
shared_ptr<Plot> placement_plot_prompt(
    Board &board, const shared_ptr<Player> &player,
    const string tile_id, const bool require_adjacent_road, const bool can_quit) {
    // If there is no tile there, return
    if (!board.has_tile(tile_id)) {
        return nullptr;
    }

    // Gets the tile, then displays the board showing the indexes for the available plots adjacent to it.
    Tile tile = board.get_tile(tile_id);
    tile.override_plot_icons(board, player->get_color(), require_adjacent_road);
    board.display_board(false, {tile.get_position_id()});
    tile.reset_plot_icon_overrides();

    // Formulates the valid indexes and the display of them in the prompt based on if they can be selected for settlement placement
    vector<int> valid_indexes;
    string valid_index_message = "(";
    for (auto &it : tile.get_adjacent_plots()) {
        if (it.second->is_occupied() || it.second->next_to_building()) {
            continue;
        }
        if (require_adjacent_road) {
            bool valid = false;
            for (auto &map_pair : it.second->get_adjacent_plots()) {
                shared_ptr<Path> connected_path =
                    board.get_path(map_pair.second->get_position_coordinates(),
                        it.second->get_position_coordinates());
                if (connected_path == nullptr) {
                    continue;
                }
                if (connected_path->is_occupied() && connected_path->get_inhabitant()->get_color() == player->get_color()) {
                    valid = true;
                }
            }
            if (!valid) {
                continue;
            }
        }
        valid_indexes.push_back(it.first);
        if (valid_index_message.length() > 1) {
            valid_index_message += ", ";
        }
        valid_index_message += to_string(it.first);
    }
    valid_index_message += ")";

    string int_prompt_message;
    if (valid_indexes.empty()) {
        int_prompt_message = "There are no valid locations adjacent to the tile you selected.\n"
                             "Please enter [Quit] to go back.";
    } else {
        int_prompt_message = "Please enter an index for the desired plot " + valid_index_message + ". ";
    }

    int plot_index = -1;
    while (true) {
        // This loop handles the int input, validation, and exit code.
        plot_index = generic_int_prompt(
            int_prompt_message,
            "Sorry, it doesn't seem like you entered anything.",
            "Sorry, it seems you entered something which is not a valid index.");
        if (plot_index == EXIT_CODE) {
            if (can_quit) {
                return nullptr;
            }
            cout << "Sorry, you cannot quit from this game action." << endl;
            continue;
        }
        if (!vector_has_value(valid_indexes, plot_index)) {
            cout << "Sorry, it seems the index you entered does not exist." << endl;
            continue;
        }
        return tile.get_adjacent_plots().at(plot_index);
    }
}

// Prompts the user for the number of players the game will be played with
int player_count_prompt() {
    cout << "Hex Settlers can be played with 2-4 people. ";
    int count = -1;
    while (true) {
        // Loop just handles the int input and validation with correct numbers of players (2, 3, or 4)
        count = generic_int_prompt(
            "Please enter a number of players (2, 3, or 4) ",
            "Sorry, you cannot play Hex Settlers with... *cricket noises* ...players. ",
            "Sorry, the number you entered is not a valid input.");
        if (count < 2 || count > 4) {
            cout << "Unfortunately, you cannot play Hex Settlers with " << count << " players." << endl;
            continue;
        }
        break;
    }
    cout << "Great. We'll start the game with " << count << " players!" << endl;
    return count;
}

tile_type resource_choice_prompt(
    const shared_ptr<Player> &player,
    const bool consider_remaining_bank_resources,
    const bool consider_resources_in_hand,
    const bool can_quit,
    const vector<tile_type> &exclusions,
    const string main_message_override) {
    // Forms the valid resources, and eliminates them if the function cares about what resources remain in the supply
    string input;
    map<tile_type, string> valid_resources = get_resource_conversion_to_string(false);
    // If we are taking the resources in the bank into account of validity, look at each resources in the bank and remove it from the valid resources if there are no cards left
    if (consider_remaining_bank_resources) {
        for (auto &resource : GameBank::instance().get_available_resources()) {
            if (resource.second <= 0) {
                valid_resources.erase(resource.first);
            }
        }
    }
    // If we are taking the resources in hand into account of validity, look at each resource in hand, and remove it if there are none
    // This should never result in an empty map of valid resources unless the above validity trimming occurred. You should never be prompted to choose a resource from your hand if your hand is empty
    if (consider_resources_in_hand) {
        for (auto &resource : player->get_resources()) {
            if (resource.second <= 0 ) {
                valid_resources.erase(resource.first);
            }
        }
    }

    // Remove the options that exist in the exclusion vector
    for (auto &ex : exclusions) {
        if (valid_resources.find(ex) != valid_resources.end()) {
            valid_resources.erase(ex);
        }
    }

    // If there are no resources to choose from, return
    if (valid_resources.empty()) {
        cout << "There are no valid resources to choose." << endl;
        return TILE_EMPTY; }
    // Create the prompt, only printing the available resources as options
    string prompt = main_message_override.empty() ? "Please choose a resource - " : main_message_override;
    for (auto &resource : valid_resources) {
        prompt += "[" + get_resource_color_code(resource.first) + resource.second + get_cancel_code() + "] "; }
    prompt += ": ";
    while (true) {
        cout << prompt << endl;
        getline(cin, input);
        if (input.empty()) {
            cout << "It seems you didn't enter anything " << endl;
            continue; }
        input = get_lowercase(input);
        if (input == "quit") {
            if (can_quit) {
                return TILE_EMPTY;
            }
            cout << "Sorry, but you cannot quit this action." << endl;
        }
        for (auto &resource : valid_resources) {
            if (get_lowercase(resource.second) == input) {
                return resource.first;
            }
        }
        cout << "Sorry, you didn't enter a valid resource. " << endl;
    }
}

// Sets up building placement, displaying board, asking player to choose a tile, and asking the player to choose a plot adjacent to it.
// All of this is 'quit-able'
OrderedPair place_building(
    Board &board, const shared_ptr<Player> &player,
    const bool require_adjacent_road, const bool can_quit, const bool spend_resources) {

    if (player->get_settlement_pieces() <= 0) {
        cout << player->get_name() << ", you have already placed all of your settlement pieces. " << endl;
        return OrderedPair(100, 100);
    }

    while (true) {

        // First display the board showing tile_ids
        board.display_board(true);
        // Then prompt for a tile id

        // Outer loop lets player quit to no longer place a tile, but only if the function parameter can_quit is true can the tile_id_prompt allow it
        // The can_quit parameter would be false in the initial turns ofr example, where a player must place a road
        string tile_id = tile_id_prompt(player, can_quit);
        if (tile_id == "quit" || tile_id == "") {
            return OrderedPair(100, 100);
        }
        while (true) {
            // Inner loop so you can quit and rechoose the tile

            // Then prompt for a plot, which handles displaying the board based on the selected tile and overrides it's plots to show selection indexes
            shared_ptr<Plot> plot = placement_plot_prompt(board, player, tile_id, require_adjacent_road, true);
            if (plot == nullptr) {
                break;
            }

            Building new_building = Building(player, SETTLEMENT);
            plot->set_inhabitant(new_building);
            player->incdec_settlement_pieces(-1);
            if (spend_resources) {
                player->discard_many_resources(new_building.get_recipe(), "spent", true);
            }

            cout << endl;
            board.display_board(false);

            return plot->get_position_coordinates();
        }
    }
}

OrderedPair upgrade_building(Board &board, const shared_ptr<Player> &player, const bool can_quit) {

    if (player->get_city_pieces() <= 0) {
        cout << player->get_name() << ", you have already placed all of your city pieces. " << endl;
        return OrderedPair(100, 100);
    }

    while (true) {

        board.display_board(true);

        string tile_id = tile_id_prompt(player, can_quit);
        if (tile_id == "quit" || tile_id == "") {
            return OrderedPair(100, 100);
        }

        while (true) {

            shared_ptr<Plot> plot = placement_upgrade_prompt(board, player, tile_id, true);
            if (plot == nullptr) {
                break;
            }

            plot->get_inhabitant()->upgrade();
            player->incdec_city_pieces(-1);
            player->incdec_settlement_pieces(1);

            cout << endl;
            board.display_board(false);

            return plot->get_position_coordinates();
        }
    }
}

// Sets up road placement prompt, displaying board, getting the tile id choice, and getting the path index that surrounds the chosen tile
void place_road(Board &board, const shared_ptr<Player> &player, const bool requires_adjacent_road, const bool can_quit, const bool spend_resources) {

    if (player->get_road_pieces() <= 0) {
        cout << player->get_name() << ", you have already placed all of your road pieces. " << endl;
        return;
    }

    while (true) {

        // First display the board showing tile_ids
        board.display_board(true);
        // Then prompt for a tile id

        // Outer loop lets the player quit to cancel placing a road, but only if the parameter can_quit is true can the tile_id_prompt allow this
        // The can_quit parameter would be false in the initial turns ofr example, where a player must place a road
        string tile_id = tile_id_prompt(player, can_quit);
        if (tile_id == "quit" || tile_id == "") {
            return;
        }
        // Then prompt for a path
        while (true) {
            // Inner loop allows player to quit and rechoose tile
            shared_ptr<Path> path =
                placement_path_prompt(board, tile_id, player->get_color(), requires_adjacent_road, true);
            if (path == nullptr) {
                break;
            }
            Road new_road = Road(player);
            path->set_inhabitant(new_road);
            player->incdec_road_pieces(-1);
            player->set_length_of_road(player->calculate_length_of_road(board));
            if (spend_resources) {
                player->discard_many_resources(new_road.get_recipe(), "spent", true);
            }

            cout << endl;
            board.display_board(false);

            return;
        }
    }
}

void play_devcard(Board &board, const shared_ptr<Player> &player, const bool can_quit) {
    player->print_devcards();
    if (player->get_devcards().size() <= 0) {
        return;
    }
    string index_range = "0 - " + to_string(player->get_devcards().size() - 1) + ": ";
    while (true) {
        int devcard_index = generic_int_prompt(
            ("Please choose the index for the Development Card you'd like to play: " + index_range),
            "It seems you didn't enter anything.",
            "It seems you entered something that is not a valid index."
            );
        if (devcard_index == EXIT_CODE) {
            if (can_quit) {
                return;
            }
            cout << "Sorry, you cannot quit from this game action. " << endl;
            continue;
        }
        if (devcard_index < 0 || devcard_index >= player->get_devcards().size()) {
            cout << "Sorry, the index you entered is within the valid range: " << index_range << endl;
            continue;
        }
        player->play_devcard(board, devcard_index);
        return;
    }
}

// Handles initial road placement, which is different from regular road placement because the initial roads are always placed adjacent to the
// building that was just placed
void initial_road_placement(const Board &board, const shared_ptr<Player> &player, const OrderedPair building_position) {
    // Prompts for a path given building position
    shared_ptr<Path> path = placement_path_prompt(board, building_position, true);

    // Creates a road and sticks it into the pack
    Road new_road = Road(player);
    path->set_inhabitant(new_road);

    cout << endl;
    board.display_board(false);

    player->incdec_road_pieces(-1);
    player->add_initial_road_anchor(
        path->get_endpoints().plot_a->get_position_coordinates(),
        path->get_endpoints().plot_b->get_position_coordinates());
    player->set_length_of_road(player->calculate_length_of_road(board));
}

void initial_player_setup(Board &board, const shared_ptr<Player> &player, const bool gain_adjacent_resources) {
    // Start with placing a building a road, based on the buildings position
    // You CANNOT skip this
    OrderedPair building_position = place_building(board, player, false, false, false);

    if (gain_adjacent_resources) {

        map<tile_type, int> resources_gained = Player::get_base_resource_map();

        for (OrderedPair coord : board.get_plot(building_position)->get_adjacent_tile_coords()) {
            // Gets the resource type and if it is a real resource, adds it to the map
            tile_type resource = board.get_tile(coord).get_type();
            if (resource != DESERT && resource != TILE_EMPTY) {
                resources_gained.at(resource) += 1;
            }
        }
        // gain the resources seen
        player->gain_many_resources(resources_gained, true);
    }

    initial_road_placement(board, player, building_position);
}

void robber_movement_prompt(Board &board, shared_ptr<Player> &player, const bool steal_resource) {
    board.display_board(true);
    cout << "\n" << player->get_name() << " may now move the robber. ";
    while (true) {
        string tile_id = tile_id_prompt(player, false);
        if (board.get_tile(tile_id).has_robber()) {
            cout << "The tile you chose is invalid. Please choose a tile that does not already have the robber" << endl;
            continue;
        }
        board.set_robber_position(tile_id);
        board.display_board(false);
        if (steal_resource) {
            adjacent_resource_theft_prompt(board, player, tile_id);
        }
        return;
    }
}

void adjacent_resource_theft_prompt(Board &board, shared_ptr<Player> &player, const string tile_id) {
    const Tile tile = board.get_tile(tile_id);
    vector<string> valid_players;
    map<player_color, string> colors =
        {{RED, "red"}, {YELLOW, "yellow"},
        {GREEN, "green"}, {BLUE, "blue"}};
    for (auto color : colors) {
        for (auto plot_pair : tile.get_adjacent_plots()) {
            if (!plot_pair.second->is_occupied()) {
                continue;}
            if (plot_pair.second->get_inhabitant()->get_color() == color.first && color.first != player->get_color()) {
                valid_players.push_back(color.second);
                break; }
        }
    }
    if (valid_players.empty()) {
        cout << "Sorry " << player->get_name() << ". There are no adjacent players to steal from." << endl;
        return;}

    string input = "";
    while (true) {
        cout << "Please choose a player color to steal from: ";
        for (string color : valid_players) {
            cout << "[" << get_capitalized(color) << "] ";
        }

        getline(cin, input);
        if (input.empty()) {
            cout << "It seems you didn't enter anything." << endl;
            continue;
        }
        // Lowercase for user flexibility
        input = get_lowercase(input);

        if (!vector_has_value(valid_players, input)) {
            cout << "Sorry, it seems the player you chose is not a valid player at all." << endl;
            continue; }

        shared_ptr<Player> target = GameBank::instance().get_player(input);
        if (target == nullptr) {
            cout << "SOMETHING WENT WRONG WITH RETRIEVING THE PLAYER GIVEN A STRING" << endl;
            continue;
        }

        tile_type chosen_resource = target->get_random_resource();
        if (chosen_resource == TILE_EMPTY) {
            cout << "Bummer..." << target->get_name() << " does not have any resources to steal." << endl;
            return;
        }
        target->discard_many_resources({{chosen_resource, 1}}, "lost", false);
        player->gain_many_resources({{chosen_resource, 1}}, false);
        return;
    }
}

void discard_resources_prompt(shared_ptr<Player> player, int number_of_resources) {
    if (number_of_resources < 1) {
        cout << "SOMETHING WENT WRONG WITH DISCARD RESOURCE COUNT" << endl;
        return;
    }
    cout << "You must discard " << number_of_resources << " resources total." << endl;
    while (true) {
        tile_type resource_choice = resource_choice_prompt(player, false, true);
        if (resource_choice == TILE_EMPTY) {
            return;
        }
        while (true) {
            cout << "You have " << player->get_amount_of_resource(resource_choice) << " " <<
                get_resource_conversion_to_string().at(resource_choice) << " and still need to discard " << number_of_resources << " resources. " << endl;
            int count = generic_int_prompt(
                "Please choose a number of the chosen resource to discard: ",
                "It seems you didn't enter anything",
                "The number you entered is invalid."
                );
            if (count == EXIT_CODE) {
                break; }
            if (count < 1) {
                cout << "You cannot discard " << count << " of the chosen resource" << endl;
                continue; }
            if (count > player->get_resources().at(resource_choice)) {
                cout << "You cannot discard " << count << " of the chosen resource because you only have " << player->get_resources().at(resource_choice) << "." << endl;
                continue; }
            if (count > number_of_resources) {
                cout << "You cannot discard " << count << " of the chosen resource because you only need to discard " << number_of_resources << " resources." << endl;
                continue;
            }

            number_of_resources -= count;
            player->discard_many_resources({{resource_choice, count}}, "discarded", true);

            if (number_of_resources > 0) {
                cout << player->get_name() << ", you still need to discard " << number_of_resources << " resources." << endl;
            }

            break;
        }

        if (number_of_resources <= 0) {
            return;
        }
    }
}

void request_info_prompt(
    const Board &board, const shared_ptr<Player> &player,
    const map<int, shared_ptr<Player>> &players) {

    string input;
    while (true) {
        cout << "What type of info would you like to see?" << endl;
        cout << " [Quit]  [Board]  [Player]  [Rules]  [Recipes]" << endl;
        getline(cin, input);
        if (input.empty()) {
            cout << "It seems you didn't enter anything." << endl;
            continue;
        }
        input = get_lowercase(input);
        if (input == "quit") {
            return;
        }
        if (input == "player") {
            request_player_info_prompt(board, player, players);
            return;
        }
        if (input == "rules") {
            help_message();
            return;
        }
        if (input == "recipes") {
            recipe_message();
            return;
        }
        if (input == "board") {
            request_board_info_prompt(board);
            return;
        }
    }
}

void request_board_info_prompt(const Board &board) {
    string input;

    while (true) {
        cout << "What would you like?" << endl;
        cout << " [Quit]  [Display]  [Rules] " << endl;
        getline(cin, input);
        if (input.empty()) {
            cout << "It seems you didn't enter anything." << endl;
            continue;
        }
        input = get_lowercase(input);
        if (input == "quit") {
            return;
        }
        if (input == "display") {
            string board_info_input;
            while (true) {
                cout << "Would you like to include the tiles' ID coordinates? [Y]/[N]?" << endl;
                getline(cin, board_info_input);
                if (board_info_input.empty()) {
                    cout << "It seems you didn't enter anything." << endl;
                    continue;
                }
                board_info_input = get_lowercase(board_info_input);
                if (board_info_input == "quit") {
                    break;
                }
                if (!vector_has_value({"y", "n"}, board_info_input)) {
                    cout << "Invalid input." << endl;
                    continue;
                }
                board.display_board(board_info_input == "y");
                return;
            }
        }
        if (input == "rules") {
            cout << "Here is some rules info on the board:" << endl;
            cout << " - The board is seperated into 19 tiles, each with 6 adjacent paths and intersections" << endl;
            cout << " - Buildings are placed on intersections. Roads are placed on paths." << endl;
            cout << " - There are ports on the shores of the island which indicate their trade type." << endl;
            cout << " - If a player has a building placed on an intersection marked with a port, they gain its benefit." << endl;
            cout << " - Ports marked with a resource give the player a permanent 2:1 trade with the bank with that resource." << endl;
            cout << " - Ports marked with '?' give the player a permanent 3:1 trade with all resources." << endl;
            cout << " - The robber is denoted by !R! and prevents resource production from the tile it is on." << endl;
            cout << " - Resources are produced based on the number rolled on the die. If it matches a tile's number, it produces." << endl;
            cout << " - The resource produced is denoted by the letter on the tile (Ex. W = WOOL). D is the desert which produces nothing." << endl;
            cout << endl;
            return;
        }
    }
}


void request_player_info_prompt(const Board &board, const shared_ptr<Player> &subject_player, const map<int, shared_ptr<Player>> &players) {
    map<player_color, string> colors = {{RED, "red"}, {YELLOW, "yellow"}, {GREEN, "green"}, {BLUE, "blue"}};
    vector<string> choices = {"all"};
    // Form choices vector, including the strings corresponding with the enums that exist in the current playeres
    for (auto &color : colors) {
        for (auto &player : players) {
            if (player.second->get_color() == color.first) {
                choices.push_back(color.second);
            }
        }
    }
    string choice_message = " [Quit] ";
    for (string choice : choices) {
        choice_message += " [" + get_capitalized(choice) + "] ";
    }
    string input;
    while (true) {
        cout << subject_player->get_name() << ", please choose a player whose info you'd like displayed" << endl;
        cout << choice_message << endl;
        getline(cin, input);
        if (input.empty()) {
            cout << "It seems you didn't enter anything." << endl;
            continue;
        }
        input = get_lowercase(input);
        if (input == "quit") {
            return; }
        if (!vector_has_value(choices, input)) {
            cout << "Your response was invalid." << endl;
            continue; }
        if (input == "all") {
            for (auto &player : players) {
                player.second->print_info(board);
            }
            return;
        }
        shared_ptr<Player> target_player = GameBank::instance().get_player(input);
        if (target_player == nullptr) {
            cout << "SOMETHING WENT WRONG IN REQUESTING A PLAYER'S INFO AND RETRIEVING THE PLAYER THROUGH THE GAMEBANK" << endl;
            continue;
        }
        // If the player chose themselves, let them choose whether they print all info, or just the general public info
        if (target_player == subject_player) {
            cout << subject_player->get_name() << ", you chose yourself." << endl;
            string priv_info_input;
            while (true) {
                cout << "Would you like to print private info (cards in hand, resources, etc.) [y]/[n]" << endl;
                getline(cin, priv_info_input);
                if (priv_info_input.empty()) {
                    cout << "It seems you didn't enter anything." << endl;
                    continue; }
                priv_info_input = get_lowercase(priv_info_input);
                if (priv_info_input == "quit") {
                    break;
                }
                if (!vector_has_value({"y", "n"}, priv_info_input)) {
                    cout << "Invalid input." << endl;
                    continue;
                }
                subject_player->print_info(board, priv_info_input == "y");
                return;
            }
        }

        target_player->print_info(board);
        return;
    }
}

void craft_something_prompt(Board &board, const shared_ptr<Player> &player) {
    map<tile_type, string> conversion = get_resource_conversion_to_string();

    vector<string> options;
    if (player->get_settlement_pieces() > 0 &&
        player->can_afford(Craftable::get_recipe("settlement"), false)) { options.emplace_back("settlement"); }
    if (player->get_city_pieces() > 0 &&
        player->can_afford(Craftable::get_recipe("city"), false)) { options.emplace_back("city"); }
    if (player->get_road_pieces() > 0 &&
        player->can_afford(Craftable::get_recipe("road"), false)) { options.emplace_back("road"); }
    if (GameBank::instance().has_devcard() &&
        player->can_afford(Craftable::get_recipe("devcard"), false)) { options.emplace_back("development card"); }

    string resource_message;
    for (auto &res : player->get_resources()) {
        resource_message += " | " + conversion.at(res.first) + " - " + to_string(res.second);
    }

    if (options.empty()) {
        cout << endl;
        cout << resource_message << endl;
        cout << "Sorry, there is nothing you have the resources to craft right now. \n" << endl;
        return;
    }

    cout << player->get_name() << ", what would you like to craft?" << endl;

    vector<string> alternate_codes = {"devcard", "settle", "s", "c", "r", "d"};
    string option_message = " [Quit] ";
    for (string option : options) {
        option_message += " [" + get_capitalized(option) + "] ";
    }

    string input;
    while (true) {
        cout << "\nWhat would you like to craft?" << endl;
        cout << resource_message << endl;
        cout << option_message << endl;
        getline(cin, input);
        if (input.empty()) {
            cout << "It seems you didn't enter anything." << endl;
            continue;
        }
        input = get_lowercase(input);
        if (input == "quit") {
            return;
        }
        if (!vector_has_value(options, input) && !vector_has_value(alternate_codes, input)) {
            cout << "Your response was invalid." << endl;
            continue;
        }
        map<tile_type, int> desired_recipe = Craftable::get_recipe(input);
        if (!player->can_afford(desired_recipe, true)) {
            continue;
        }

        if ((input == "settlement" || input == "settle" || input == "s") && vector_has_value(options, "settlement")) {
            place_building(board, player, true, true, true);
            return;
        }
        if ((input == "city" || input == "c") && vector_has_value(options, "city")){
            upgrade_building(board, player, true);
            return;
        }
        if ((input == "road" || input == "r") && vector_has_value(options, "road")) {
            place_road(board, player, true, true, true);
            return;
        }
        if ((input == "development card" || input == "devcard" || input == "d") && vector_has_value(options, "development card")){
            if (!GameBank::instance().has_devcard()) {
                cout << "Sorry, there are no Development Cards remaining." << endl;
                continue;
            }
            player->discard_many_resources(desired_recipe, "spent", true);
            player->gain_devcard(GameBank::instance().draw_devcard());
            return;
        }
    }
}

void trade_prompt(const Board &board, const shared_ptr<Player> &subject_player, const map<int, shared_ptr<Player>> &players) {
    map<tile_type, string> resource_conversion = get_resource_conversion_to_string();
    map<int, string> conversion = {{RED, "red"}, {YELLOW, "yellow"}, {GREEN, "green"}, {BLUE, "blue"}, {COLOR_EMPTY, "???"}};
    vector<string> options;
    for (auto &player : players) {
        if (player.second != subject_player) {
            options.push_back(conversion.at(player.second->get_color()));
        }
    }
    string option_message = " [Quit]  [Bank] ";
    for (string option : options) {
        option_message += " [" + get_capitalized(option) + "] ";
    }

    string input;
    while (true) {
        cout << subject_player->get_name() << ", with whom would you like to trade?" << endl;
        cout << option_message << endl;
        getline(cin, input);
        if (input.empty()) {
            cout << "It seems you didn't enter anything." << endl;
            continue;
        }
        input = get_lowercase(input);
        if (input == "quit") {
            return;
        }
        if (input == "bank") {
            maritime_trade(subject_player, subject_player->get_ports(board), subject_player->has_universal_port(board));
            return;
        }
        if (!vector_has_value(options, input)) {
            cout << "Your response was invalid." << endl;
            continue;
        }
        shared_ptr<Player> target_player = GameBank::instance().get_player(input);
        if (target_player == nullptr) {
            cout << "SOMETHING WENT WRONG WITH THE GAMEBANK GETPLAYER" << endl;
            return;
        }
        cout << subject_player->get_name() << " has decided to propose a trade with " << target_player->get_name() << ". " << endl;

        map<tile_type, int> current_payment;
        map<tile_type, int> current_gain;
        string trade_input;
        string previous_input;
        while (true) {
            cout << subject_player->get_name() << " How would you like to proceed with the trade?" << endl;
            cout << " [Quit]  [Modify]  [Confirm]  [Review]" << endl;
            getline(cin, trade_input);
            if (trade_input.empty()) {
                cout << "It seems you didn't enter anything." << endl;
                continue;
            }
            trade_input = get_lowercase(trade_input);
            if (trade_input == "quit") {
                break;
            }
            if (trade_input == "modify") {
                previous_input = "modify";
                modify_trade_prompt(subject_player, current_payment, current_gain);
                continue;
            }
            if (trade_input == "confirm") {
                previous_input = "confirm";

                if (!valid_trade_side(current_payment)) {
                    cout << "You cannot confirm a trade with no payment. " << endl;
                    continue;
                }

                if (!valid_trade_side(current_gain)) {
                    cout << "You cannot confirm a trade with no gain. " << endl;
                    continue;
                }

                if (previous_input == "modify") {
                    cout << "The trade is the following: " << endl;
                    display_trade(subject_player, target_player, current_payment, current_gain);
                }
                bool result = trade_confirm_prompt(subject_player, target_player, current_payment, current_gain);
                if (!result) {
                    return;
                }
                continue;
            }
            if (trade_input == "review") {
                previous_input = "review";
                display_trade(subject_player, target_player, current_payment, current_gain);
                continue;
            }
            cout << "Invalid input." << endl;
        }
    }
}

void maritime_trade(const shared_ptr<Player> &subject_player, const vector<tile_type> &typed_ports, const bool with_universal_port) {

    int trade_price = with_universal_port ? 3 : 4;

    map<tile_type, int> trade_rates = {
        {GRAIN, trade_price},
        {WOOL, trade_price},
        {BRICK, trade_price},
        {ORE, trade_price},
        {LUMBER, trade_price}};

    for (auto &type : typed_ports) {
        if (trade_rates.find(type) != trade_rates.end()) {
            trade_rates[type] = 2;
        }
    }

    // Gets a list of all resources the player does not have enough of to trade.
    vector<tile_type> invalid_payments;

    for (auto &res : subject_player->get_resources()) {
        if (res.second < trade_rates.at(res.first)) {
            invalid_payments.push_back(res.first);
        }
    }

    cout << "Trade rates: " << endl;
    for (auto &rate : trade_rates) {
        string material = get_resource_conversion_to_string().at(rate.first);
        cout << " | " << material << " - " << rate.second << ":1 ";
    }
    cout << " |" << endl;

    tile_type payment = resource_choice_prompt(
        subject_player,
        false,
        false,
        true,
        invalid_payments,
        "Please choose the resource you will pay - ");
    if (payment == TILE_EMPTY) {
        return;
    }

    tile_type gain = resource_choice_prompt(
        subject_player,
        true,
        false,
        true,
        {},
        "Please choose a resource to receive - ");
    if (gain == TILE_EMPTY) {
        return;
    }

    subject_player->discard_many_resources({{payment, trade_rates.at(payment)}}, "traded away", true);
    subject_player->gain_many_resources({{gain, 1}}, true);

}

bool trade_confirm_prompt(
    const shared_ptr<Player> &proposer, const shared_ptr<Player> &target_player,
    const map<tile_type, int> &proposers_payment, const map<tile_type, int> &targets_payment) {

    string input;
    while (true) {
        cout << target_player->get_name() << ", would you like to [Confirm], [Deny], or [Suggest] changes." << endl;
        getline(cin, input);
        if (input.empty()) {
            cout << "It seems you didn't enter anything." << endl;
            continue;
        }
        input = get_lowercase(input);
        if (input == "quit") {
            cout << "You cannot quit from this game action." << endl;
            continue;
        }
        if (input == "deny") {
            cout << target_player->get_name() << " has denied the trade." << endl;
            return false;
        }
        if (input == "confirm") {
            if (!target_player->can_afford(targets_payment, false)) {
                cout << "Sorry " << target_player->get_name() << ", " << proposer->get_name() << " was too ambitious." << endl;
                target_player->can_afford(targets_payment, true);
                continue;
            }
            cout << target_player->get_name() << " has confirmed the trade." << endl;

            proposer->discard_many_resources(proposers_payment, "traded away", false);
            target_player->gain_many_resources(proposers_payment, false);

            target_player->discard_many_resources(targets_payment, "traded away", false);
            proposer->gain_many_resources(targets_payment, false);

            return false;
        }
        if (input == "suggest") {
            cout << target_player->get_name() << " would like some changes made to the trade." << endl;
            return true;
        }
        if (input == "suggest changes") {
            cout << "There is no command called suggest changes. Did you mean 'suggest'?" << endl;
            continue;
        }
        cout << "Invalid input." << endl;
    }
}

void display_trade(const shared_ptr<Player> &proposer, const shared_ptr<Player> &target_player, const map<tile_type, int> &payment, const map<tile_type, int> &gain) {
    map<tile_type, string> resource_conversion = get_resource_conversion_to_string();
    cout << proposer->get_name() << " ----> " << target_player->get_name() << endl;
    if (!valid_trade_side(payment)) {
        cout << " * No Payment * " << endl;
    } else {
        for (auto &resource : payment) {
            cout << " - " << resource.second << " " << resource_conversion.at(resource.first) << endl;
        }
    }
    cout << proposer->get_name() << " <---- " << target_player->get_name() << endl;
    if (!valid_trade_side(gain)) {
        cout << " * No Gain * " << endl;
    } else {
        for (auto &resource : gain) {
            cout << " - " << resource.second << " " << resource_conversion.at(resource.first) << endl;
        }
    }

}

bool valid_trade_side(const map<tile_type, int> &payment) {
    if (payment.size() == 0) {
        return false;
    }
    for (auto &resource : payment) {
        if (resource.second > 0) {
            return true;
        }
    }
    return false;
}

void modify_trade_prompt(const shared_ptr<Player> &proposer, map<tile_type, int> &payment, map<tile_type, int> &gain) {
    map<tile_type, string> resource_conversion = get_resource_conversion_to_string();
    string input;
    string target_aspect;
    string change;
    while (true) {
        cout << "How would you like to change about the trade?" << endl;
        cout << " [Quit]  [Payment]  [Gain]" << endl;
        getline(cin, input);
        if (input.empty()) {
            cout << "It seems you didn't enter anything." << endl;
            continue;
        }
        input = get_lowercase(input);
        if (input == "quit") {
            return;
        }
        if (input == "payment") {
            target_aspect = "payment";
            break;
        }
        if (input == "gain") {
            target_aspect = "gain";
            break;
        }
        cout << "Invalid input." << endl;
    }

    while (true) {
        cout << "What would you like to do?" << endl;
        cout << " [Quit]  [Add]  [Subtract]" << endl;
        getline(cin, input);
        if (input.empty()) {
            cout << "It seems you didn't enter anything." << endl;
            continue;
        }
        input = get_lowercase(input);
        if (input == "quit") {
            return;
        }
        if (input == "add") {
            change = "add";
            break;
        }
        if (input == "subtract") {
            change = "subtract";
            break;
        }
        cout << "Invalid input." << endl;
    }

    tile_type resource_choice;
    while (true) {
        cout << "Choose a resource to " << change << ". ";
        resource_choice = resource_choice_prompt(
            proposer, false,
            change == "add" && target_aspect == "payment", true);
        if (resource_choice == TILE_EMPTY) {
            return;
        }

        int max_addition;
        if (payment.find(resource_choice) == payment.end()) {
            max_addition = proposer->get_amount_of_resource(resource_choice);
        } else {
            max_addition = proposer->get_amount_of_resource(resource_choice) - payment.at(resource_choice);
        }
        if (target_aspect == "payment") {
            cout << "You have " << max_addition << " " <<
                resource_conversion.at(resource_choice) << " that has not been added to the trade.";
        }
        int num = generic_int_prompt(
            "How much " + resource_conversion.at(resource_choice) + " would you like to " + change + "?",
            "It seems you didn't enter anything",
            "Invalid input for number of resources.");
        if (num <= 0) {
            cout << "Invalid input. You cannot " << change << " " << num << " " << resource_conversion.at(resource_choice) << "." << endl;
            continue;
        }
        if (change == "add" && target_aspect == "payment" && num > max_addition) {
            cout << "You cannot add " << num << " " << resource_conversion.at(resource_choice) <<
                " to the payment. You only have " << max_addition << " left that has not been added already." << endl;
            continue;
        }

        if (target_aspect == "payment") {
            modify_trade_side(payment, change, target_aspect, resource_choice, num);
        } else {
            modify_trade_side(gain, change, target_aspect, resource_choice, num);
        }
        return;
    }
}

void modify_trade_side(
    map<tile_type, int> &side, const string change,
    const string target_aspect, const tile_type resource, const int amount) {
    if (!vector_has_value({"add", "subtract"}, change) || !vector_has_value({"payment", "gain"}, target_aspect)) {
        cout << "SOMETHING WENT WRONG WITH PROVIDED CHANGE OR TARGET ASPECT" << endl;
        return;
    }
    if (resource == TILE_EMPTY) {
        cout << "RESOURCE TO ADD/SUBTRACT FROM TRADE SIDE INVALID" << endl;
        return;
    }
    map<tile_type, string> resource_conversion = get_resource_conversion_to_string();
    if (side.find(resource) == side.end() && change == "add") {
        side.insert({resource, amount});
        cout << amount << " " << resource_conversion.at(resource) << " added to "  << target_aspect << "." << endl;
    } else {
        for (auto &res : side) {
            if (res.first == resource) {
                if (change == "add") {
                    res.second += amount;
                    cout << amount << " " << resource_conversion.at(resource) << " added to "  << target_aspect << "." << endl;
                } else {
                    int delta = max(res.second, amount);
                    res.second -= delta;
                    cout << delta << " " << resource_conversion.at(resource) << " subtracted from " << target_aspect << "." << endl;
                }
            }
        }
    }
}

void help_message() {
    cout << "| How To Play |\n";
    cout << "Settlers of Catan is a game about collecting and managing resources and expanding your settlements across the island." << endl;
    cout << "Unfortunately, this is not Settlers of Catan, but rather Hex Settlers..." << endl;;
    cout << "Hex Settlers has the same rules as Settlers of Catan, so for any specific\n"
            "rules queries, you may need to seek an official rulebook" << endl;
    cout << "\n| Basics |\n";
    cout << " - A players turn starts with rolling two six sided dice.\n"
            "The result determines the resources gathered based on the numbers displayed on the tiles." << endl;
    cout << " - If a tile has the robber {!R!} on it, it will not generate resources." << endl;
    cout << " - Settlements (S) generate 1 resource while Cities (C) generate 2 resources" << endl;
    cout << " - A city must be placed replacing an existing settlement you own." << endl;
    cout << " - Settlements cannot be placed within 1 space of another and usually must be placed at the end of a road you own." << endl;
    cout << " - You use resources to craft development cards for powerful effects, or buildings/roads to expand your rule." << endl;;
    cout << " - The winner is the first person to 10 points. Points are determined mostly by buildings.\n"
            "Settlements (1 point each); Cities (2 points each)." << endl;
    cout << " - ... but there are also special awards for players with both the largest army of knight cards played, and the longest\n"
            "continuous road. Both of these are worth 2 points." << endl;
    cout << " - You may trade resources with other willing players on your turn, or the bank but at a steep cost (4:1)\n" << endl;
    cout << "That's pretty much it for the basics of Hex Settlers. Good luck!\n" << endl;
}

void recipe_message() {
    map<tile_type, string> conversion = get_resource_conversion_to_string();
    cout << "Crafting Recipes:" << endl;

    vector<string> craftables = {"settlement", "city", "road", "devcard"};
    for (string craft : craftables) {
        cout << " - " << get_capitalized(craft) << ": ";
        for (auto &res : Craftable::get_recipe(craft)) {
            cout << res.second << " " << conversion.at(res.first) << " | ";
        }
        cout << endl;
    }
    cout << endl;
}

void welcome_message() {
    cout << "\n| Welcome to Hex Settlers |\n" << endl;
}