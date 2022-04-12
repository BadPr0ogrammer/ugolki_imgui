#pragma once

#include <vector>

#define BOARD_DIM    8
#define PAWNS_DIM    3
#define DEBUT_MOVES  10

#define MIN(a, b) ((a)>(b)?(b):(a))
#define MAX(a, b) ((a)>(b)?(a):(b))

typedef struct {
	int i0;
	int j0;
	int i1;
	int j1;
	double len;
} move_t;

class maze_c
{
public:
	char** map = nullptr;
	maze_c();
	~maze_c();

	void sync(const int(&checkers)[BOARD_DIM][BOARD_DIM]);
	void clean_path();
	bool start_step(int& x, int& y);
	int path_len();
	std::vector<std::pair<int, int>> find_finish(bool one, const int(&checkers)[BOARD_DIM][BOARD_DIM], int ii, int jj);
	std::vector<move_t> search_moves(bool one, const int(&checkers)[BOARD_DIM][BOARD_DIM]);
	void set_move(int i0, int j0, int i1, int j1);
	void set_back(int i0, int j0, int i1, int j1);
};

class board_c
{
public:
	int checkers[BOARD_DIM][BOARD_DIM];

	board_c();
	
	void print_checkers();
	void set_move1(int i0, int j0, int i1, int j1);
	void set_move2(int i0, int j0, int i1, int j1);

	bool debut(bool allow_right, bool allow_left, bool allow_top);
	bool move();
};

int maze_search(char**, int, int);
char** read_maze(int* rows, int* cols);
void print_maze(char** maze, int rows, int cols);
