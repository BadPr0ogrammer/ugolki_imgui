#include <algorithm>
#include <cmath>
#include <random>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>

#include "board.h"

board_c::board_c()
{
	for (int i = 0; i < BOARD_DIM; i++)
		for (int j = 0; j < BOARD_DIM; j++) {
			pawns_1[i][j] = i < PAWNS_DIM&& j < PAWNS_DIM;
			pawns_2[i][j] = i >= BOARD_DIM - PAWNS_DIM && j >= BOARD_DIM - PAWNS_DIM;
		}
	create_map();
}

board_c::~board_c()
{
	del_map();
}

void board_c::print_pawns()
{
	for (int i = 0; i < BOARD_DIM; i++) {
		for (int j = 0; j < BOARD_DIM; j++)
			std::cout << (pawns_1[i][j] || pawns_2[i][j] ? '#' : '.');
		std::cout << std::endl;
	}
}

void board_c::del_map()
{
	for (int i = 0; i < BOARD_DIM; i++)
		delete[] map[i];
	delete[] map;
	map = nullptr;
}

void board_c::sync_map()
{
	for (int i = 0; i < BOARD_DIM; i++) for (int j = 0; j < BOARD_DIM; j++)
		map[i][j] = pawns_1[i][j] || pawns_2[i][j] ? '#' : '.';
}

void board_c::create_map()
{
	delete map;
	map = new char* [BOARD_DIM];
	for (int i = 0; i < BOARD_DIM; i++)
		map[i] = new char[BOARD_DIM];
	sync_map();
}

void board_c::set_move1(int i0, int j0, int i1, int j1)
{
	pawns_1[i0][j0] = false;
	pawns_1[i1][j1] = true;
	print_pawns();
	std::cout << "(" << i0 << "," << j0 << ")" << "->(" << i1 << "," << j1 << ")" << "\n";
}

void board_c::set_move2(int i0, int j0, int i1, int j1)
{
	pawns_2[i0][j0] = false;
	pawns_2[i1][j1] = true;
	print_pawns();
	std::cout << "(" << i0 << "," << j0 << ")" << "->(" << i1 << "," << j1 << ")" << "\n";
}

void board_c::clean_path()
{
	for (int i = 0; i < BOARD_DIM; i++) for (int j = 0; j < BOARD_DIM; j++)
		if (map[i][j] == '*') 
			map[i][j] = '.';
}

bool board_c::debut()
{
	std::cout << "try debut\n";
	//std::srand(std::time(nullptr));
	for (int ii = 0; ii < 100; ii++) {
		int i, j;
		while (1) {
			i = (int)(PAWNS_DIM * ((double)std::rand() / RAND_MAX));
			j = (int)(PAWNS_DIM * ((double)std::rand() / RAND_MAX));
			if (pawns_1[i][j])
				break;
		}
		bool move_down = i + 1 >= 0 && i + 1 < BOARD_DIM && !pawns_1[i + 1][j] && !pawns_2[i + 1][j];
		bool move_right = j + 1 >= 0 && j + 1 < BOARD_DIM && !pawns_1[i][j + 1] && !pawns_2[i][j + 1];
		if (move_down) {
			set_move1(i, j, i + 1, j);
			return true;
		}
		if (move_right) {
			set_move1(i, j, i, j + 1);
			return true;
		}
	}
	return false;
}

bool board_c::maze_start_step(int &x, int &y)
{
	int i, j;
	for (i = 0; i < BOARD_DIM; i++) for (j = 0; j < BOARD_DIM; j++)
		if (map[i][j] == 'S')
			goto end;
end:
	if (i == BOARD_DIM || j == BOARD_DIM)
		return false;
	y = i;
	x = j;
	if (i < BOARD_DIM - 1 && map[i + 1][j] == '*')
		y++;
	else if (j < BOARD_DIM - 1 && map[i][j + 1] == '*')
		x++;
	else if (i > 0 && map[i - 1][j] == '*')
		y--;
	else if (j > 0 && map[i][j - 1] == '*')
		x--;
	return true;
}

