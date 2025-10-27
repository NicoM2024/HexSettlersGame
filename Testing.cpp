//
// Created by nmila on 9/28/2025.
//

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>

#include "HeaderFiles/Board.h"
#include "HeaderFiles/EndpointPair.h"
#include "HeaderFiles/OrderedPair.h"
#include "HeaderFiles/GameBank.h"
#include "HeaderFiles/Enums.h"

using std::cout, std::endl, std::shared_ptr, std::make_shared, std::string, std::vector, std::map;

bool test_Board();
bool test_DevCard();
bool test_GameBank();
bool test_Plot();
bool test_Path();
bool test_Player();
bool test_Tile();

int main() {
	if (test_Board()) { cout << "Passed all Board tests." << endl; }
	if (test_DevCard()) { cout << "Passed all DevCard tests." << endl; }
	if (test_GameBank()) { cout << "Passed all GameBank tests." << endl; }
	if (test_Plot()) { cout << "Passed all Plot tests." << endl; }
	if (test_Path()) { cout << "Passed all Path tests." << endl; }
	if (test_Player()) { cout << "Passed all Player tests." << endl; }
	if (test_Tile()) { cout << "Passed all Tile tests." << endl; }

	return 0;
}

bool test_Board() {
	bool passed = true;

	GameBank bank = GameBank::instance(4);
	GameBank::reset();
	Board board = Board();

	if (board.get_board_paths().size() != 72) {
		passed = false;
		cout << "FAILED BOARD PATH COUNT TEST" << endl;
	}

	if (board.get_board_plots().size() != 54) {
		passed = false;
		cout << "FAILED BOARD PLOT COUNT TEST" << endl;
	}

	if (board.get_board_tiles().size() != 3) {
		passed = false;
		cout << "FAILED BOARD TILE RINGS TEST" << endl;
	}

	int number_of_tiles = 0;
	for (auto &ring : board.get_board_tiles()) {
		number_of_tiles += ring.second.size();
	}
	if (number_of_tiles != 19) {
		passed = false;
		cout << "FAILED BOARD TILE COUNT TEST" << endl;
	}

	for (shared_ptr<Plot> plot : board.get_board_plots()) {
		if (!board.has_plot(plot->get_position_coordinates())) {
			passed = false;
			cout << "FAILED BOARD GET_PLOT TEST" << endl;
			break;
		}
	}

	for (shared_ptr<Plot> plot : board.get_board_plots()) {
		for (auto adj : plot->get_adjacent_plots()) {
			if (!board.has_path(plot->get_position_coordinates(), adj.second->get_position_coordinates())) {
				passed = false;
				cout << "FAILED BOARD GET_PATH WITH COORDS TEST" << endl;
				break;
			}
		}
	}

	for (shared_ptr<Plot> plot : board.get_board_plots()) {
		for (auto adj : plot->get_adjacent_plots()) {
			EndpointPair endpoint_pair(plot, adj.second);
			if (!board.has_path(endpoint_pair)) {
				passed = false;
				cout << "FAILED BOARD GET_PATH WITH ENDPOINT PAIR TEST" << endl;
				break;
			}
		}
	}

	for (shared_ptr<Path> path : board.get_board_paths()) {
		if (!board.has_path(path->get_endpoints().plot_a, path->get_direction()) ||
			!board.has_path(path->get_endpoints().plot_b, path->get_direction())) {
			passed = false;
			cout << "FAILED BOARD GET_PATH WITH PLOT AND DIRECTION TEST" << endl;
			break;
		}
	}

	for (auto ring : board.get_board_tiles()) {
		for (auto tile : ring.second) {
			if (!board.has_tile(tile.second.get_position_coordinate())) {
				passed = false;
				cout << "FAILED BOARD GET_TILE WITH COORDINATE TEST" << endl;
				break;
			}
		}
	}

	for (auto ring : board.get_board_tiles()) {
		for (auto tile : ring.second) {
			if (!board.has_tile(tile.second.get_position_id())) {
				passed = false;
				cout << "FAILED BOARD GET_TILE WITH STRING ID TEST" << endl;
				break;
			}
		}
	}

	for (auto ring : board.get_board_tiles()) {
		for (auto tile : ring.second) {
			if (board.get_tile_position_coordinates(tile.second.get_position_id()) !=
				tile.second.get_position_coordinate()) {
				passed = false;
				cout << "FAILED BOARD GET COORD FROM STRING ID TEST" << endl;
				break;
			}
		}
	}

	OrderedPair robber_position = OrderedPair(-100, -100);
	for (auto ring : board.get_board_tiles()) {
		for (auto tile : ring.second) {
			if (tile.second.has_robber()) {
				robber_position = tile.second.get_position_coordinate();
				break;
			}
		}
		if (robber_position != OrderedPair(-100, -100)) {
			break;
		}
	}
	if (board.get_tile_with_robber().get_position_coordinate() != robber_position) {
		passed = false;
		cout << "FAILED BOARD GET TILE WITH ROBBER TEST" << endl;
	}

	// This just makes sure the robber moves to a new spot for next test
	if (board.get_tile_with_robber().get_position_id() == "c,d") {
		board.set_robber_position("a,a");
	} else {
		board.set_robber_position("c,d");
	}

	robber_position = OrderedPair(-100, -100);
	for (auto ring : board.get_board_tiles()) {
		for (auto tile : ring.second) {
			if (tile.second.has_robber()) {
				robber_position = tile.second.get_position_coordinate();
				break;
			}
		}
		if (robber_position != OrderedPair(-100, -100)) {
			break;
		}
	}
	if (board.get_tile_with_robber().get_position_coordinate() != robber_position) {
		passed = false;
		cout << "FAILED BOARD SET_ROBBER_POSITION TEST" << endl;
	}

	Board board2 = Board();
	vector<int> prod_nums = {1, 2, 3, 4, 5};
	int num = board2.get_random_production_number(prod_nums);
	if (!(num > 0 && num <= 5)) {
		passed = false;
		cout << "FAILED BOARD GET RANDOM PRODUCTION NUMBER TEST 1" << endl;
	}
	if (prod_nums.size() != 4) {
		passed = false;
		cout << "FAILED BOARD GET RANDOM PRODUCTION NUMBER TEST 2" << endl;
	}

	shared_ptr<Path> p1 = board2.get_path(OrderedPair(-1, 0), OrderedPair(-1, 1));
	if (board.can_place_road(bank.get_player(RED))) {
		passed = false;
		cout << "FAILED BOARD CAN PLACE ROAD TEST 1" << endl;
	}
	p1->set_inhabitant(Road(bank.get_player(RED)));
	if (!board2.can_place_road(bank.get_player(RED))) {
		passed = false;
		cout << "FAILED BOARD CAN PLACE ROAD TEST 2" << endl;
	}

	map<tile_type, int> tile_types = {{GRAIN, 2}, {WOOL, 2}, {ORE, 2}};
	tile_type choice = board2.get_random_tile_type(tile_types);
	if (!(choice == GRAIN || choice == WOOL || choice == ORE)) {
		passed = false;
		cout << "FAILED BOARD GET RANDOM TILE TYPE TEST 1" << endl;
	}
	if (tile_types.at(GRAIN) + tile_types.at(WOOL) + tile_types.at(ORE) != 5) {
		passed = false;
		cout << "FAILED BOARD GET RANDOM TILE TYPE TEST 2" << endl;
	}

	return passed;
}

