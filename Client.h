
using namespace std; 

class Client {

public: 
	
	int connect(const char* server, int serverPort);

	void getAllWaitingGames();
	int startNewGame();
	int joinExistingGame();
	int makeMove();
	int getNextMove();

private:


};