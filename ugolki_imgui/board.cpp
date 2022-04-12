#include <algorithm>
#include <cmath>
#include <random>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>

#include "board.h"

maze_c::~maze_c()
{
	for (int i = 0; i < BOARD_DIM; i++)
		delete[] map[i];
	delete[] map;
	map = nullptr;
}

maze_c::maze_c()
{
	map = new char* [BOARD_DIM];
	for (int i = 0; i < BOARD_DIM; i++)
		map[i] = new char[BOARD_DIM];
}

void maze_c::sync(const int(&checkers)[BOARD_DIM][BOARD_DIM])
{
	for (int i = 0; i < BOARD_DIM; i++) for (int j = 0; j < BOARD_DIM; j++)
		map[i][j] = checkers[i][j] != 0 ? '#' : '.';
}

void maze_c::clean_path()
{
	for (int i = 0; i < BOARD_DIM; i++) for (int j = 0; j < BOARD_DIM; j++)
		if (map[i][j] == '*')
			map[i][j] = '.';
}

bool maze_c::start_step(int& x, int& y)
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

int maze_c::path_len()
{
	int len = 0;
	for (int i = 0; i < BOARD_DIM; i++) for (int j = 0; j < BOARD_DIM; j++)
		if (map[i][j] == '*')
			len++;
	return len;
}

void maze_c::set_move(int i0, int j0, int i1, int j1)
{
	map[i0][j0] = '.';
	map[i1][j1] = '#';
	print_maze(map, BOARD_DIM, BOARD_DIM);
}

void maze_c::set_back(int i0, int j0, int i1, int j1)
{
	map[i0][j0] = '#';
	map[i1][j1] = '.';
}

std::vector<std::pair<int, int>> maze_c::find_finish(bool one, const int(&checkers)[BOARD_DIM][BOARD_DIM], int ii, int jj)
{
	// TODO: make target tangent 
	std::vector<std::pair<int, int>> fv;
	if (one) {
		int d0 = MIN(3, BOARD_DIM - 1 - ii);
		int d1 = MIN(2, BOARD_DIM - 1 - jj);
		for (int i = 0; i < d0; i++)
			for (int j = 0; j < d1; j++)
			if (!checkers[ii + i][jj + j])
				fv.push_back(std::make_pair(ii + i, jj + j));
	}
	else {
		int d0 = MIN(3, ii);
		int d1 = MIN(2, jj);
		for (int i = 0; i < d0; i++)
			for (int j = 0; j < d1; j++)
			if (!checkers[ii - i][jj - j])
				fv.push_back(std::make_pair(ii - i, jj - j));
	}
	return fv;
}

std::vector<move_t> maze_c::search_moves(bool one, const int(&checkers)[BOARD_DIM][BOARD_DIM])
{
	sync(checkers);
	std::vector<move_t> moves;
	for (int i = 0; i < BOARD_DIM; i++) {
		for (int j = 0; j < BOARD_DIM; j++) {
			if (one && checkers[i][j] == 1 || !one && checkers[i][j] == -1) {
				std::vector<std::pair<int, int>> fv = find_finish(one, checkers, i, j);
				for (auto& fin : fv) {
					bool failed = false;
					int F_i = fin.first;
					int F_j = fin.second;
					if (abs(F_i - i) == 1 && abs(F_j - j) == 0 || abs(F_i - i) == 0 && abs(F_j - j) == 1) {
						move_t move;
						move.i0 = i;
						move.j0 = j;
						move.i1 = F_i;
						move.j1 = F_j;
						move.len = 1;
						moves.push_back(move);
					}
					else {
						map[F_i][F_j] = 'F';
						map[i][j] = 'S';
						if (maze_search(map, BOARD_DIM, BOARD_DIM) == 1) {
							std::cout << "search succeeded\n";
							int sz = path_len();
							int x, y;
							if (start_step(x, y)) {
								move_t move;
								move.len = sz;
								move.i0 = i;
								move.j0 = j;
								move.i1 = y;
								move.j1 = x;
								moves.push_back(move);
								std::cout << sz << "\n";
							}
						}
						else {
							std::cout << "search failed!\n";
							failed = true;
						}
						print_maze(map, BOARD_DIM, BOARD_DIM);
						clean_path();
						map[i][j] = '#';
						map[F_i][F_j] = '.';
					}
					if (!failed)
						break;
				}
			}
		}
	}
	return moves;
}


board_c::board_c()
{
	for (int i = 0; i < BOARD_DIM; i++)
		for (int j = 0; j < BOARD_DIM; j++)
			checkers[i][j] = i < PAWNS_DIM&& j < PAWNS_DIM ? 1 : i >= BOARD_DIM - PAWNS_DIM && j >= BOARD_DIM - PAWNS_DIM ? -1 : 0;
}