bool test_DevCard() {
	bool passed = true;

	// Cannot test these methods because they are completely reliant on player input.
	// test_playability
	// play

	DevCard vp(VICTORY_POINT);
	DevCard vp2(VICTORY_POINT);
	DevCard rb(ROAD_BUILDING);
	DevCard yop(YEAR_OF_PLENTY);
	DevCard yop2(YEAR_OF_PLENTY);
	DevCard mnply(MONOPOLY);
	DevCard kn(KNIGHT);

	if (kn > rb) {
		passed = false;
		cout << "FAILED DEVCARD > TEST" << endl;
	}
	if (yop < rb) {
		passed = false;
		cout << "FAILED DEVCARD < TEST" << endl;
	}
	if (yop == mnply) {
		passed = false;
		cout << "FAILED DEVCARD == TEST" << endl;
	}
	if (yop != yop) {
		passed = false;
		cout << "FAILED DEVCARD != TEST" << endl;
	}
	if (vp != vp2) {
		passed = false;
		cout << "FAILED DEVCARD != TEST" << endl;
	}
	if (!(yop == yop2)) {
		passed = false;
		cout << "FAILED DEVCARD == TEST" << endl;
	}

	return passed;
}

bool test_GameBank() {
	bool passed = true;

	GameBank& bank = GameBank::instance(4);
	bank.get_player(RED)->set_length_of_road(8, false);
	bank.draw_devcard();
	GameBank::reset();
	if (bank.get_player(RED)->get_length_of_road() == 8) {
		passed = false;
		cout << "FAILED GAMEBANK RESET TEST 1" << endl;
	}
	bank.get_player(RED)->set_length_of_road(0, false);

	if (bank.get_available_devcards().size() != 25) {
		passed = false;
		cout << "FAILED GAMEBANK DEVCARD DECK AND RESET TEST" << endl;
	}
	bank.draw_devcard();
	if (bank.get_available_devcards().size() != 24) {
		passed = false;
		cout << "FAILED GAMEBANK DRAW DEVCARD TEST" << endl;
	}

	for (tile_type res : {GRAIN, WOOL, BRICK, ORE, LUMBER}) {
		if (!bank.has_resource(res)) {
			passed = false;
			cout << "FAILED GAMEBANK RESOURCE COLLECTION TEST" << endl;
			break;
		}
	}

	for (tile_type res : {GRAIN, WOOL, BRICK, ORE, LUMBER}) {
		if (!bank.resource_exists(res)) {
			passed = false;
			cout << "FAILED GAMEBANK RESOURCE EXISTS TEST" << endl;
			break;
		}
	}

	bank.draw_resource(GRAIN);
	if (bank.get_available_resources().at(GRAIN) != 18) {
		passed = false;
		cout << "FAILED GAMEBANK DRAW RESOURCE TEST" << endl;
	}
	bank.return_resource(GRAIN);
	if (bank.get_available_resources().at(GRAIN) != 19) {
		passed = false;
		cout << "FAILED GAMEBANK RETURN RESOURCE TEST" << endl;
	}
	bank.return_resource(WOOL);
	if (bank.get_available_resources().at(GRAIN) != 19) {
		passed = false;
		cout << "FAILED GAMEBANK RETURN RESOURCE WHEN RESOURCE IS FULL TEST" << endl;
	}

	if (bank.get_players().size() != 4) {
		passed = false;
		cout << "FAILED GAMEBANK PLAYER SETUP TEST" << endl;
	}

	if (bank.get_player(RED) == nullptr) {
		passed = false;
		cout << "FAILED GAMEBANK GETPLAYER WITH COLOR TEST" << endl;
	}
	if (bank.get_player("RED") == nullptr) {
		passed = false;
		cout << "FAILED GAMEBANK GETPLAYER WITH STRING TEST" << endl;
	}
	if (bank.get_player(1) == nullptr) {
		passed = false;
		cout << "FAILED GAMEBANK GETPLAYER WITH INT TEST" << endl;
	}

	if (bank.get_longest_road_player() != nullptr) {
		passed = false;
		cout << "FAILED GAMEBANK GET LONGEST ROAD PLAYER TEST" << endl;
	}
	if (bank.get_largest_army_player() != nullptr) {
		passed = false;
		cout << "FAILED GAMEBANK GET LARGEST ARMY PLAYER TEST" << endl;
	}
	bank.get_player(RED)->set_largest_army(true);
	bank.get_player(YELLOW)->set_longest_road(true);
	if (bank.get_longest_road_player() != bank.get_player(YELLOW)) {
		passed = false;
		cout << "FAILED GAMEBANK GET LONGEST ROAD PLAYER TEST" << endl;
	}
	if (bank.get_largest_army_player() != bank.get_player(RED)) {
		passed = false;
		cout << "FAILED GAMEBANK GET LARGEST ARMY PLAYER TEST" << endl;
	}

	bank.get_player(RED)->set_largest_army(false);
	bank.get_player(YELLOW)->set_longest_road(false);

	return passed;
}

