/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 03/01/2021 at 03:18 PM
 * 
 * Connect_Four.cc
 * 
 * -----------------------------------------------------------------------------
 */

#include <iostream>
#include <array>
#include <set>
#include <tuple>
using namespace std;

//------------------------------------------------------------------------------
// Constant values
//------------------------------------------------------------------------------
const int max_rows = 6;
const int max_columns = 7;
const char default_tile_char = '!';
const char default_player_char = 'P';
const int max_term_length = 80;
//------------------------------------------------------------------------------
// Tile Class
//------------------------------------------------------------------------------

class Tile {
public:
	Tile() =default;
	explicit Tile(char cha): ch{cha} { }	
	[[nodiscard]] char get_tile_character() const { return ch; }
	void set_tile_character(char ch) { this->ch = ch; }
	[[nodiscard]] bool is_title_character(char newch) const { return ch == newch; }

	// Function to check if a tile has been set by a player.
	bool is_full() { return selected; }
	void set_full() { selected = true; }
	void set_empty() { selected = false; }
                       
private:
	char ch{default_tile_char};
	void clear_screen();
	bool selected{false}; // Determines if the player has placed a piece there.

};



unsigned int get_column_size(array <array <Tile, max_columns >, max_rows > &tilem,
							 int column);
//------------------------------------------------------------------------------
// Map 
//------------------------------------------------------------------------------
std::array<std::array<Tile, max_columns>, max_rows> tile_map;
void draw_tile_map(std::array<std::array<Tile, max_columns>, max_rows> &tilem) {
	for (auto it = tilem.rbegin(); it != tilem.rend(); it++) {
		for (auto &col: *it)
			cout << col.get_tile_character() << "  ";
		cout << endl;
	}	
}


//------------------------------------------------------------------------------
// Tile_Tuple and Compare function
//------------------------------------------------------------------------------
using Tile_Tuple = std::tuple<int, int>;

struct Tuple_Comparator {
	Tuple_Comparator() = default;
	bool operator()(const Tile_Tuple &a, const Tile_Tuple &b) const {
		// Compare each x and then y value 
		if (std::get<0>(a) == std::get<0>(b))
			return std::get<1>(a) < std::get<1>(b);
		else
			return std::get<0>(a) < std::get<0>(b);
	}
};

//------------------------------------------------------------------------------
// Player Class
//------------------------------------------------------------------------------
class Player {
public:
	Player() =default;
	explicit Player(char pc) : player_char{pc} { }
	Player(const Player &p); // Copy Constructor
	void add_tuple(Tile_Tuple t);
	void add_tuple(int row, int column);
	bool check_win_condition();
	[[nodiscard]] char get_player_token() const { return player_char; }
	void set_player_token(char ch) { player_char = ch; }
	void increment_turn() { term_num++; }
private:
	char player_char{default_player_char};
	int term_num{0};
	// Contains all pieces placed by a player in
	set<Tile_Tuple, Tuple_Comparator> piece_set;
	bool check_vertical();
	bool check_horizontal();

	bool check_upper_diagonal();
	bool check_lower_diagonal();
	bool check_diagonal();
};


Player::Player(const Player &p) {
	this->player_char = p.player_char;
	this->piece_set = p.piece_set;
}

void Player::add_tuple(Tile_Tuple t) { piece_set.insert(t); }
void Player::add_tuple(int row, int column) {
	piece_set.insert(make_tuple(row, column));
}

/**
 * Check if the player has won the game.
 */
bool Player::check_win_condition() {
	bool check_win;
	check_win = check_vertical() or check_diagonal() or check_horizontal();

	return check_win;
}

/**
 * Check if the player has a vertical row of 4 of his own pieces.
 */
bool Player::check_vertical() {
	// Check if the x component is the same for four pieces.
	int x_val = std::get<0>(*piece_set.begin());
	
	int vertical_count = 1;
		
	for (auto i = ++piece_set.begin(); i != piece_set.end(); i++) {
		int temp_y = std::get<0>(*i);
		
		if (temp_y == x_val)
			vertical_count++;
		else {
			x_val = temp_y;
			vertical_count = 1;
		}
	}
	
	return (vertical_count == 4);
}

/**
 * Check if the player has a horizontal row of 4 of his own pieces.
 */
bool Player::check_horizontal() {
	int horizontal_count{};
	int x_val, y_val;
	Tile_Tuple temp_tuple;
	for (auto &piece : piece_set) {
		temp_tuple = piece;
		x_val = std::get<0>(piece);
		y_val = std::get<1>(piece);
		horizontal_count = 1;

		for (int i = 1; i < 4; i++) {
			std::get<0>(temp_tuple) = x_val + i;
			auto check = piece_set.find(temp_tuple);
			if (check != piece_set.end()) {
				horizontal_count++;
				if (horizontal_count == 4)
					return true;
			}
			else break;
		}
	}

	return false;
}


