#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
//AANISH PATEL SIKORA WEDNESDAY 9pm
//MODIFICATIONS BEGIN HERE
#include <vector>

class Actor;
class Player;
class BugSprayer;

class StudentWorld : public GameWorld
{
public:
	StudentWorld()
	{
		numZumi = 0; //There are no Zumi's initally. 
		//The constructor was not utilized as the init() method was more handy as three completely different studentworlds are created.
	}

	virtual int init(); //Initilizes the student world

	virtual int move(); //Called 20 times per second
	int getPX(); //Returns players X pos
	int getPY(); //Returns players Y pos
	int CZsearch() //Returns complex zumi search distance
	{
		return val12;
	}

	virtual void cleanUp(); //Destroys current student world object

	void addBugSprayer(int x, int y); //Adds a BugSprayer at point (x,y)
	void addLifeGoodie(int x, int y); //Adds a LifeGoodie at point (x,y)
	void addGhostGoodie(int x, int y); //Adds a GhostGoodie
	void addKillerGoodie(int x, int y); //Adds a Killer Goodie

	void killZumi(int x, int y); //Kills if there is a simple/complex zumi at point

	int betterCheck(int x, int y); //This checks to see what actor if any is at (x,y)
	void addBugSpray(int x , int y, BugSprayer * cur); //Adds Bug Spray to point
	void killActor(int x, int y); //Kills whatever actor is at point
	void showExit(); //Reveals Exit
	void killPlayer(); //Kills Player
	int getNumZumi() //Returns number of Zumi currently Alive
	{
		return numZumi;
	}
	void decZumi()
	{
		numZumi--;
	}
	void setDisplayText(); //Updates banner showing score,lives,etc.

	void decBonus() //Bonus decrements by 1 each turn
	{		
		if (val8!=0)
			val8--;	
	}

	unsigned int getProbOfGoodieOverall() const{return val1;}
	unsigned int getProbOfExtraLifeGoodie() const{return val2;}
	unsigned int getProbOfWalkThruGoodie() const{return val3;}
	unsigned int getProbOfMoreSprayersGoodie() const{return val4;}

	unsigned int getGoodieLifetimeInTicks() const{return val7;}
	unsigned int getComplexZumiSearchDistance() const{return val12;}

	unsigned int getLevelBonus() const{return val8;}
	unsigned int getWalkThruLifeTime() const{return val9;}
	unsigned int getSprayLifeTime() const{return val10;}
	unsigned int getMaxSpray() const{return val11;}
	virtual ~StudentWorld();

private:
	Player * m_player;	//Player point
	std::vector<Actor*> m_objects; //Vector of Actor pointers
	int numZumi;

	unsigned int val1; //optionProbOfGoodieOverall
	unsigned int val2;  //optionProbOfExtraLifeGoodie;
	unsigned int val3; //optionProbOfWalkThruGoodie;
	unsigned int val4; //optionProbOfMoreSprayersGoodie;
	unsigned int val5; //optionTicksPerSimpleZumiMove;
	unsigned int val6; //optionTicksPerComplexZumiMove;
	unsigned int val7; //optionGoodieLifetimeInTicks;

	unsigned int val8; //optionLevelBonus;
	unsigned int val9; //optionWalkThruLifetimeTicks;
	unsigned int val10; //optionBoostedSprayerLifetimeTicks;
	unsigned int val11; //optionMaxBoostedSprayers

	unsigned int val12; //complexZumiSearchDistance
};


#endif // STUDENTWORLD_H_
