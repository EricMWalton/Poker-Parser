


		// todo: move this to TourneyHand
		 template<>
		void TourneyHand<ID_MERGE, rapidxml::xml_node<>*>::whichEliminated(bool* dest)
			{
			memset(dest, 0, sizeof(bool) * this->getNSeatsAtTable());

			for (rapidxml::xml_node<>* eventNode = node->last_node("round")->first_node("event"); eventNode; eventNode = eventNode->next_sibling("event"))
				{
				rapidxml::xml_attribute<>* typeAttr = eventNode->first_attribute("type");
				assert(typeAttr);

				if (strcmp(typeAttr->value(),"PLAYER_OUT") == 0)
					// someone was eliminated
					{
					rapidxml::xml_attribute<>* playerAttr = eventNode->first_attribute("player");
					assert(playerAttr);
					int playerOut = StringToNumber<int>(playerAttr->value());
					dest[playerOut] = true;
					}
				}			
			}