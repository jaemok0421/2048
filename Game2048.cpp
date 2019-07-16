#include "pch.h"
#include "curses.h"
#include "Game2048.h"
#include <ctime>
#include <fstream>
#include <string>
#include <vector>

Game2048::Game2048() {
	e.seed(time(NULL));
	std::ifstream in("data\\top.sav");
	if (in.is_open())
		in >> hiscore;
	else
		hiscore = 0;
	in.close();
	start();
}  //初始化

int Game2048::getStatus() {
	return status;
}  //获取当前游戏状态

void Game2048::rotate() {
	int tmp[N][N];
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			tmp[i][j] = data[i][j];
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			data[i][j] = tmp[j][N - i - 1];
}  //逆时针90°旋转矩阵

bool Game2048::isOver() {
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j) {
			if (!data[i][j])
				return false;
			if (i < N - 1 && data[i][j] == data[i + 1][j])
				return false;
			if (j < N - 1 && data[i][j] == data[i][j + 1])
				return false;
		}
	return true;
}

void Game2048::processInput() {
	char cmd = getch();
	if (cmd >= 'a' && cmd <= 'z')
		cmd -= 32;
	if (status != S_LOSE) {
		bool updated = false;
		switch (cmd) {
			case 'A':
				updated = moveLeft();
				break;
			case 'S':
				rotate();
				rotate();
				rotate();
				updated = moveLeft();
				rotate();
				break;
			case 'D':
				rotate();
				rotate();
				updated = moveLeft();
				rotate();
				rotate();
				break;
			case 'W':
				rotate();
				updated = moveLeft();
				rotate();
				rotate();
				rotate();
		}
		if (updated) {
			randNew();
			if (isOver())
				status = S_LOSE;
		}
	}
	switch (cmd) {
		case 'R':
			start();
			break;
		case 'Q':
			status = S_QUIT;
	}
}

int getLen(int n) {
	if (!n)
		return 0;
	int len = 1;
	while (n >= 10) {
		n /= 10;
		++len;
	}
	return len;
}  //获取数字长度

void Game2048::draw() {
	clear();
	for (int i = 0; i <= N; ++i) {
		move(LINES / 3 - 4 + 2 * i, COLS / 2 - 2 * WIDTH - 2);
		std::string line(WIDTH, '-');  //填充适当长度的横线
		for (int j = 0; j < N; ++j) {
			addch('+');
			addstr(line.data());
			if (j == N - 1)
				addch('+');
		}  //横线边框绘制
		if (i != N) {
			move(LINES / 3 - 4 + 2 * i + 1, COLS / 2 - 2 * WIDTH - 2);
			for (int j = 0; j < N; ++j) {
				std::string space(WIDTH - getLen(data[i][j]), ' ');
				addch('|');
				addstr(space.data());
				if (data[i][j])
					addstr(std::to_string(data[i][j]).data());
				if (j == N - 1)
					addch('|');
			}
		}  //竖线+数字绘制
	}
	//其他文本绘制
	std::string strTitle = "2048",
				strHelp = "W(UP),S(DOWN),A(LEFT),D(RIGHT),R(RESTART),Q(QUIT)",
				strWin = "You win, continue or press R to restart",
				strLose = "You lose, press R to restart";
	mvprintw(LINES / 3 - 9, COLS / 2 - strTitle.size() / 2, strTitle.data());
	move(LINES / 3 - 7, COLS / 2 + WIDTH);
	addstr("SCORE: ");
	addstr(std::to_string(score).data());
	move(LINES / 3 - 6, COLS / 2 + WIDTH - 3);
	addstr("HI SCORE: ");
	addstr(std::to_string(hiscore).data());
	mvprintw(LINES / 3 + 6, COLS / 2 - strHelp.size() / 2, strHelp.data());
	if (status == S_WIN)
		mvprintw(LINES / 3 + 7, COLS / 2 - strWin.size() / 2, strWin.data());
	else if (status == S_LOSE)
		mvprintw(LINES / 3 + 7, COLS / 2 - strLose.size() / 2, strLose.data());
	refresh();
}  //画面绘制

void Game2048::quit() {
	std::ofstream out("data\\top.sav");
	if (out.is_open())
		out << hiscore;
	out.close();
}

void Game2048::start() {
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			data[i][j] = 0;
	randNew();
	randNew();
	status = S_NORMAL;
	score = 0;
}  //初始化并随机生成两个数字

bool Game2048::randNew() {
	std::vector<int> emptyPos;
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			if (!data[i][j])
				emptyPos.push_back(i*N + j);
	int n = emptyPos.size();
	if (!n)
		return false;
	std::uniform_int_distribution<int> r1(0, n - 1);  //决定生成位置
	std::uniform_int_distribution<int> r2(0, 9);  //决定生成数字
	int p = emptyPos[r1(e)];
	data[p / N][p % N] = !r2(e) ? 4 : 2;
	return true;
}  //随机生成一个数字

bool Game2048::moveLeft() {
	bool changed = false;
	for (int i = 0; i < N; ++i) {
		int writePos = 0;
		for (int j = 0; j < N; ++j) {
			if (!data[i][j])
				continue;
			while (writePos < j) {
				if (data[i][writePos] == data[i][j]) {
					data[i][writePos] *= 2;
					data[i][j] = 0;
					score += data[i][writePos];
					if (score > hiscore)
						hiscore = score;
					if (status != S_WIN && data[i][writePos] == TARGET)
						status = S_WIN;
					changed = true;
					++writePos;
					break;
				}
				else if (!data[i][writePos]) {
					data[i][writePos] = data[i][j];
					data[i][j] = 0;
					changed = true;
					break;
				}
				++writePos;
			}
		}
	}
	return changed;
}  //左移操作处理