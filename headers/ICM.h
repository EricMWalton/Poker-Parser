
//necessary, but included in the file that uses ICM.h:
//#include <array_helpers.h>
//#include <helpers.h>


// several differences from the version used by Tournament Helper: simple pointers and arrays instead of STLs
// also, parameterize hero to allow client to quickly calculate all ICM values for a set of stacks
double getICMEquity(unsigned int * stacks, unsigned int totalChips, const float * const prizes, unsigned int hero, unsigned int nInMoney, unsigned int numPlayers, unsigned int depth)
	{

	double eq = (double)prizes[depth] * stacks[hero] / totalChips;

	if (depth + 1 < nInMoney)
		{
		for (unsigned int i = 0; i < numPlayers; i++)
			{

			if (i != hero && stacks[i] > 0)
				{
				unsigned int temp = stacks[i];
				stacks[i] = 0;
				eq += getICMEquity(stacks,totalChips - temp, prizes, hero, nInMoney, numPlayers, depth+1) * temp / totalChips;
				stacks[i] = temp;
				}
			}
		}
	return eq;
	} 

double getICMEquity(unsigned int * stacks, unsigned int numPlayers, const float * const prizes, unsigned int nInMoney, unsigned int hero)
	{
	unsigned int totalChips = array_helpers::sum(stacks, numPlayers);
	return getICMEquity(stacks, totalChips, prizes, hero, nInMoney, numPlayers,0);
	}