bool test_Path() {
	bool passed = true;

	shared_ptr<Plot> endpointa = make_shared<Plot>(OrderedPair(-1, 0));
	shared_ptr<Plot> endpointb = make_shared<Plot>(OrderedPair(-1, 1));
	shared_ptr<Plot> endpointc = make_shared<Plot>(OrderedPair(1, 1));
	shared_ptr<Plot> endpointd = make_shared<Plot>(OrderedPair(1, 0));

	/* Plots A, B, C, D
	*  Paths 1, 2, 3
	*
	*      B --2-- C
	*     1         3
	*    A    0,0    D
	*/

	Path path1 = Path(EndpointPair(endpointa, endpointb), OrderedPair(0, 0));
	Path path2 = Path(EndpointPair(endpointb, endpointc), OrderedPair(0, 0));
	Path path3 = Path(EndpointPair(endpointc, endpointd), OrderedPair(0, 0));

	if (path1.get_icon() != "/" || path2.get_icon() != "-----" || path3.get_icon() != "\\") {
		passed = false;
		cout << "FAILED PATH GET ICON TEST" << endl;
	}

	path1.set_icon_override("S");
	if (path1.get_icon() != "S") {
		passed = false;
		cout << "FAILED PATH OVERRIDE ICON TEST" << endl;
	}
	path2.set_icon_override("S");
	if (path2.get_icon() != "- S -") {
		passed = false;
		cout << "FAILED PATH OVERRIDE ICON HORIZONTAL TEST" << endl;
	}

	path2.set_inhabitant(Road(GameBank::instance(4).get_player(RED)));
	if (!path2.is_occupied()) {
		passed = false;
		cout << "FAILED PATH OCCUPIED TEST" << endl;
	}
	path2.set_icon_override("");

	if (path3.calculate_direction(
		endpointc->get_position_coordinates(),
		endpointd->get_position_coordinates(),
		OrderedPair(0, 0)) != -1) {
		passed = false;
		cout << "FAILED PATH CALCULATING DIRECTION TEST" << endl;
	}

	Board board = Board();
	// pic -> path in context
	shared_ptr<Path> pic1 = board.get_path(board.get_plot(OrderedPair(-1, 0)), 1);
	shared_ptr<Path> pic2 = board.get_path(board.get_plot(OrderedPair(-1, 0)), -1);
	shared_ptr<Path> pic3 = board.get_path(board.get_plot(OrderedPair(-1, 0)), 0);
	if (pic1->has_adjacent_matching_road(RED)) {
		passed = false;
		cout << "FAILED PATH ADJACENT MATCHING ROAD TEST 1" << endl;
	}
	pic1->set_inhabitant(Road(GameBank::instance().get_player(RED)));
	pic2->set_inhabitant(Road(GameBank::instance().get_player(YELLOW)));
	if (pic1->has_adjacent_matching_road(RED)) {
		passed = false;
		cout << "FAILED PATH ADJACENT MATCHING ROAD TEST 2" << endl;
	}
	pic3->set_inhabitant(Road(GameBank::instance().get_player(RED)));
	if (!pic1->has_adjacent_matching_road(RED)) {
		passed = false;
		cout << "FAILED PATH ADJACENT MATCHING ROAD TEST 3" << endl;
	}

	return passed;
}

