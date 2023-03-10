#pragma once

struct Game {
	int id;

	int user1;
	std::string userName1;

	int user2;
	std::string userName2;

	int turnOfUser;
	int board[3][3];

	int winnerId;

	Game() :
		id(0), user1(0), user2(0), winnerId(0) {

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				this->board[i][j] = 0;
			}
		}
	}

	Game(int id, int userId, std::string name) :
		id(id), user1(userId), userName1(name), user2(-1), turnOfUser(userId) {

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				this->board[i][j] = 0;
			}
		}
	}
};
