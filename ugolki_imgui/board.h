#pragma once

#include <vector>

#define BOARD_DIM    8
#define PAWNS_DIM    3

class board_c
{
public:
	bool pawns_1[BOARD_DIM][BOARD_DIM];
	bool pawns_2[BOARD_DIM][BOARD_DIM];
	char** map = nullptr;

	std::vector<std::pair<int, int>> pawns_moved;
	int F_i = -1, F_j = -1;
	int num_moves = 0;

	board_c();
	~board_c();
	
	void create_map();
	void del_map();
	void print_pawns();
	void set_finish(bool fixed);
	int path_len();
	void sync_map();
	void set_move1(int i0, int j0, int i1, int j1);
	void set_move2(int i0, int j0, int i1, int j1);
	void clean_path();
	bool maze_start_step(int& x, int& y);

	bool debut();
	bool move();
};

int maze_search(char**, int, int);
char** read_maze(int* rows, int* cols);
void print_maze(char** maze, int rows, int cols);
