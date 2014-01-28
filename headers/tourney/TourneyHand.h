
#include <cmath>


// example template arguments:  <ID_MERGE, rapidxml::xml_node<>*>
template<ID_SITE siteID, typename nodeType>
class TourneyHand : public Hand<siteID, nodeType>

	{
	private:

		Tourney * const tourney;

		// defined in TourneyHand_XML.h
		void whichEliminated(bool * dest);


	public:
		

		TourneyHand(nodeType firstHand, Tourney * t, bool doLog): Hand(firstHand, t, doLog), tourney(t)
			{
			if (!this->isFirstInTourney())
				{
				t->resetChipsInPlay(this->sumStacks());

				}
			}

	private:

		bool checkInvariants()
			{
			if (this->tourney->getChipsInPlay() != this->sumStacks())
				return false;
			return Hand::checkInvariants();
			}
		void awardKOs()
			{
			if (this->isComplete)
				assert(this->tourney->getHHStartsLate() || this->countPlayers() == this->getNSeated());
			if (!hasEndofHand())
				return;

			bool * elim = new bool[this->getNSeatsAtTable()];
			whichEliminated(elim);

			int prizeIndex = this->getNSeated() - 1;
			bool * getsThisPrize = new bool[this->getNSeatsAtTable()];

			while (true)
				{
				memset(getsThisPrize, 0, sizeof(bool) * this->getNSeatsAtTable());
				unsigned int maxStack = 0;
				unsigned nSharing = 0;
				// todo: make the type for stacks a templated parameter?
				for (unsigned int i = 0; i < this->getNSeatsAtTable(); i++)
					{
					if (elim[i])
						{
						if (this->stacks[i] > maxStack)
							{
							maxStack = this->stacks[i];
							memset(getsThisPrize, false, sizeof(bool) * this->getNSeatsAtTable());
							getsThisPrize[i] = true;
							nSharing = 1;
							}
						else if (this->stacks[i] == maxStack)
							{
							getsThisPrize[i] = true;
							nSharing++;
							}

						}
					}
				if (nSharing == 0)
					break;
				for (unsigned int seat = 0; seat < this->getNSeatsAtTable(); seat++)
					{
					if (getsThisPrize[seat])
						{
						this->tourney->grantPrize(seat, prizeIndex, nSharing);
						elim[seat] = false;
						}
					}
				prizeIndex--;
				}

			delete[] getsThisPrize;
			delete[] elim;
			}

	public:
		bool isFirstInTourney()
			{
			return (this->getID() == 1);
			}

		// are all players eliminated this hand?
		bool isLastInTourney()
			{
			bool * elim = new bool[this->getNSeatsAtTable()];
			whichEliminated(elim);		

			int playersLeft = this->getNSeated();
			for (unsigned int i = 0; i < this->getNSeatsAtTable(); i++)
				{
				if (elim[i])
					playersLeft--;
				}
			delete[] elim;
			return (playersLeft == 0);		
			}
		bool advance(bool doLog = true)
			{
			this->awardKOs();
			bool retVal = Hand::advance(doLog);
			if (this->tourney->getChipsInPlay() != this->sumStacks())
				{
				std::cout << "Incorrect pot size: " << this->getGameID() << "-" << this->getHandID() << std::endl;
				}
			return retVal;
			}

		//unsigned int getSeatsAtTable()
		//	{
		//	return this->tourney->nSeatsAtTable;
		//	}

		void getWinnings(float * dest)
			{
			this->tourney->getWinnings(dest);
			if (this->isLastInTourney())
				assert(array_helpers::sum(dest, this->tourney->getNSeatsAtTable()) == 100.f);
			}
		void getProjWinnings(float * dest, unsigned int * stackVals)
			{
			this->tourney->getProjWinnings(dest, stackVals);
			}

		void getProjWinnings(float * dest)
			{
			this->tourney->getProjWinnings(dest, this->stacks);
			}


	};