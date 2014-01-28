

#include <my_math.h> // isAlmostEqual()



class Tourney : public Game  {
	//friend class TourneyHand;
	//  todo: remove this, adding getter for Hand.stacks

public:
	// todo: make hand object an attribute of Game
	//const unsigned int nEntrants; 
	const unsigned int nInMoney;

private:
	const float * const prizes;	
	float * const winnings;

public:
	unsigned int nChipsInPlay;

private:
	bool HHStartsLate;

public:

//private:
	void resetChipsInPlay(unsigned int val)
		{
		this->HHStartsLate = true;
		this->nChipsInPlay = val;

		}
	bool getHHStartsLate()
		{
		return HHStartsLate;

		}

	const float * const getPrizes(const int nEntrants)
		// sadly, no tourney overview file is included in Merge hand histories
		{
		float * prizes = new float[nEntrants];

		memset(prizes,0.f,nEntrants*sizeof(float));

		if (nEntrants == 6)
			{
			prizes[0] = 65.f;
			prizes[1] = 35.f;
			}
		else if (nEntrants == 9 || nEntrants == 10)
			{
			prizes[0] = 50.f;
			prizes[1] = 30.f;
			prizes[2] = 20.f;
			}
		else // obv fine for headsup tourneys; for others, there's no way to tell what the prizes should be
			prizes[0] = 100.f;



		assert(array_helpers::sum(prizes, nEntrants) == 100);

		return prizes;
		}
	unsigned int getChipsInPlay()
		{
			return this->nChipsInPlay;

		}

private:
	unsigned int parseChipsInPlay()
		{
		// todo: really?
		if (this->nSeatsAtTable == 9)
			{
			return 13500;
			}
		else if (this->nSeatsAtTable == 6)
			{
			return 1800;
			}
		else if (this->nSeatsAtTable == 10)
			{
			return 15000;
			}
		else
			{
			std::cout << "HH starts after first hand?" << std::endl;
			return 0;
			}
		}

 // todo: make  the constructors of the base classes protected?

public:
	Tourney(unsigned int ID, unsigned int nEntrants, bool prePost = false) : Game(ID, nEntrants, prePost), nInMoney(nEntrants / 3), prizes(getPrizes(nEntrants)), winnings(new float[nEntrants]), nChipsInPlay(parseChipsInPlay()) 


		{
		// todo: find prePost
		// at same time as calculating nChipsInPlay
		// for (int i = 0; i < nEntrants; i++)
		//players[i].seat = i;
		this->HHStartsLate = (this->nChipsInPlay == 0);

		memset(winnings, 0.f, sizeof(float) * nEntrants);
		}
	~Tourney()
		{
		delete[] prizes;
		delete[] winnings;
		}


	//  todo: move all SQL fcns outside these classes 
	// setup winnings() and projWinnings() as Tourney methods		
	// save to SQL database the tournament ID and projected winnings from this tournament

//protected:
	// player {seat} exits in {prize} place, tied with {nSHarers} other players
	// prize is 0-indexed; nSharers is in range 1..nEntrants
	void grantPrize(unsigned int seat, unsigned int prize, unsigned int nSharers = 1)
		{
		assert(seat < this->nSeatsAtTable);
		assert(prize < this->nSeatsAtTable);
		assert(nSharers > 0 && nSharers <= this->nSeatsAtTable);
		if (prize < this->nInMoney)
			this->winnings[seat] += this->prizes[prize] / nSharers;
		}

	// todo: distinguish nEntrants and nSeatsAtTable properly
	// given stackVals, fill dest[] with ICM values
	// may not add up to 100 if any prizes / bounties have already been awarded but are not in this->winnings[]

public:
	void getProjWinnings(float * dest, unsigned int * stackVals)
		{
		memcpy(dest, this->winnings, sizeof(float) * this->nSeatsAtTable);
		for (unsigned int i = 0; i < this->nSeatsAtTable; i++)
			{
			if (stackVals[i] > 0)
				{
				dest[i] += getICMEquity(stackVals, this->nSeatsAtTable, this->prizes, this->nInMoney, i);
				}
			}
		assert(my_math::isAlmostEqual(array_helpers::sum<float>(dest, this->nSeatsAtTable), 100.f));
		}

	void getWinnings(float * dest)
		{
		memcpy(dest, this->winnings, sizeof(float) * this->nSeatsAtTable);
		}



	};