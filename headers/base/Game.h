#define GAME 1

class Game	{

 	

private:
	const unsigned int ID;
	const bool prePost;

protected:
	const unsigned int nSeatsAtTable;
	

public:

	Game(unsigned int gameID, unsigned int nSeatsAtTable, bool prePost) : ID(gameID), prePost(prePost), nSeatsAtTable(nSeatsAtTable)
		{

		}
	unsigned int getID()
		{
			return this->ID;

		}
	// todo: make private?
	bool getPrePost()
		{
		return this->prePost;
		}
	unsigned int getNSeatsAtTable()
		{
		return this->nSeatsAtTable;
		}
	};