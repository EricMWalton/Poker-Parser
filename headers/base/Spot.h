#define SPOT 1


enum id_action {ID_FOLD, ID_CALL, ID_BET, ID_RAISE, ID_ALLIN, ID_SMALLBLIND, ID_BIGBLIND, ID_ANTE, ID_NACTIONS};
std::string str_action[] = {"FOLD", "CALL", "BET", "RAISE", "ALL_IN", "SMALL_BLIND", "BIG_BLIND", "ANTE"};

enum id_street {ID_BLINDS, ID_PREFLOP, ID_FLOP, ID_TURN, ID_RIVER, ID_NSTREETS};
std::string str_street1[] = {"BLINDS", "PREFLOP", "FLOP", "TURN", "RIVER"};
std::string str_street2[] = {"BLINDS", "PREFLOP", "POSTFLOP", "POSTTURN", "POSTRIVER"};


// todo: make MERGE a template for both arguments below

// example template arguments:  <ID_MERGE, rapidxml::xml_node<>*>
template<ID_SITE siteID, typename nodeType>
class Spot	{


/************** method declarations ******************/
// defined for each hand history type in files like Spot_XML.h, Spot_Pokerstars.h, etc.

private:

   	unsigned int parseAmt();
	id_action parseType();
	bool advanceNode();
	nodeType getNextNode();
	unsigned int parseNextToAct();
	bool nodeToNextStreet();
	bool nodeToEnd();
	void processBlindsAndAntes();
	void processWinnings();
	id_street parseStreetID();
	nodeType parseFirstNode(nodeType);

public: 
		void awardWinnings();


/**************  DATA MEMBERS ********************/

private:

	// data members
	nodeType node;
	Hand<siteID, nodeType> * hand;
	unsigned int * const stacks;
	unsigned int * const inFront;  // inFront[i] is the amount in front of player i for the current street
	unsigned int * const inFront_prev; // like above, but for previous streets together
	bool * const hasCards;
	unsigned int nextToAct;
	id_street streetID;
	bool reachedEnd;


/***************** FULLY DEFINED FUNCTIONS *****************/

private:

	// todo: make these templates somehow?
	// returns ID_NACTIONS if the type given is something other than the "action" types 
	// these are skipped
	static id_action getActionID(std::string actionStr)
		{
		for (unsigned int i = ID_FOLD; i != ID_NACTIONS; i++)
			{
			if (str_action[i] == actionStr)
				return (id_action)i;
			}

		return ID_NACTIONS;
		}

	// returns ID_NSTREETS if the type given is not a street name (means end of hand?)
	static id_street getStreetID(std::string streetStr)
		{
		for (unsigned int i = ID_BLINDS; i != ID_NSTREETS; i++)
			{
			if (streetStr == str_street1[i] || streetStr == str_street2[i])
				return (id_street)i;
			}
		return ID_NSTREETS;
		}

	void check()
		{
		// check invariants
		assert(hasCards[nextToAct]);
		// sum of inFront + stacks  = chips in play
		}

	bool advanceStreet()
		{
		// add inFront[] values to inFront_prev[]
		array_helpers::plus(this->inFront_prev, this->inFront, this->inFront_prev, this->hand->getNSeatsAtTable());

		// subtract the inFront values from each stack
		array_helpers::minus<unsigned int>(this->stacks, this->stacks, this->inFront, this->hand->getNSeatsAtTable(), true);

		// reset inFront[] to all 0
		memset(this->inFront, 0, sizeof(unsigned int) * this->hand->getNSeatsAtTable());

		if (this->nodeToNextStreet())
			{
			this->nextToAct = this->parseNextToAct();
			return true;
			}
		else
			return false;

		// move node, then call resetActionVars()

		}

	// process the current action
	// find the next spot in this street
	// if there is one, reset the Spot variables to reflect it
	// if there isn't, return false
	bool advanceSpot()
		{
		this->processAction();
		if (this->advanceNode())
			{
			this->nextToAct = this->parseNextToAct();
			return true;
			}
		else
			return false;
		}

	// todo: count BLINDS and PREFLOP as same street
	// so a RAISE from the blinds sets a new total for that combined "PREFLOP" street
	void processAction()
		{
		unsigned int amt;
		id_action action = parseType();

		switch(action)
			{
		case ID_FOLD:
			hasCards[nextToAct] = false;
			break;
		case ID_BET:
		case ID_ALLIN:
		case ID_CALL:	
		case ID_SMALLBLIND:
		case ID_BIGBLIND:
			amt = parseAmt();
			inFront[nextToAct] += amt;
			break;
		case ID_ANTE:  // antes are not included in the "inFront" for the preflop betting round; basically they're a separate street
			amt = parseAmt();
			this->stacks[nextToAct] -= amt;
			inFront_prev[nextToAct] += amt;
			break;
		case ID_RAISE:
			amt = parseAmt();
			inFront[nextToAct] = amt; // when you raise, the amt given is not the amt of the raise but the new total inFront for that street
			break;
			}
		// other possibilities we don't have to do anything with: "CHAT", etc.
		}

	// todo: make sure you call this with a hand object that is actually at the beginning of a hand
	// (not after updateStacksPostHand)
	// todo: initialize hasCards

public:

	Spot(Hand<siteID, nodeType> * h, nodeType node) : node(parseFirstNode(node)), hand(h), stacks(new unsigned int[h->getNSeatsAtTable()]), inFront(new unsigned int[h->getNSeatsAtTable()]), inFront_prev(new unsigned int [h->getNSeatsAtTable()]), hasCards(new bool[h->getNSeatsAtTable()]), reachedEnd(false)
		{
		memset(inFront, 0, sizeof(unsigned int) * h->getNSeatsAtTable());
		memset(inFront_prev, 0, sizeof(unsigned int) * h->getNSeatsAtTable());
		h->getWasDealtin(this->hasCards);
		h->getStacks(this->stacks);
		this->nextToAct = this->parseNextToAct();
		this->streetID = this->parseStreetID();
		// parse the blinds
		processBlindsAndAntes();
		}

	void getStacks(unsigned int * dest)
		{
		memcpy(dest, this->stacks, sizeof(unsigned int) * this->hand->getNSeatsAtTable());
		}

	bool advance()
		{
		if (advanceSpot())
			return true;
		else if (advanceStreet())
			return true;
		else 
			{
			// reached end of hand
			this->nodeToEnd();
			this->reachedEnd = true;
			return false;
			}
		}

	~Spot()
		{
		delete[] stacks;
		delete[] inFront_prev;
		delete[] inFront;
		delete[] hasCards;
		}
	};

