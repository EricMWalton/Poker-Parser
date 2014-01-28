/********************** INCLUDES FUNCTION DEFINITIONS FOR VARIOUS MEMBERS OF Hand, specific to XML-formatted hand history files ********/


#ifndef RAPIDXML
#include <rapidxml.hpp>
#include <rapidxml_iterators.hpp>
#include <rapidxml_print.hpp>
#include <rapidxml_utils.hpp>
#endif


#ifndef XML_HELPERS
#include <xml_helpers.h>
#endif


#include <cmath>



#define HAND_XML(retVal)				\
	template<>									\
	retVal Hand<ARGS_MERGE>




template<ID_SITE siteID, typename nodeType>
class Spot;


// add the blinds and antes back to the player's stacks
// used when Tourney::prePost is true, when the HH file gives initial stacks as being after posting
// todo: make this part of Spot()?
HAND_XML(void)::addBlinds()
	{
	for (rapidxml::xml_node<> * eventNode=node->first_node("round")->first_node("event"); eventNode; eventNode=eventNode->next_sibling())
		{
		std::string typeStr=eventNode->first_attribute("type")->value();
		if (typeStr=="SMALL_BLIND" || typeStr=="BIG_BLIND" || typeStr=="ANTE" || typeStr=="ALL_IN")
			{
			int p = StringToNumber<int>(eventNode->first_attribute("player")->value());
			unsigned int amt = StringToNumber<unsigned int>(eventNode->first_attribute("amount")->value());
			this->stacks[p] += amt;
			}
		}
	}


HAND_XML(void)::getStacks_preblinds()
	{
	// place in Hand::stacks the values of everyone's stack, pre-blinds and antes

	//bool * isAllinBlind = new bool[this->game->nSeatsAtTable];
	//unsigned int nAllinBlind = 0;
	//memset(isAllinBlind, false, sizeof(bool) * this->game->nSeatsAtTable);

	memset(this->stacks, 0, sizeof(unsigned int) * this->getNSeatsAtTable()); 

	rapidxml::xml_node<>* playerList = this->node->first_node("players");
	assert(playerList);

	for (rapidxml::xml_node<>* player = playerList->first_node("player"); player; player = player->next_sibling("player"))
		{
		rapidxml::xml_attribute<>* balanceAttr = player->first_attribute("balance");
		assert(balanceAttr);

		rapidxml::xml_attribute<>* seatAttr = player->first_attribute("seat");
		assert(seatAttr);

		unsigned int seat = StringToNumber<unsigned int>(seatAttr->value());
		assert(seat >= 0 && seat < this->getNSeatsAtTable());

		unsigned int amt = dollarsToNumber<unsigned int>(balanceAttr->value());

		//if (amt == 0)
		//	{
		//	if (this->game->prePost)
		//		{
		//		 isAllinBlind[seat] = true;
		//		 nAllinBlind++;
		//		 // when this happens (see Merge/2012/07-20/file...(58619799-1).xml hand 37)
		//		 // for some reason the "amount" listed under the ALL_IN blind is 0.
		//		}
		//	else
		//		assert(false);

		//	}
		this->stacks[seat] = amt;
		}
	if (this->game->getPrePost())
		addBlinds();

	// for handling weird handhistories with prepost and someone allin blind.
	//amt  = 0 in that case
	//if (nAllinBlind > 0)
	//	{
	//	int remainder = this->tourney->nChipsInPlay - sumStacks();
	//	assert(remainder > 0);
	//	for (unsigned int i = 0; i < this->game->nSeatsAtTable; i++)
	//		{
	//		if (isAllinBlind[i])
	//			stacks[i] = remainder / nAllinBlind;
	//		}
	//	}
	//assert(sumStacks() == tourney->nChipsInPlay);
	//delete[] isAllinBlind;
	}


// does this hand list stacks at top? if no, that means we got disconnected
HAND_XML(bool)::hasStackInfo()
	{
	rapidxml::xml_node<>* playerList = this->node->first_node("players");
	assert(playerList);
	return ((playerList->first_node("player")) != 0);
	}

// todo: add check() fcn for testing invariants
HAND_XML(bool)::hasEndofHand()
	{
	rapidxml::xml_node<>* lastRound = node->last_node("round");
	assert(lastRound);
	rapidxml::xml_attribute<>* idAttr = lastRound->first_attribute("id");
	assert(idAttr);
	return(strcmp(idAttr->value(),"END_OF_GAME\0") == 0);
	}

HAND_XML(unsigned int)::getHandID(rapidxml::xml_node<>* handNode)
	{
	rapidxml::xml_attribute<>* idAttr = handNode->first_attribute("id");

	assert(idAttr);

	std::string idString = std::string(idAttr->value());
	int dashIndex = idString.find("-");

	assert(dashIndex != std::string::npos);

	unsigned int retNumber =  StringToNumber<unsigned int>(idString.substr(dashIndex + 1));

	assert(retNumber);

	return retNumber;
	}

// count the players in the hand
HAND_XML(unsigned int)::countPlayers()
	{
	rapidxml::xml_node<>* playerList = node->first_node("players");
	assert(playerList);
	return xml_helpers::countChildren(playerList);
	}

// advance to next hand in the hand history; return true if there is another hand
HAND_XML(bool)::advance(bool doLog)
	{
	// todo: error handling: exit and set a bool to false, on Hand and/or Tourney object
	// if either bool is false, don't do the SQL insertion and dont' do all the assert checks
	if (xml_helpers::isLastChild(this->node, "game"))
		return false;
	this->node = this->node->next_sibling("game");

	// as of now, Hand::advance() always returns true
	this->resetVars(doLog);
	return true;
	}

HAND_XML(void)::parseWasDealtin()
	{
	memset(this->wasDealtin, false, sizeof(bool) * this->game->getNSeatsAtTable());

	rapidxml::xml_node<>* playerList = this->node->first_node("players");
	assert(playerList);

	for (rapidxml::xml_node<>* playerNode = playerList->first_node("player"); playerNode != 0; playerNode = playerNode->next_sibling("player"))
		{
		rapidxml::xml_attribute<>* seatAttr = playerNode->first_attribute("seat");
		assert(seatAttr != 0);
		unsigned int seat = StringToNumber<unsigned int>(seatAttr->value());

		rapidxml::xml_attribute<>* dealtAttr = playerNode->first_attribute("dealtin");
		assert(dealtAttr != 0);

		if (strcmp(dealtAttr->value(), "true") == 0)
			this->wasDealtin[seat] = true;
		}
	}

