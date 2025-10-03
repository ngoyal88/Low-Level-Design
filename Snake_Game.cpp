#include <bits/stdc++.h>
#include<iostream>
using namespace std;

enum Direction {UP, DOWN, LEFT, RIGHT};

struct Cell {
	int x, y;
	Cell(int x, int y): x(x), y(y) {}
	bool isSame(const Cell &other) const {
		return x == other.x && y == other.y;
	}
};

class Snake {
	deque<Cell> body;
	Direction dir;
public:
	Snake(int rows, int cols) {
		body.push_front(Cell(rows / 2, cols / 2));
		dir = RIGHT;
	}
	void changeDirection(Direction newDir) {
		if ((dir == UP && newDir == DOWN) ||
		        (dir == DOWN && newDir == UP) ||
		        (dir == LEFT && newDir == RIGHT) ||
		        (dir == RIGHT && newDir == LEFT)) {
			return;
		}
		dir = newDir;
	}

	void move(Cell newHead, bool ateFood) {
		if (!ateFood) body.pop_back();
		body.push_front(newHead);
	}

	vector<Cell> getBody() const {
		return vector<Cell>(body.begin(), body.end());
	}
	Cell getHead() const {
		return body.front();
	}
	Direction getDirection() const {
		return dir;
	}
};
class FoodGenerator {
public:
	Cell generateFood(int rows, int cols, const vector<Cell> &occupied) {
		srand(time(0));
		while (true) {
			int x = rand() % rows;
			int y = rand() % cols;
			Cell nfood(x, y);
			bool clash = false;
			for (auto&c : occupied) {
				if (c.isSame(nfood)) {
					clash = true;
					break;
				}
			}
			if (!clash) return nfood;
		}
	}
};
class Board {
	int rows, cols;
	Snake snake;
	Cell food;
	FoodGenerator foodGen;

public:
	Board(int r, int c): rows(r), cols(c), snake(r, c), food(0, 0) {
		food = foodGen.generateFood(r, c, snake.getBody());
	}
	void printBoard() {
		vector<vector<char>> grid(rows, vector<char>(cols, '.'));

		//draw snake
		for (auto &c : snake.getBody()) {
			grid[c.x][c.y] = 'S';
		}

		grid[food.x][food.y] = 'O';
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				cout << grid[i][j] << " ";
			}
			cout << endl;
		}
		cout << "----------------------" << endl;
	}
	Cell getNextHead() {
		Cell curr = snake.getHead();
		Direction d = snake.getDirection();
		if (d == UP) return Cell(curr.x - 1, curr.y);
		if (d == DOWN) return Cell(curr.x + 1, curr.y);
		if (d == LEFT) return Cell(curr.x, curr.y - 1);
		return Cell(curr.x, curr.y + 1);
	}
	bool collideWithBoundary(const Cell &head) {
		return (head.x < 0 || head.y < 0 || head.x >= rows || head.y >= cols);
	}

	bool collideWithSelf(const Cell &head) {
		for (auto& c : snake.getBody()) {
			if (c.isSame(head)) return true;
		}
		return false;
	}
	void step(Direction dir) {
		snake.changeDirection(dir);
		Cell next = getNextHead();

		if (collideWithSelf(next) || collideWithBoundary(next)) {
			cout << "Game Over!" << endl;
			exit(0);
		}

		bool ateFood = next.isSame(food);
		snake.move(next, ateFood);

		if (ateFood) {
			food = foodGen.generateFood(rows, cols, snake.getBody());
		}
	}
};
Direction toDir(char &c){
	if(c == 'R') return RIGHT;
	if(c == 'L') return LEFT;
	if(c == 'U') return UP;
	else return DOWN;
}
int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	Board b(10, 10);
	while (true) {
		char ch;
		cin >> ch;
		if(ch == 'S'){
			cout<<"Stopped!"<<endl;
			break;
		}
		b.step(toDir(ch));
		b.printBoard();
	}
	return 0;
}