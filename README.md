Poker-Parser
============

Parses hand history files for online poker games.  The project is geared towards tournaments, and towards Merge hand histories, but the class structure is designed to be extensible in both respects.
It is under development...as I have time to develop and clarify the code, I will annotate as well.

Dependencies
-----------

sqlite3, rapidxml


Description
-----------

main.cpp serves as an example application, recording into an sqlite3 database the stack sizes at the beginning of every hand, and the winnings after every tournament.  If the hand history file doesn't reach the end of the tournament, ICM values are substituted for the remaining players.  More detailed information can be extracted, especially by using the public methods of the Spot class to analyze each decision a player faces

There are a few inconsistencies in the way Merge encodes hand hsitories, and error handling has not been fully checked.   

Template Parameters
-------------------

Each of these templates takes two parameters, an ID for the site, and the type for the hand node of each.  For now, only the following template parameters, which are used in Merge hand histories, have been implemented:

```
<ID_MERGE, rapidxml::xml_node<>*>
```

Classes as Iterators
--------------------

The Hand and Spot classes function like iterators.  They point to a specific hand or spot in the tournament, and the API is designed so that the client calls Hand.advance() or Spot.advance() to progress through the game.  In fact, a few members of the Game object depend on where Hand is at the moment.

I'm open to suggestings of better ways of structuring this.

    
Class Structure
---------------
 
| Class | Description |
| ------ | ---------- |
| template<> class Hand | contains basic information about a hand: stack sizes, # of players etc.  also includes a method advance(), for advancing to the next hand in the history. | 
| template<> class TourneyHand: public Hand | contains Tournament-specific information and methods. |
| template<> class Game | basic info about the Game: # of players seated, etc. |
| template<> class Tourney : public Game | Tournament-specific info...prize structure, # of players, winnings so far |
| template<> Spot | refers to a "Spot", which, in poker lingo, is a decision a player faces.  Includes the current total in the pot, how much each player has contributed, what street this is, etc. |
                                  
                                  
Implemeting for HH Types
------------------------

Member fcns that parse the hand history files are given empty declarations in Hand.h, TourneyHand.h, Spot.h, etc.
The files Hand_merge.h, TourneyHand_merge.h, Spot_merge.h give template specializations for those functions, in the case of Merge hand history files.


Plans to Extend
---------------

I plan to add, for cash games:
  class CashGame : public Game
  
For HH files other than Merge, there will be a new set of header files with template specialization for different template arguments.  For example, they might be called:
```
  Hand_Fulltilt.h, Spot_Fulltilt.h
```  
 
Lastly, would like to include methods in Spot that allow for hypotheticals.  For instance,

```
  Spot.addaction(ID_PLAYER ID_ACTION)
```

would add a hypothetical action.  Then call

```
  Spot.awardpot()
```

to get the resulting stack sizes.  These could be used to analyze poker decisions.







                                  










