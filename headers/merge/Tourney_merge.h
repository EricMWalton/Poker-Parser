#define TOURNEY_XML 1


class Tourney_XML : public Tourney
	{
	private:
		// are the blinds posted before stack amts listed?  sometimes true on older Merge hands
		bool parsePrePost(rapidxml::xml_node<>* handNode)
			{
			unsigned int nChipsInPlay = 0;

			rapidxml::xml_node<>* playerList = handNode->first_node("players");
			assert(playerList);

			if (!playerList->first_node("player"))
				{
				return false;
				}
			for (rapidxml::xml_node<>* player = playerList->first_node("player"); player; player = player->next_sibling("player"))
				{
				rapidxml::xml_attribute<>* amtAttr = player->first_attribute("balance");
				assert(amtAttr);

				unsigned int amt = dollarsToNumber<unsigned int>(amtAttr->value());
				assert(amt > 0);
				nChipsInPlay += amt;
				}
			return ((nChipsInPlay % 300) != 0);
			}

		// count the seats at the tables
		int parseNEntrants(rapidxml::xml_node<>* handNode)
			{
			rapidxml::xml_node<>* playerList = handNode->first_node("players");
			assert(playerList);
			unsigned int maxSeat = 0;

			for (rapidxml::xml_node<>* playerNode = playerList->first_node("player"); playerNode != 0; playerNode = playerNode->next_sibling("player"))
				{
				rapidxml::xml_attribute<>* seatAttr = playerNode->first_attribute("seat");
				assert(seatAttr);
				unsigned int thisSeat = StringToNumber<unsigned int>(seatAttr->value());
				if (thisSeat > maxSeat)
					 maxSeat = thisSeat;

				}
			return maxSeat + 1;
			}

	unsigned int parseID(rapidxml::xml_node<>* handNode)
		{
		rapidxml::xml_attribute<>* idAttr = handNode->first_attribute("id");

		assert(idAttr);

		std::string idString = std::string(idAttr->value());
		int dashIndex = idString.find("-");

		assert(dashIndex != std::string::npos);

		unsigned int retNumber =  StringToNumber<unsigned int>(idString.substr(0,dashIndex));

		assert(retNumber);

		return retNumber;
		}

	

	public:

		Tourney_XML(rapidxml::xml_node<>* firstHand) : Tourney(parseID(firstHand), parseNEntrants(firstHand), parsePrePost(firstHand))
			{

			}



	};