bool test_Plot() {
	bool passed = true;

	Plot plot1(OrderedPair(-1, 0));

	if (plot1.get_icon() != " . ") {
		passed = false;
		cout << "FAILED PLOT ICON TEST" << endl;
	}

	plot1.set_icon_override("S");
	if (plot1.get_icon() != " S "){
		passed = false;
		cout << "FAILED PLOT ICON OVERRIDE TEST" << endl;
	}

	Board board = Board();
	shared_ptr<Plot> pic1 = board.get_plot(OrderedPair(-1, 0));
	shared_ptr<Plot> pic2 = board.get_plot(OrderedPair(-1, 1));

	pic1->set_inhabitant(Building(GameBank::instance().get_player(RED), SETTLEMENT));
	if (!pic2->next_to_building()) {
		passed = false;
		cout << "FAILED PLOT NEXT TO BUILDING TEST 1" << endl;
	}
	if (pic1->next_to_building()) {
		passed = false;
		cout << "FAILED PLOT NEXT TO BUILDING TEST 2" << endl;
	}

	for (OrderedPair p : pic1->get_adjacent_tile_coords()) {
		if (!board.has_tile(p)) {
			passed = false;
			cout << "FAILED PLOT ADJACENT TILE COORD TEST" << endl;
			break;
		}
	}

	return passed;
}

