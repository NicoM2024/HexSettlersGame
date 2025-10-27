//
// Created by nmila on 9/4/2025.
//
#include <iostream>
#include <windows.h>

#include "HeaderFiles/Board.h"
#include "HeaderFiles/Player.h"
#include "HeaderFiles/InputFunctions.h"
#include "HeaderFiles/GameBank.h"

using std::cout, std::endl, std::cin, std::vector, std::map, std::shared_ptr;
using std::getline, std::string, std::to_string, std::string_literals::operator ""s;

void player_board_setup(Board &board, const map<int, shared_ptr<Player>> &players);

void roll_dice(Board &board, shared_ptr<Player> current_player, map<int, shared_ptr<Player>> players, const int number_override = -1);

void print_winner_score(const Board &board, const shared_ptr<Player> &current_player);

void quick_setup(Board &board);

int main() {
    system(("chcp "s + to_string(CP_UTF8)).c_str());

    welcome_message();
    help_message();

    GameBank::instance();
    Board board = Board();

    player_board_setup(board, GameBank::instance().get_players());

    board.display_board();

    int turn_index = 1;
    cout << "\nThe game will now truly begin!\n" << endl;
    while (true) {
        shared_ptr<Player> current_player = GameBank::instance().get_player(turn_index);

        current_player->start_new_turn();
        roll_dice(board, current_player, GameBank::instance().get_players());

        cout << "\n" << current_player->get_name() << ", it is your turn? ";
        string input;
        while (true) {
            cout << "What would you like to do?" << endl;
            cout << " [Pass]  [Trade]  [Craft]  [Devcard]  [Info]" << endl;
            getline(cin, input);
            if (input.empty()) {
                cout << "It seems you didn't enter anything" << endl;
                continue;
            }
            input = get_lowercase(input);
            if (input == "quit") {
                cout << "You cannot quit from this game state. Did you mean [Pass]?" << endl;
                continue;
            }
            if (input == "pass") {
                break;
            }
            if (input == "trade") {
                trade_prompt(board, current_player, GameBank::instance().get_players());
            }
            if (input == "craft") {
                craft_something_prompt(board, current_player);
            }
            if (input == "devcard") {
                play_devcard(board, current_player, true);
            }
            if (input == "info") {
                request_info_prompt(board, current_player, GameBank::instance().get_players());
            }
        }

        if (current_player->get_points(board) >= 10) {
            print_winner_score(board, current_player);
            break;
        }

        // Continue turn index
        turn_index += 1;
        if (turn_index > GameBank::instance().get_players().size()) {
            turn_index = 1;
        }
        cout << endl;
    }

    cout << "\n\n" << "Thanks for Playing!\n\n" << endl;
    return 0;
}

