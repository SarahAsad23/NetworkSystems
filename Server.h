
using namespace std; 

class Server {

	int serverSd;
	

public: 
	Server()
	{
		this->serverSd = -1;
	}

	~Server() 
	{
		if (this->serverSd != -1) {
			close(this->serverSd);
			this->serverSd = -1;
		}
	}

	int Start(int port);

private:

	int processRequest(int clientSd);
};