/**
 * Check if the player has a upper slope or positive diagonal row of 4 of his
 * own pieces.
 * @return A boolean value
 */
bool Player::check_upper_diagonal() {
	int x_val;
	int y_val;
	
	int diagonal_count;
	for (auto &tuple : piece_set) {
		// Check the map if a diagonal line starting at tuple exists
		diagonal_count = 1;
		x_val = std::get<0>(tuple);
		y_val = std::get<1>(tuple);
		Tile_Tuple temp_tuple = tuple;

		for (int j = 1; j < 4; j++) {
			std::get<0>(temp_tuple) = x_val + j;
			std::get<1>(temp_tuple) = y_val + j;
			//auto check = piece_set.find(make_tuple(x_val + j, y_val + j));
			auto check = piece_set.find(temp_tuple);
			if (check != piece_set.end()) {
				diagonal_count++;
				if (diagonal_count == 4) return true;
			}
			else break;
		}
	}

	return false;
}

/**
 * Check if the player has a negative slope or lower diagonal row of 4 of his
 * own pieces.
 * @return A boolean value
 */
bool Player::check_lower_diagonal() {
	int x_val{};
	int y_val{};

	int diagonal_count{};
	for (auto &tuple : piece_set) {
		diagonal_count = 1;
		x_val = std::get<0>(tuple);
		y_val = std::get<1>(tuple);
		Tile_Tuple temp_tuple = tuple;

		for (int j = 1; j < 4; j++) {
			std::get<0>(temp_tuple) = x_val + j;
			std::get<1>(temp_tuple) = y_val - j;
			auto check = piece_set.find(temp_tuple);
			//auto check = piece_set.find(make_tuple(x_val + j, y_val - j));
			if (check != piece_set.end()) {
				diagonal_count++;
				if (diagonal_count == 4) return true;
			}
			else
				break;
		}
	}
	
	return false;
}

/**
 * Check if the player has a diagonal row of 4 of his own pieces.
 */
bool Player::check_diagonal() {
	// Check for positive or negative slopes:
	return check_upper_diagonal() || check_lower_diagonal();
}

/**
 * Get the size of a column.
 */
unsigned int get_column_size(array < array < Tile, max_columns >, max_rows > &tilem,
							 int column) {
	unsigned length = 0;
	for (auto & i : tilem)
		if (i.at(column).is_full())
			length++;

	return length;
}

//------------------------------------------------------------------------------
// Misc Functions
//------------------------------------------------------------------------------
void print_dash_line() {
	for (int i = 0; i < max_term_length; i++)
		cout << '-';
	cout << endl;
}

//------------------------------------------------------------------------------
// Turn functions
//------------------------------------------------------------------------------

void clear_screen() {
	// Clear screen and print the current board:
    // Windows Version
#if defined(WIN32) || defined(_WIN32)
    system("cls");
#else // Linux/POSIX
    system("clear");
#endif
	
}

void player_turn(array<array<Tile, max_columns >, max_rows > &tilem, Player & p) {

	clear_screen();
	
	char pch = p.get_player_token();
	print_dash_line();
	cout << "It is now Player " << pch << "'s turn.\n";

	array<array<Tile, max_columns>, max_rows> &array_ref = tile_map;

	draw_tile_map(tilem);
	print_dash_line();
	cout << "Please choose a column to place a piece."
		 << "\nMake sure that the column is not full and is between 1 and 7."
		 << endl;

	int column;
	bool check_range;
	int column_size; // Basically the row you are going to use
input:	// Disgusting label
	cin >> column;
	check_range = (1 <= column && column <= max_columns);


	while (!check_range) {
		cerr << "Column " << column << " is not a valid column between 1 and 7. "
			 << "Try again.\n";

		cin >> column;
		check_range = (1 <= column && column <= max_columns);
	}

	column_size = get_column_size(tilem, column - 1);
	if (column_size == max_rows) {
		cout << "Column " << column << " is full. Try again.\n";
		goto input; // Disgusting GOTO but it works
	}

done:
	// If everything went well, set the tile and add its index to the map.
	tilem.at(column_size).at(column - 1).set_full();
	tilem.at(column_size).at(column - 1).set_tile_character(pch);
	p.add_tuple(column - 1, column_size);
	p.increment_turn();
	// Now check winning conditions:

	bool check_win = p.check_win_condition();
	if (check_win) {
		draw_tile_map(tilem);
		cout << "Player " << pch << " wins!\n";
		exit(EXIT_SUCCESS);
	}
}

int main(void) {
	Player player1{'@'};
	Player player2{'$'};
	while (true) {
		player_turn(tile_map, player1);
		player_turn(tile_map, player2);
	}
	// debug();	
}