bool test_Player() {
	bool passed = true;

	GameBank bank = GameBank::instance(4);
	Board board = Board();
	shared_ptr<Player> red = bank.get_player(RED);
	shared_ptr<Player> yellow = bank.get_player(YELLOW);

	if (red->get_points(board, true) != 0) {
		passed = false;
		cout << "FAILED PLAYER INITIAL POINTS TEST: " << red->get_points(board, true) << endl;
	}
	board.get_plot(OrderedPair(-1, 0))->set_inhabitant(Building(red, SETTLEMENT));
	if (red->get_points(board, true) != 1) {
		passed = false;
		cout << "FAILED PLAYER POINTS TEST" << endl;
	}
	if (red->get_number_of_placed_building(board, SETTLEMENT) != 1) {
		passed = false;
		cout << "FAILED PLAYER NUMBER OF PLACED BUILDING TEST" << endl;
	}

	board.get_path(OrderedPair(-1, 0), OrderedPair(-1, 1))->set_inhabitant(Road(red));
	red->add_initial_road_anchor(OrderedPair(-1, 0), OrderedPair(-1, 1));
	board.get_path(OrderedPair(-1, 1), OrderedPair(1, 1))->set_inhabitant(Road(red));
	if (red->get_number_of_placed_roads(board) != 2) {
		passed = false;
		cout << "FAILED PLAYER NUMBER OF ROADS PLACED TEST" << endl;
	}
	red->set_length_of_road(5, false);
	if (!red->has_longest_road()) {
		passed = false;
		cout << "FAILED PLAYER LONGEST ROAD TEST" << endl;
	}
	red->set_length_of_road(5, false);
	if (yellow->has_longest_road()) {
		passed = false;
		cout << "FAILED PLAYER LONGEST ROAD OVERTAKE TEST 1" << endl;
	}
	yellow->set_length_of_road(6, false);
	if (!yellow->has_longest_road()) {
		passed = false;
		cout << "FAILED PLAYER LONGEST ROAD OVERTAKE TEST 2" << endl;
	}

	board.get_path(OrderedPair(1, 0), OrderedPair(3, 0))->set_inhabitant(Road(red));
	red->add_initial_road_anchor(OrderedPair(1, 0), OrderedPair(3, 0));
	if (red->calculate_length_of_road(board) != 2) {
		passed = false;
		cout << "FAILED PLAYER ROAD CALCULATION TEST 1: " << red->calculate_length_of_road(board) << endl;
	}
	board.get_path(OrderedPair(1, 1), OrderedPair(1, 0))->set_inhabitant(Road(red));
	if (red->calculate_length_of_road(board) != 4) {
		passed = false;
		cout << "FAILED PLAYER ROAD CALCULATION TEST 2: " << red->calculate_length_of_road(board) << endl;
	}

	red->gain_many_resources({{GRAIN, 1}, {WOOL, 2}, {ORE, 1}}, true, false);
	if (red->get_number_of_resources() != 4) {
		passed = false;
		cout << "FAILED PLAYER RESOURCE COUNT TEST" << endl;
	}
	if (!red->has_resource(GRAIN)) {
		passed = false;
		cout << "FAILED PLAYER HAS RESOURCE TEST" << endl;
	}
	if (red->get_amount_of_resource(GRAIN) != 1 ||
		red->get_amount_of_resource(WOOL) != 2 ||
		red->get_amount_of_resource(ORE) != 1) {
		passed = false;
		cout << "FAILED PLAYER AMOUNT OF SPECIFIC RESOURCE TEST" << endl;
	}
	if (red->can_afford(Craftable::get_recipe("settlement"), false)) {
		passed = false;
		cout << "FAILED PLAYER CAN AFFORD TEST 1" << endl;
	}
	if (!red->can_afford(Craftable::get_recipe("devcard"), false)) {
		passed = false;
		cout << "FAILED PLAYER CAN AFFORD TEST 2" << endl;
	}
	red->discard_many_resources({{GRAIN, 1}, {WOOL, 1}}, "", true, false);
	if (red->get_number_of_resources() != 2) {
		passed = false;
		cout << "FAILED PLAYER DISCARD MANY RESOURCES TEST" << endl;
	}

	red->gain_devcard(VICTORY_POINT, false);
	if (red->get_number_of_vp_devcards() != 1) {
		passed = false;
		cout << "FAILED PLAYER VP DEVCARD COUNT TEST" << endl;
	}
	red->gain_devcard(KNIGHT, false);
	red->gain_devcard(DevCard(KNIGHT), false);
	if (red->get_devcards().size() != 3) {
		passed = false;
		cout << "FAILED PLAYER DEVCARD COUNT TEST" << endl;
	}
	if (red->has_devcard(YEAR_OF_PLENTY)) {
		passed = false;
		cout << "FAILED PLAYER HAS DEVCARD TEST 1" << endl;
	}
	if (!red->has_devcard(KNIGHT)) {
		passed = false;
		cout << "FAILED PLAYER HAS DEVCARD TEST 2" << endl;
	}

	if (!red->get_ports(board).empty() || red->has_universal_port(board)) {
		passed = false;
		cout << "FAILED PLAYER INITIAL PORTS TEST" << endl;
	}

	board.get_plot(OrderedPair(1, 5))->set_inhabitant(Building(red, SETTLEMENT));
	if (red->get_ports(board).size() != 1 && !red->has_universal_port(board)) {
		passed = false;
		cout << "FAILED PLAYER PORTS TEST" << endl;
	}
	board.get_plot(OrderedPair(3, 3))->set_inhabitant(Building(red, SETTLEMENT));
	board.get_plot(OrderedPair(5, 0))->set_inhabitant(Building(red, SETTLEMENT));
	if (!red->has_universal_port(board)) {
		passed = false;
		cout << "FAILED PLAYER UNIVERSAL PORTS TEST" << endl;
	}

	return passed;
}

