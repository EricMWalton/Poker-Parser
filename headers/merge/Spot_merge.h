/*************** IMPLEMENTS VARIOUS FUNCTIONS IN Spot, specific to XML hand histories **************/


#define SPOT_XML(retVal)				\
	template<>									\
	retVal Spot<ARGS_MERGE>



// returns 0 if there are no more action nodes
// otherwise returns the node for the next action node
SPOT_XML(rapidxml::xml_node<>*)::getNextNode()
	{
	rapidxml::xml_node<>* nextSpot = this->node;
	id_action action;
	do	{
		nextSpot = nextSpot->next_sibling("event");
		if (nextSpot == 0)
			break;
		action = getActionID(nextSpot->first_attribute("type")->value());
		}
	while (action == ID_NACTIONS);
	return nextSpot;		
	}

SPOT_XML(unsigned int)::parseAmt()
	{
	rapidxml::xml_attribute<>* amtAttr = this->node->first_attribute("amount");
	assert(amtAttr != 0);
	return StringToNumber<unsigned int>(amtAttr->value());
	}


// return the action type of this event: FOLD, CALL, BET, RAISE, ALL_IN, SMALLBLIND, or BIGBLIND
SPOT_XML(id_action)::parseType()
	{
	rapidxml::xml_attribute<>* typeAttr = this->node->first_attribute("type");
	assert(typeAttr != 0);
	return this->getActionID(typeAttr->value());
	}

// keep advancing till you find another action of betting type (FOLD, CALL, BET, RAISE, etc.)
// return false if there is none
SPOT_XML(bool)::advanceNode()
	{
	rapidxml::xml_node<>* nextSpot = this->getNextNode();
	if (nextSpot == 0)
		{
		return false;
		}
	else
		{
		this->node = nextSpot;
		return true;
		}
	}

SPOT_XML(unsigned int)::parseNextToAct()
	{
	rapidxml::xml_attribute<>* playerAttr = this->node->first_attribute("player");
	assert(playerAttr != 0);
	return StringToNumber<unsigned int>(playerAttr->value());
	}

SPOT_XML(bool)::nodeToEnd()
	{
	rapidxml::xml_node<>* roundNode = this->node->parent();
	rapidxml::xml_attribute<>* idAttr;
	do
		{
		roundNode = roundNode->next_sibling("round");
		if (roundNode == 0)
			// incomplete HH?
			{
			std::cout << "No end of hand given: " << this->hand->getGameID() << "-" << this->hand->getHandID() << std::endl;
			return false;
			}

		idAttr = roundNode->first_attribute("id");
		assert(idAttr != 0);
		}		
		while(strcmp(idAttr->value(), "END_OF_GAME") != 0 && strcmp(idAttr->value(), "END_OF_FOLDED_GAME") != 0);

		this->node = roundNode;
		return true;
	}

// returns false if this is the end of the hand
SPOT_XML(bool)::nodeToNextStreet()
	{
	rapidxml::xml_node<>* nextRound = this->node->parent()->next_sibling("round");
	if (nextRound == 0)
		{
		// incomplete HH?
		std::cout << "No end of hand given: " << this->hand->getGameID() << "-" << this->hand->getHandID() << std::endl;
		return false;
		}
	rapidxml::xml_attribute<>* idAttr = nextRound->first_attribute("id");
	assert(idAttr != 0);
	this->streetID = this->getStreetID(idAttr->value());
	if (streetID != ID_NSTREETS)
		{
		rapidxml::xml_node<>* eventNode = nextRound->first_node("event");
		if (eventNode != 0)
			{
			this->node = eventNode;
			return true;
			}
		else
			return false;
		}
	else
		{
		return false;
		}
	}

// award pot winnings as written in HH
SPOT_XML(void)::awardWinnings()
	{
	assert(this->reachedEnd);

	for (rapidxml::xml_node<>* winnerNode = this->node->first_node("winner"); winnerNode != 0; winnerNode = winnerNode->next_sibling("winner"))
		{
		rapidxml::xml_attribute<>* amtAttr = winnerNode->first_attribute("amount");
		if (amtAttr)
			{
			rapidxml::xml_attribute<>* playerAttr = winnerNode->first_attribute("player");
			assert(playerAttr);
			unsigned int p = StringToNumber<unsigned int>(playerAttr->value());
			unsigned int amt = StringToNumber<unsigned int>(amtAttr->value());
			this->stacks[p] += amt;
			}
		}
	}

// returns 0 if there are no action nodes
// otherwise returns the first action node in the hand
SPOT_XML(rapidxml::xml_node<>*)::parseFirstNode(rapidxml::xml_node<>* handNode)
	{															 
	rapidxml::xml_node<>* firstRound = handNode->first_node("round");
	assert(firstRound);
	rapidxml::xml_node<>* eventNode = firstRound->first_node("event");

	assert(eventNode);
	id_action action = getActionID(eventNode->first_attribute("type")->value());
	while (action == ID_NACTIONS)
		{
		eventNode = eventNode->next_sibling("event");
		if (eventNode == 0)
			break;
		action = getActionID(eventNode->first_attribute("type")->value());
		}
	return eventNode;
	}

SPOT_XML(void)::processBlindsAndAntes()
	{
	assert(strcmp(node->parent()->first_attribute("id")->value(), "BLINDS") == 0);
	bool hasAnotherSpot = true;
	while (hasAnotherSpot)
		{
		hasAnotherSpot = this->advanceSpot();
		}
	assert(this->nodeToNextStreet() != 0);
	//assert(this->advanceStreet());
	// if false: incomplete HH. no streets
	}

// for actual winnings 
SPOT_XML(void)::processWinnings()
	{
	assert(strcmp(node->parent()->first_attribute("id")->value(), "END_OF_GAME") == 0 || strcmp(node->parent()->first_attribute("id")->value(), "END_OF_FOLDED_GAME") == 0);
	}

SPOT_XML(id_street)::parseStreetID()
	{
	rapidxml::xml_node<>* roundNode = this->node->parent();
	assert(roundNode != 0);

	rapidxml::xml_attribute<>* idAttr = roundNode->first_attribute("id");
	assert(idAttr != 0);
	return getStreetID(idAttr->value());
	}


// functions for class Hand

template<>
Spot<ID_MERGE, rapidxml::xml_node<>*> Hand<ID_MERGE, rapidxml::xml_node<>*>::firstSpot()
	{
	return Spot<ID_MERGE, rapidxml::xml_node<>*>(this, this->node);
	}	

template<>
void Hand<ID_MERGE, rapidxml::xml_node<>*>::updateStacksPostHand()
	{
	Spot<ID_MERGE, rapidxml::xml_node<>*> spot = this->firstSpot();
	bool hasAnotherSpot = true;
	while (hasAnotherSpot)
		{
		hasAnotherSpot = spot.advance();
		}
	spot.awardWinnings();
	spot.getStacks(this->stacks);
	this->checkInvariants();
	}