void board_c::set_finish(bool fixed)
{
	static bool flag[PAWNS_DIM][PAWNS_DIM] = {
		{ false, false, false },
		{ false, false, false },
		{ false, false, false }
	};
	int mask_sz = num_moves < 20 ? 5 : num_moves < 30 ? 4 : num_moves < 35 ? 3 : 0;
	for (int i = 0; i < PAWNS_DIM; i++) for (int j = 0; j < mask_sz; j++)
		map[BOARD_DIM - 1 - i][BOARD_DIM - 1 - j] = '.';
	for (int j = 0; j < PAWNS_DIM; j++) for (int i = 0; i < mask_sz; i++)
		map[BOARD_DIM - 1 - i][BOARD_DIM - 1 - j] = '.';

	if (!fixed) {
		bool ret = false;
		for (int j = 0; j < PAWNS_DIM; j++) for (int i = 0; i < PAWNS_DIM; i++) {
			F_i = BOARD_DIM - 1 - i;
			F_j = BOARD_DIM - 1 - j;
			if (!flag[F_i][F_j] && !pawns_1[F_i][F_j] && !pawns_2[F_i][F_j]) {
				map[F_i][F_j] = 'F';
				flag[F_i][F_j] = true;
				return;
			}
		}
	}
	else {
		map[BOARD_DIM - 1][BOARD_DIM - 1] = 'F';
		F_i = BOARD_DIM - 1;
		F_j = BOARD_DIM - 1;
	}
	std::cout << "F=(" << F_i << "," << F_j << ")" << "\n";
}

int board_c::path_len()
{
	int len = 0;
	for (int i = 0; i < BOARD_DIM; i++) for (int j = 0; j < BOARD_DIM; j++)
		if (map[i][j] == '*')
			len++;
	return len;
}

bool board_c::move()
{
	std::cout << (num_moves < 10 ? "try mittelspiel" : "try endspiel") << " move N "<< num_moves << "\n";
	sync_map();
	set_finish(num_moves++ < 50);
	print_maze(map, BOARD_DIM, BOARD_DIM);
	
	int i1 = -1, j1 = -1, x0 = -1, y0 = -1;
	for (int m = 0; m < 500; m++) {
		int len = 10000;
		for (int i = 0; i < BOARD_DIM; i++) {
			for (int j = 0; j < BOARD_DIM; j++) {
				bool has_moved = false;
				if (pawns_moved.size() == PAWNS_DIM * PAWNS_DIM) {
					pawns_moved.clear();
					std::cout << "pawns_moved.clear\n";
				}
				else {
					auto it = std::find_if(pawns_moved.begin(), pawns_moved.end(), [=](const std::pair<int, int>& p) { return p == std::make_pair(i, j); });
					has_moved = it != pawns_moved.end();
				}
				if (!has_moved && pawns_1[i][j]) {
					if (abs(F_i - i) == 1 && abs(F_j - j) == 0 || abs(F_i - i) == 0 && abs(F_j - j) == 1) {
						i1 = i;
						j1 = j;
						y0 = F_i;
						x0 = F_j;
						goto end;
					}
					else {
						map[i][j] = 'S';
						if (maze_search(map, BOARD_DIM, BOARD_DIM) == 1) {
							std::cout << "search succeeded\n";
							int sz = path_len();
							int x, y;
							maze_start_step(x, y);
							if (y >= i && sz < len) {
								len = sz;
								i1 = i;
								j1 = j;
								y0 = y;
								x0 = x;
								std::cout << sz << "\n";
							}
						}
						else
							std::cout << "search failed!\n";
						print_maze(map, BOARD_DIM, BOARD_DIM);
						clean_path();
						map[i][j] = '#';
					}
				}
			}
		}
		if (x0 == -1 || y0 == -1 || i1 == -1 || j1 == -1) {
			i1 = j1 = x0 = y0 = -1;
			std::cout << "all search failed - repeat!\n";
			pawns_moved.clear();
		}
		else
			goto end;
	}
end:
	set_move1(i1, j1, y0, x0);
	pawns_moved.push_back(std::make_pair(y0, x0));
	if (y0 == 7 && x0 == 7) {
		print_maze(map, BOARD_DIM, BOARD_DIM);
	}
	std::cout << "moved\n";
	return true;
}
