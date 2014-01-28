#define HAND 1




template<ID_SITE siteID, typename nodeType>
class Spot;


// example template arguments:  <ID_MERGE, rapidxml::xml_node<>*>
template<ID_SITE siteID, typename nodeType>
class Hand {


/************** method declarations ******************/
// defined for each hand history type in files like Hand_XML.h, Hand_Pokerstars.h, etc.

private:
	void getStacks_preblinds();	// fill Hand::stacks with stack values at top of hand;
	void addBlinds();		 // put blinds back in the stacks (for cases where stacks listed are "preposted"
	void parseWasDealtin();

protected:

	bool hasEndofHand();
	unsigned int getHandID(nodeType);
	unsigned int countPlayers();

public:

	bool hasStackInfo();
	bool advance(bool);
	Spot<siteID, nodeType> firstSpot();
	void updateStacksPostHand();

/********************** DATA MEMBERS ****************************/

private:
   
	Game * const game;
	unsigned int ID;
	unsigned int nSeated;
	bool * const wasDealtin; // is this player dealt in?

protected:
	nodeType node;
	unsigned int * const stacks;
	bool isComplete;  // is there a break in hand history here?  see advance()


/*************** FUNCTION DEFINITIONS *********/

private:

	void printLog()
		{
		printf("Game: %10d \t Hand: %3d\n", this->game->getID(), this->ID); 
		}

	// advance to next hand; derived class processes text, and calls this if there is another hand
	// derived class returns false if there isn't another hand
	// if doLog = true, writes to console basic info on the hand
	void resetVars(bool doLog) 
		{
		int newID = getHandID();

		bool hsi = hasStackInfo();
		this->isComplete = (hsi && (this->ID + 1 == newID));
		if (hsi)
			getStacks_preblinds();	
		this->ID = newID;

		if (doLog)
			{
			printLog(); 
			}

		unsigned int count = countPlayers();
		assert (count <= this->nSeated);
		if (count)
			this->nSeated = count;
		}

protected:

	virtual bool checkInvariants()
		{
		if (this->countPlayers() != this->nSeated)
			{

			return false;
			}
		if (this->nSeated <= this->game->getNSeatsAtTable())
			return false;
		return true;
		}



	unsigned int sumStacks()
		{
		return array_helpers::sum<unsigned int>(stacks, game->getNSeatsAtTable());
		}
public:

	// todo: parseHandID() is protected, getHandID() is public
	unsigned int getHandID()
		{
		return this->getHandID(this->node);
		}


	Hand(nodeType firstHand, Game * gamePtr, bool doLog) : game(gamePtr), ID(getHandID(firstHand)), nSeated(gamePtr->getNSeatsAtTable()), wasDealtin(new bool[gamePtr->getNSeatsAtTable()]), node(firstHand), stacks(new unsigned int[nSeated]), isComplete(hasStackInfo())
		{
		if (doLog)
			{
			printLog();
			}
		//this->isComplete = this->hasStackInfo();
		if (this->isComplete)
			this->getStacks_preblinds();
		this->parseWasDealtin();
		this->checkInvariants();
		}

	~Hand()
		{
		delete[] stacks;
		delete[] wasDealtin;
		}
	void getStacks(unsigned int * dest)
		{
		memcpy(dest, this->stacks, sizeof(unsigned int) * this->getNSeatsAtTable());
		}

	unsigned int getID()
		{
		return this->ID;
		}

	unsigned int getNSeatsAtTable()
		{
		return this->game->getNSeatsAtTable();
		}

	unsigned int getGameID()
		{
		return this->game->getID();
		}

	unsigned int getNSeated()
		{
		return this->nSeated;
		}
	void getWasDealtin(bool * dest)
		{
		memcpy(dest, this->wasDealtin, sizeof(bool) * this->game->getNSeatsAtTable());
		}

	};


