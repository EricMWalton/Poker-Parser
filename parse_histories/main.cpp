/************* DEMONSTRATES PARSING HAND HISTORY FILES FOR MERGE SIT N GOES ********************/
// parses all hand history files you have in a listed directory
// saves to an sqlite3 database the following:
	// a snapshot of the stacks at the start of each ahnd
	// the winnings of each player (if the HH file ends before the SnG ends, ICM estimates are used for players still alive

#include "../headers/std_headers.h"
#include "../headers/merge_headers.h"




char * openFile(rapidxml::xml_document<>* doc, std::string file)
	{
	char * buffer;
	ifstream is;
	int length;

	//cout<<endl<<endl<<"New file: "<<files[i]<<endl;
	is.open (file, ios::binary );  // real

	// get length of file:
	is.seekg (0, ios::end);
	length = is.tellg();
	is.seekg (0, ios::beg);

	// allocate memory:
	try
		{
		buffer = new char [length+1];
		}
	catch (bad_alloc& ba)
		{
		cerr << endl<<"bad_alloc caught: " << ba.what() << endl;

		}

	// read data as a block:
	is.read (buffer,length);
	buffer[length]='\0';
	is.close();

	rapidxml::parse_trim_whitespace;
	doc->parse<rapidxml::parse_trim_whitespace>(buffer);    // 0 means default parse flags

	return buffer;

	}



// save to an SQL database the hand ID, # of players,and stacks in tournament at start of hand
template<ID_SITE siteID, typename nodeType>
void saveSnapshotToSQL(sqlite3 * db, Hand<siteID, nodeType>& handObj)
	{
	const size_t countData = handObj.getNSeatsAtTable() + 3;
	unsigned int * insertData = new unsigned int[countData];

	// gameID
	insertData[0] = handObj.getGameID();

	// handID
	insertData[1] = handObj.getID();

	// current number of players
	insertData[2] = handObj.getNSeated();

	handObj.getStacks(insertData + 3);

	string * insertStrs = new string[countData];
	NumbersToStrings<unsigned int>(insertStrs, insertData, countData);

	sql_helpers::insertRow(db,"SNAPSHOTS_" + NumberToString<unsigned int>(handObj.getNSeatsAtTable()), insertStrs, countData, false); 
	delete[] insertStrs;
	delete[] insertData;		
	}

// return a pointer to the first hand...check for null pointers on the way
rapidxml::xml_node<>* getFirstHand(rapidxml::xml_document<>* doc)
	{
	rapidxml::xml_node<>* descrNode = doc->first_node("description");

	assert(descrNode);

	rapidxml::xml_attribute<>* typeAttr = descrNode->first_attribute("type");

	assert(typeAttr);

	if (string(typeAttr->value()) != "Holdem Tournament")
		return 0;
	rapidxml::xml_node<>* firstHand = doc->first_node("game");
	assert(firstHand);
	return firstHand;
	}

template<ID_SITE siteID, typename nodeType>
void saveWinningsToSQL(sqlite3 * db, Hand<siteID, nodeType>& handObj, float * finalWinnings)
	{
	std::string * insertStrings = new string[handObj.getNSeatsAtTable() + 1];
	insertStrings[0] = NumberToString<unsigned int>(handObj.getGameID());
	NumbersToStrings<float>(insertStrings + 1, finalWinnings, handObj.getNSeatsAtTable());
	sql_helpers::insertRow(db, "WINNINGS_" + NumberToString<unsigned int>(handObj.getNSeatsAtTable()), insertStrings, handObj.getNSeatsAtTable() + 1);
	delete[] insertStrings;
	}

std::string parseGame(rapidxml::xml_document<>* doc, sqlite3* db, bool verbose)
	{
	rapidxml::xml_node<>* firstHand = getFirstHand(doc);

	if(!firstHand)
		return "Not a holdem game";

	Tourney_XML tourney(firstHand);
	TourneyHand<ARGS_MERGE> handObj(firstHand, &tourney, verbose);

	do {
		if (!handObj.isFirstInTourney() && handObj.hasStackInfo())
			saveSnapshotToSQL<ARGS_MERGE>(db, handObj);
		} while (handObj.advance(verbose));

	float * finalWinnings = new float[handObj.getNSeatsAtTable()];

	if (!handObj.isLastInTourney())
		// hand history doesn't go to end of tournament
		{
		handObj.updateStacksPostHand();
		handObj.getProjWinnings(finalWinnings);
		}
	else
		tourney.getWinnings(finalWinnings);

	saveWinningsToSQL<ARGS_MERGE>(db, handObj, finalWinnings);
	delete[] finalWinnings;	

	return "Success";
	}

int main()
	{
	char *zErrMsg = 0;
	int rc;
	sqlite3 * db;
	const char* data = "Callback function called";


	// Open database
	rc = sqlite3_open("../ICMTest.db", &db);
	sql_helpers::checkErrors(rc, zErrMsg, "database opening", true);

	// put your hand history directory here (set in preferences for the poker site, or for whatever poker tracking software you use)
	string dir = string("C://Users/Eric Walton/My Documents/PokerTracker 2/Processed/Merge");

	vector<std::string> files;

	int e = fileIO::addFiles(dir, files);

	if (e == -1)
		throw("Failed to open directory");

	srand(time(NULL));

	bool startingPoint = false;

	for (int i = 0; i < 1; i++)
		{
		char * buffer; 
		ifstream is;

		// for random testing
		//i = rand() % files.size();

		// for testing problem files
		//files[i] = "C://Users/Eric Walton/My Documents/PokerTracker 2/Processed/Merge/2012\
/08-20/%2A%2ANEW%2A%2A Hummingbird Room - 3 Minute Levels (60044778-1).xml";

		is.open (files[i], ios::binary );  // real

		cout<<endl<<endl<<"New file: "<<files[i]<<endl;


		if (!is.is_open())
			// failed to open file
			{
			std::cout << "Could not open file: " << std::endl;
			continue;
			}

		// get length of file:
		is.seekg (0, ios::end);
		unsigned int length = is.tellg();
		is.seekg (0, ios::beg);

		// allocate memory:
		try
			{
			buffer = new char [length+1];
			}
		catch (bad_alloc& ba)
			{
			cerr << endl<<"bad_alloc caught: " << ba.what() << endl;
			}

		// read data as a block:
		is.read (buffer,length);
		buffer[length]='\0';
		is.close();

		rapidxml::xml_document<> doc;
		doc.parse<0>(buffer);

		std::cout << parseGame(&doc, db, true) << std::endl;
		delete[] buffer;
		}

	sqlite3_close(db);

	return 0;

	}