void board_c::print_checkers()
{
	for (int i = 0; i < BOARD_DIM; i++) {
		for (int j = 0; j < BOARD_DIM; j++)
			std::cout << (checkers[i][j] == 1 ? 'x' : checkers[i][j] == -1 ? 'o' : '.');
		std::cout << std::endl;
	}
}

void board_c::set_move1(int i0, int j0, int i1, int j1)
{
	checkers[i0][j0] = 0;
	checkers[i1][j1] = 1;
	print_checkers();
	std::cout << "One:(" << i0 << "," << j0 << ")" << "->(" << i1 << "," << j1 << ")" << "\n";
}

void board_c::set_move2(int i0, int j0, int i1, int j1)
{
	checkers[i0][j0] = 0;
	checkers[i1][j1] = -1;
	print_checkers();
	std::cout << "Two:(" << i0 << "," << j0 << ")" << "->(" << i1 << "," << j1 << ")" << "\n";
}

bool board_c::debut(bool allow_right, bool allow_left, bool allow_top)
{
	std::cout << "try debut\n";
	int idxs[DEBUT_MOVES][2] = {
		{3, 2}, {3, 1}, {2, 3}, {1, 3}, {3, 0}, {3, 3}, 
		{4, 3}, {4, 2}, {4, 1}, 
		{4, 4},
	};
	int k;
	for (k = 0; k < DEBUT_MOVES; k++) {
		int i = idxs[k][0];
		int j = idxs[k][1];
		if (checkers[i - 1][j] == 1 && !checkers[i][j]) {
			set_move1(i - 1, j, i, j);
			break;
		}
		if (checkers[i][j - 1] == 1 && !checkers[i][j]) {
			set_move1(i, j - 1, i, j);
			break;
		}
	}
	if (k < 10)
		return true;

	std::srand(std::time(nullptr));
	for (int ii = 0; ii < 100; ii++) {
		int i, j;
		while (1) {
			i = (int)(PAWNS_DIM * ((double)std::rand() / RAND_MAX));
			j = (int)(PAWNS_DIM * ((double)std::rand() / RAND_MAX));
			if (checkers[i][j] == 1)
				break;
		}
		bool move_down = i + 1 < BOARD_DIM && !checkers[i + 1][j];
		bool move_right = j + 1 < BOARD_DIM && !checkers[i][j + 1];
		bool move_left = j - 1 >= 0 && !checkers[i][j - 1];
		bool move_top = i - 1 >= 0 && !checkers[i - 1][j];
		if (move_down) {
			set_move1(i, j, i + 1, j);
			return true;
		}
		if (allow_right && move_right) {
			set_move1(i, j, i, j + 1);
			return true;
		}
		if (ii > 25 && allow_left && move_left) {
			set_move1(i, j, i, j - 1);
			return true;
		}		
		if (ii > 50 && allow_top && move_top) {
			set_move1(i, j, i - 1, j);
			return true;
		}
	}
	std::cout << "FATAL ERROR!\n";
	return false;
}

bool board_c::move()
{
	maze_c maze;
	std::vector<move_t> ai_moves = maze.search_moves(true, checkers);
	if (!ai_moves.size()) {
		std::cout << "No way found, proceed random.\n";
		return false;
	}
	std::sort(ai_moves.begin(), ai_moves.end(), [](const move_t& a, const move_t& b) { return a.len < b.len; });
	
	set_move1(ai_moves[0].i0, ai_moves[0].j0, ai_moves[0].i1, ai_moves[0].j1);
	std::cout << "moved\n";
	return true;

	/*
	some agent blocking algo

	move_t move;
	move.len = 0;
	move.i0 = -1;
	for (int j = 0; j < ai_moves.size(); j++) {
		const move_t& ai_m = ai_moves[j];
		maze.set_move(ai_m.i0, ai_m.j0, ai_m.i1, ai_m.j1);
		std::vector<move_t> agent_moves = maze.search_moves(false, checkers);
		if (!agent_moves.size()) {
			move = ai_m;
			move.len = 1; //????????????????
		}
		else {
			double len = 0;
			for (int i = 0; i < agent_moves.size(); i++)
				len += agent_moves[i].len;
			len /= agent_moves.size();
			if (move.len < len) {
				move.len = len;
				move = ai_m;
			}
			maze.set_back(ai_m.i0, ai_m.j0, ai_m.i1, ai_m.j1);
		}
	}
	if (move.i0 > -1) {
		set_move1(move.i0, move.j0, move.i1, move.j1);
		std::cout << "moved\n";
		return true;
	}
	else
		return false;
	*/
}
