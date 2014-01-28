//
//
//#define STREET 1
//
//
//
//class Street
//	{
//	// both of these are indexed around their seat number
//	unsigned int * stacks;
//	unsigned int * inFront;
//	unsigned int pot;
//	int street;
//
//	public:
//
//	Street(Hand& handObj)	:	Hand(handObj), 
//								stacks(new unsigned int[this->nEntrants]),
//								inFront(new unsigned int[this->nEntrants]),
//								pot(0),
//								street(ID_PREFLOP)
//		{
//		
//			   memset(stacks, 0, sizeof(unsigned int) * this->nEntrants);
//			   memset(inFront, 0, sizeof(unsigned int) * this->nEntrants);
//
//
//		}
//	~Street()
//		{
//		 delete[] stacks;
//		 delete[] inFront;
//		}
//
//
//	};