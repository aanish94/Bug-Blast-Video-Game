#include "StudentWorld.h"
#include "GameWorld.h"
#include "GraphObject.h"
#include "Level.h"
#include "Actor.h" 
#include <sstream>
#include <iomanip>
using namespace std;
#include <cstdlib>
#include "GameConstants.h"
#include <vector>

GameWorld* createStudentWorld()
{
	return new StudentWorld();
}
//AANISH PATEL SIKORA WEDNESDAY 9pm
//MODIFICATIONS BEGIN HERE

int StudentWorld::init() //INITIAZLIZES GAME WORLD
 {
		Level lev;
		int curLev = getLevel();
		ostringstream tl;
		tl << "level0";
		tl << curLev;
		tl << ".dat";
		string abc = tl.str();

		Level::LoadResult result = lev.loadLevel(abc);

		if (getLevel()==0 && result == Level::load_fail_file_not_found)
			return GWSTATUS_NO_FIRST_LEVEL;
		if (getLevel()!=0 && result == Level::load_fail_file_not_found)
			return GWSTATUS_PLAYER_WON;
		if (result == Level::load_fail_bad_format)
			return GWSTATUS_LEVEL_ERROR;

		//LOAD LEVEL PARAMETERS
		val1 = lev.getOptionValue(optionProbOfGoodieOverall);
		val2 = lev.getOptionValue(optionProbOfExtraLifeGoodie);
		val3 = lev.getOptionValue(optionProbOfWalkThruGoodie);
		val4 = lev.getOptionValue(optionProbOfMoreSprayersGoodie);
		val5 = lev.getOptionValue(optionTicksPerSimpleZumiMove);
		val6 = lev.getOptionValue(optionTicksPerComplexZumiMove);
		if (val6<10){val6=10;} //SETTING THIS VALUE BELOW 10 causes the complex zumis to not move
		val7 = lev.getOptionValue(optionGoodieLifetimeInTicks);
		val8 = lev.getOptionValue(optionLevelBonus);
		val9 = lev.getOptionValue(optionWalkThruLifetimeTicks);
		val10 = lev.getOptionValue(optionBoostedSprayerLifetimeTicks);
		val11 = lev.getOptionValue(optionMaxBoostedSprayers);
		val12 = lev.getOptionValue(optionComplexZumiSearchDistance);		

		for (int i = VIEW_WIDTH-1 ; i >=0 ; i--) //ITERATE THROUGH LEVEL DATA AND CREATE ACTORS
		{
			for (int j = VIEW_HEIGHT-1 ; j >=0; j--)
			{
				Level::MazeEntry ge = lev.getContentsOf(i,j);
				if (ge==Level::perma_brick)
				{
					m_objects.push_back(new PBrick(i,j));					
				}
				else if (ge==Level::destroyable_brick)
				{
					m_objects.push_back(new DBrick(i,j));
				}
				else if (ge==Level::player)
				{
					m_player = new Player(i,j,this,val11);
				}
				else if (ge==Level::complex_zumi)
				{
					m_objects.push_back(new ComplexZumi(i,j,this,val6));
					numZumi++;
				}
				else if (ge==Level::simple_zumi)
				{
					m_objects.push_back(new SimpleZumi(i,j,this,val5));
					numZumi++;
				}
				else if (ge==Level::exit)
				{
					m_objects.push_back(new GameExit(i,j));
				}
			}
		}

		return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
   {
	   //FOR EVERY ALIVE ACTOR AND PLAYER, ALLOW THEM TO DO SOMETHING
		m_player->doSomething();
		for (unsigned int i = 0 ; i < m_objects.size() ; i++)
		{
			m_objects[i]->doSomething();
		}
		if (!m_player->isAlive()) //IF THE PLAYER IS DEAD, RETURN 
		{
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		//IDENTIFY DEAD OBJECTS AND REMOVE FROM GAME
		for (unsigned int i = 0 ; i < m_objects.size() ; i++)
		{
			if (!m_objects[i]->isAlive())
			{
				int x = m_objects[i]->getX();
				int y = m_objects[i]->getY();
				killActor(x,y);
			}
		}
		//UPDATE DISPLAY TEST
		setDisplayText();
		//BONUS DECREASES BY 1 EACH TICK
		decBonus();
		//REVEAL EXIT IF THERE ARE NO ZUMI
		if (getNumZumi()==0)
		{
			int x = m_player->getX();
			int y = m_player->getY();
			if (betterCheck(x,y)==IID_EXIT)
			{
				increaseScore(val8);
				playSound(SOUND_FINISHED_LEVEL);
				return GWSTATUS_FINISHED_LEVEL;
				//PLAYER WINS IF THEY STEP ON EXIT
			}
		}
        return GWSTATUS_CONTINUE_GAME;
    }

void StudentWorld::cleanUp() //DESTROYS CURRENT ITERATION OF GAME WORLD
{
	delete m_player;
	for (unsigned int i = 0 ; i < m_objects.size() ; i++)
	{
		m_objects[i]->setVisible(false);
   }
	while (!(m_objects.empty()))
	{
		m_objects.pop_back();
	}
	numZumi = 0;
}

void StudentWorld::setDisplayText() //Score: 00000100 Level: 09 Lives: 003 Bonus: 345
{
		int score = getScore();
		int level = getLevel();
		unsigned int bonus = val8;
		int livesLeft = getLives();
		ostringstream first;
		first << "Score:  ";
		first.fill('0');
		first << setw(8) << score;
		first << "  Level:  ";
		first << setw(2) << level;
		first << "  Lives:  ";
		first << setw(3) << livesLeft;
		first << "  Bonus:  ";
		first.fill(' ');
		first << setw(6) << bonus;
		string s = first.str();

		setGameStatText(s);
}

void StudentWorld::killActor(int x, int y)
{
	//IF ACTOR IS ZUMI, CALL SPECIFIC FUNCTION
	killZumi(x,y);
	for (unsigned int i = 0 ; i < m_objects.size(); i++)
	{
		if (m_objects[i]->getX()==x && m_objects[i]->getY()==y)
		{
			if (m_objects[i]->newGetID() == IID_BUGSPRAYER)
			{
				m_player->removeBS();
			}
			m_objects[i]->kill();
			m_objects[i]->setVisible(false);
			delete m_objects[i];
			m_objects.erase(m_objects.begin()+i);
		}
	}
}

void StudentWorld::killZumi(int x, int y)
{
	bool goodie = false;
	for (unsigned int i = 0 ; i < m_objects.size(); i++)
	{
		if (m_objects[i]->getX()==x && m_objects[i]->getY()==y && m_objects[i]->newGetID() ==IID_SIMPLE_ZUMI)
		{
			//INCREASE SCORE IF SIMPLE ZUMI IS KILLED
			increaseScore(100);
			decZumi();
			m_objects[i]->kill();
			m_objects[i]->setVisible(false);
			m_objects.erase(m_objects.begin()+i);
			goodie = true;
			//CHANCE OF GOODIE DROP IS THERE
		}
		if (m_objects[i]->getX()==x && m_objects[i]->getY()==y && m_objects[i]->newGetID() ==IID_COMPLEX_ZUMI)
		{
			//THE SCORE IS INCREASED BY x FOR KILLING COMPLEX ZUMI
			increaseScore(500);
			decZumi();
			m_objects[i]->kill();
			m_objects[i]->setVisible(false);
			m_objects.erase(m_objects.begin()+i);	
			goodie = true;
			//THERE IS CHANCE OF GOODIE DROP
		}
	}
	//IF A ZUMI WAS KILLED AND POSITION IS VALID, THERE IS A CHANCE A GOODIE WILL BE DROPPED
	if (goodie)
	{
		if (betterCheck(x,y)==0 || betterCheck(x,y)==IID_BUGSPRAY)
		{
			unsigned int c = rand() % 100;
			if (c < val1) //val1 is probability of a goodie drop
			{
				addGhostGoodie(x,y);
				addLifeGoodie(x,y);
				addKillerGoodie(x,y);
			}
		}
	}
	//CHECK IF THERE ARE NO MORE ZUMI, IF SO, SHOW THE EXIT
	if (getNumZumi()==0){showExit();}
}	

int StudentWorld::betterCheck(int x, int y) 
{
	//IDENTIFIES WHAT ACTOR IS AT POISITION X, Y
	int a = 0;
	for (unsigned int i = 0 ; i < m_objects.size() ; i++)
	{
		if (m_objects[i]->getX()== x && m_objects[i]->getY()==y)
		{
			a = m_objects[i]->newGetID();		
		}
	}
	return a;
	//0 DENOTES EMPTY
}

void StudentWorld::addBugSpray(int x, int y, BugSprayer * cur)
{
	//IF POSITION IS EMPTY OR AVAILABLE, DROP A VISIBLE BUG SPRAY
	if (betterCheck(x,y)==0 || betterCheck(x,y)==IID_BUGSPRAYER)
	{
		BugSpray * temp = new BugSpray(x,y,this, cur,3);
		m_objects.push_back(temp);
	}
	else if (betterCheck(x,y)==IID_PERMA_BRICK || betterCheck(x,y)==IID_EXTRA_LIFE_GOODIE || betterCheck(x,y)==IID_INCREASE_SIMULTANEOUS_SPRAYER_GOODIE || betterCheck(x,y)==IID_WALK_THRU_GOODIE)
	{
		//DO NOT DROP A BUG SPRAY ON THESE ACTORS
	}
	else
	{
		BugSpray * temp = new BugSpray(x,y,this,cur,3);
		m_objects.push_back(temp);
		temp->setVisible(false);
		//DROP AN INVISIBLE BUG SPRAY
	}
}

void StudentWorld::addKillerGoodie(int x, int y)
{
	//IF POSITION IS AVAILABLE, DROP A KILLER GOODIE AND KILL WHATEVER WAS THERE
	unsigned int c = rand() % 100;
	if (c<val4)
	{
		if (betterCheck(x,y) == IID_BUGSPRAY && betterCheck(x,y) !=IID_SIMPLE_ZUMI){killActor(x,y);}
		Killer * temp = new Killer(x,y,this,val7);
		m_objects.push_back(temp);
	}
}

void StudentWorld::addGhostGoodie(int x, int y)
{
	//IF POSITION IS AVAILABLE, DROP A GHOST GOODIE AND KILL WHATEVER WAS THERE
	unsigned int c = rand() % 100;
	if (c<val3) //Prob of walk thru goodie
	{
		if (betterCheck(x,y) == IID_BUGSPRAY && betterCheck(x,y) !=IID_SIMPLE_ZUMI){killActor(x,y);}
		Ghost * temp = new Ghost(x,y,this,val7);
		m_objects.push_back(temp);
	}
}

void StudentWorld::addLifeGoodie(int x, int y)
{
	//IF POSITION IS AVAILABLE, DROP A LIFE GOODIE AND KILL WHATEVER WAS THERE
	unsigned int c = rand() % 100;
	if (c<val2) //Prob of life goodie
	{
		if (betterCheck(x,y) == IID_BUGSPRAY && betterCheck(x,y)!=IID_SIMPLE_ZUMI){killActor(x,y);}
		LifeGoodie * temp = new LifeGoodie(x,y,this,val7);
		m_objects.push_back(temp);
	}
}

void StudentWorld::addBugSprayer(int x, int y)
{
	//ADD A BUG SPRAYER WITH LIFETIME EQUAL TO x
	m_objects.push_back(new BugSprayer(x,y,this,40));
}

int StudentWorld::getPX()
{
	//PLAYERS X pos
	return m_player->getX();
}

int StudentWorld::getPY()
{
	//PLAYERS Y pos
	return m_player->getY();
}

void StudentWorld::showExit()
{
	//REVEAL EXIT
	for (unsigned int i = 0 ; i < m_objects.size(); i++)
	{
		if (m_objects[i]->newGetID() == IID_EXIT)
		{
			if (!(m_objects[i]->isVisible())){playSound(SOUND_REVEAL_EXIT);}
			m_objects[i]->setVisible(true);
		}
	}
}

void StudentWorld::killPlayer()
{
	//KILL PLAYER
	playSound(SOUND_PLAYER_DIE);
	m_player->kill();
	m_player->setVisible(false);
}

StudentWorld::~StudentWorld()
{
	//DESTRUCTOR
	delete m_player;
	for (unsigned int i = 0 ; i < m_objects.size() ; i++)
	{
		delete m_objects[i];
	}
}