void quick_setup(Board &board) {
    // RED
    if (GameBank::instance().get_player(RED) != nullptr) {
        shared_ptr<Player> red = GameBank::instance().get_player(RED);
        board.get_plot(OrderedPair(-1, 0))->set_inhabitant(Building(red, SETTLEMENT));
        board.get_plot(OrderedPair(3, -2))->set_inhabitant(Building(red, SETTLEMENT));

        board.get_path(OrderedPair(-1, 0), OrderedPair(-1, 1))->set_inhabitant(Road(red));
        red->add_initial_road_anchor(OrderedPair(-1, 0), OrderedPair(-1, 1));

        board.get_path(OrderedPair(3, -2), OrderedPair(3, -1))->set_inhabitant(Road(red));
        red->add_initial_road_anchor(OrderedPair(3, -2), OrderedPair(3, -1));
    }

    // YELLOW
    if (GameBank::instance().get_player(YELLOW) != nullptr) {
        shared_ptr<Player> yellow = GameBank::instance().get_player(YELLOW);
        board.get_plot(OrderedPair(1, -1))->set_inhabitant(Building(yellow, SETTLEMENT));
        board.get_plot(OrderedPair(-1, 3))->set_inhabitant(Building(yellow, SETTLEMENT));

        board.get_path(OrderedPair(1, -1), OrderedPair(1, 0))->set_inhabitant(Road(yellow));
        yellow->add_initial_road_anchor(OrderedPair(-1, 0), OrderedPair(-1, 1));

        board.get_path(OrderedPair(-1, 3), OrderedPair(1, 3))->set_inhabitant(Road(yellow));
        yellow->add_initial_road_anchor(OrderedPair(-1, 3), OrderedPair(-1, 3));
    }

    // GREEN
    if (GameBank::instance().get_player(GREEN) != nullptr) {
        shared_ptr<Player> green = GameBank::instance().get_player(GREEN);
        board.get_plot(OrderedPair(-3, -1))->set_inhabitant(Building(green, SETTLEMENT));
        board.get_plot(OrderedPair(3, 2))->set_inhabitant(Building(green, SETTLEMENT));

        board.get_path(OrderedPair(3, 2), OrderedPair(3, 1))->set_inhabitant(Road(green));
        green->add_initial_road_anchor(OrderedPair(3, 2), OrderedPair(3, 1));

        board.get_path(OrderedPair(-3, -1), OrderedPair(-5, -1))->set_inhabitant(Road(green));
        green->add_initial_road_anchor(OrderedPair(-3, -1), OrderedPair(-5, -1));
    }

    // BLUE
    if (GameBank::instance().get_player(BLUE) != nullptr) {
        shared_ptr<Player> blue = GameBank::instance().get_player(BLUE);
        board.get_plot(OrderedPair(-1, -3))->set_inhabitant(Building(blue, SETTLEMENT));
        board.get_plot(OrderedPair(1, 5))->set_inhabitant(Building(blue, SETTLEMENT));

        board.get_path(OrderedPair(-1, -3), OrderedPair(-1, -4))->set_inhabitant(Road(blue));
        blue->add_initial_road_anchor(OrderedPair(-1, -3), OrderedPair(-1, -4));

        board.get_path(OrderedPair(1, 5), OrderedPair(1, 4))->set_inhabitant(Road(blue));
        blue->add_initial_road_anchor(OrderedPair(1, 5), OrderedPair(1, 4));
    }
}


void print_winner_score(const Board &board, const shared_ptr<Player> &current_player) {
    cout << current_player->get_name() << " WINS!!!" << endl;
    cout << "Here's the breakdown: " << endl;

    int num = current_player->get_number_of_placed_building(board, SETTLEMENT);
    cout << " - " << num << " Settlements: " << num << " Victory Points" << endl;

    num = current_player->get_number_of_placed_building(board, CITY);
    cout << " - " << num << " Cities: " << (num * 2) << " Victory Points" << endl;

    num = current_player->get_number_of_vp_devcards();
    cout << " - " << num << " Victory Point Cards: " << num << " Victory Points" << endl;

    if (current_player->has_largest_army()) {
        cout << " - Largest Army: 2 Victory Points" << endl;
    }

    if (current_player->has_longest_road()) {
        cout << " - Longest Road: 2 Victory Points" << endl;
    }

    cout << "\n" << current_player->get_name() << " total Victory Points: " << current_player->get_points(board) << endl;
}

void roll_dice(Board &board, shared_ptr<Player> current_player, map<int, shared_ptr<Player>> players, const int number_override) {
    int result = -1;
    if (number_override >= 2 && number_override <= 12) {
        result = number_override;
    } else {
        srand(time(nullptr));
        result = ((rand() % 6) + 1) + ((rand() % 6) + 1);
    }
    cout << "--------------------------------------------------------" << endl;
    cout << "\nThe Dice have rolled a " << result << "\n" << endl;
    if (result == 7) {
        for (auto player : players) {
            player.second->seven_rolled();
        }

        robber_movement_prompt(board, current_player, true);

    } else {
        board.generate_resources(result);
    }
}

void player_board_setup(Board &board, const map<int, shared_ptr<Player>> &players) {

    vector<shared_ptr<Player>> all_players;
    // Put players into a vector for easy forward and reverse iteration. I know there are other ways to do this, just don't know how
    for (auto &it : players) {
        all_players.push_back(it.second);
    }

    // First round of player placements
    for (int i = 0; i < all_players.size(); i++) {
        initial_player_setup(board, all_players[i], false);
    }
    // Second round of player placements
    for (int i = all_players.size() - 1; i >= 0; i--) {
        initial_player_setup(board, all_players[i], true);
    }
}