bool test_Tile() {
	bool passed = true;

	Board board = Board();
	GameBank bank = GameBank::instance();
	GameBank::reset();

	shared_ptr<Player> player = GameBank::instance().get_player(RED);
	shared_ptr<Plot> plot = board.get_plot(OrderedPair(-1, 0));
	Tile& tic1 = board.get_tile(OrderedPair(0, 0));

	tic1.override_plot_icons(board, RED, false);
	for (auto &p : tic1.get_adjacent_plots()) {
		if (p.second->get_icon() == " . ") {
			passed = false;
			cout << "FAILED PLOT ICON OVERRIDE TEST" << endl;
			break;
		}
	}
	tic1.reset_plot_icon_overrides();
	for (auto &p : tic1.get_adjacent_plots()) {
		if (p.second->get_icon() != " . ") {
			passed = false;
			cout << "FAILED PLOT RESET ICON OVERRIDE TEST" << endl;
			break;
		}
	}

	tic1.override_path_icons(RED, false);
	for (auto &p : tic1.get_adjacent_paths()) {
		string icon = p.second->get_icon();
		if (icon == "-----" || icon == "\\" || icon == "/") {
			passed = false;
			cout << "FAILED PATH ICON OVERRIDE TEST" << endl;
			break;
		}
	}
	tic1.reset_path_icon_overrides();
	for (auto &p : tic1.get_adjacent_paths()) {
		string icon = p.second->get_icon();
		if (!(icon == "-----" || icon == "\\" || icon == "/")) {
			passed = false;
			cout << "FAILED RESET PATH ICON OVERRIDE TEST" << endl;
			break;
		}
	}

	plot->set_inhabitant(Building(player, SETTLEMENT));
	string old_icon = plot->get_icon();
	tic1.override_settlement_icons(RED);
	if (plot->get_icon() == old_icon) {
		passed = false;
		cout << "FAILED OVERRIDE SETTLEMENT ICON TEST" << endl;
	}
	tic1.reset_plot_icon_overrides();

	for (OrderedPair p : plot->get_adjacent_tile_coords()) {
		if (board.has_tile(p)) {
			board.get_tile(p).produce_resource(false);
		}
	}
	if (player->get_number_of_resources() <= 0) {
		passed = false;
		cout << "FAILED TILE PRODUCE RESOURCES TEST" << endl;
	}

	Tile tile = Tile(ORE, 6, "a,a", OrderedPair(0, 0), {}, {});

	if (tile.get_main_display_row(false).size() != 9) {
		passed = false;
		cout << "FAILED MAIN DISPLAY ROW SIZE TEST" << endl;
	}
	if (tile.get_robber_display_row().size() != 9) {
		passed = false;
		cout << "FAILED ROBBER DISPLAY ROW SIZE TEST" << endl;
	}
	if (tile.get_top_display_row().size() != 9) {
		passed = false;
		cout << "FAILED TOP DISPLAY ROW SIZE TEST" << endl;
	}

	return passed;
}