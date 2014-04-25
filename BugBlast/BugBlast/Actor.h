#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
//AANISH PATEL SIKORA: WEDNESDAY 9pm
//MODIFICATIONS BEGIN HERE
#include "GameConstants.h"

class StudentWorld; //STUDENT WORLD POINTER USED

//---------------------ACTOR CLASS-------------------------
//Abstract Base Class. All Game objects derived from here.
//---------------------------------------------------------
class Actor : public GraphObject 
{
	public:
		Actor(int imageID, int startX, int startY) : GraphObject(imageID,startX,startY)
		{
			m_alive = true;		
		}
		virtual void doSomething()=0; 
		virtual bool isAlive()
		{
			return m_alive;
		}
		virtual void kill()
		{
			m_alive = false;
		}
		StudentWorld * getWorld()
		{
			return m_pWorld;
		}
		void setWorld(StudentWorld * p)
		{
			m_pWorld = p;
		}
		virtual unsigned int newGetID()=0;
		virtual ~Actor()
		{
			//delete m_pWorld;
		}
	private:
		bool m_alive;
		StudentWorld * m_pWorld;
};

//---------------------PLAYER CLASS-------------------------
//Main User Class. 
//---------------------------------------------------------
class Player : public Actor
{
	public:
		Player(int startX, int startY, StudentWorld * c, int maxS) : Actor(IID_PLAYER,startX,startY)
		{
			setVisible(true);
			setWorld(c);
			numBugS = 0; //Number of Currently Dropped Bug Sprayers
			ghost = false; //Cannot Walk Through Walls by default
			mS = 2; //Maximum Number of Sprayers
			lives = 3; 
			killer = false;
		}
		virtual void doSomething();
		virtual unsigned int newGetID()
		{
			return IID_PLAYER;
		}
		void giveMoreKill(int wTime,int mSpray) //Increase max number of Sprayers
		{
			tKill = wTime;
			mS = mSpray;
			killer = true;
		}
		bool isKiller()
		{
			return killer;
		}
		void giveLessKill()
		{
			mS = 2;
			killer = false;
		}
		bool numBS() //Checks if player can drop more Bug Sprayers
		{
			int i = mS;
			if (numBugS < i){return true;}
			return false;
		}
		void addBS()
		{
			numBugS++;
		}
		int getPLives()
		{
			return lives;
		}
		void incPLives()
		{
			lives++;
		}
		virtual void kill()
		{
			Actor::kill();
			lives--;
		}
		void removeBS()
		{
			numBugS--;
		}
		bool isGhost() //Check if can walk through walls
		{
			return ghost;
		}
		void setGhost(bool a,int gTime) //Make player a ghost (walk through walls)
		{
			ghost = a;
			if (ghost){tGhost = gTime;}
		}
		int GhostTime(){return tGhost;}
		int KillTime(){return tKill;}
		void decTKill(){tKill--;}
		void decTGhost(){tGhost--;}
		bool canMove(int x, int y);
		virtual ~Player(){}
	private:
		int numBugS;
		bool ghost;
		int mS;
		int lives;
		bool killer;
		int tKill;
		int tGhost;
};

class PBrick : public Actor
{
	public:
		PBrick(int startX,int startY) : Actor(IID_PERMA_BRICK,startX,startY)
		{
			setVisible(true);
		}

		virtual void doSomething(){} //Brick's Dont Do Anything
		virtual unsigned int newGetID()
		{
			return IID_PERMA_BRICK;
		}
		virtual ~PBrick(){}
};

class DBrick : public Actor
{
	public:
		DBrick(int startX, int startY) : Actor(IID_DESTROYABLE_BRICK,startX,startY)
		{
			setVisible(true);
		}
		virtual void doSomething(){} //Bricks' Dont Do Anything

		virtual unsigned int newGetID()
		{
			return IID_DESTROYABLE_BRICK;
		}
		virtual ~DBrick(){}
	private:
};

//---------------------ZUMI CLASS-------------------------
//Main Zumi Class. Simple and Complex derived from here
//---------------------------------------------------------
class Zumi : public Actor
{
	public:
		Zumi(int imageID, int startX,int startY, StudentWorld * c, unsigned int speed) : Actor(imageID,startX,startY)
		{
			setWorld(c);
			curDir = rand() % 4; //Arbitrarily picks Left, Right, Up or Down
			m_t = speed; //This is the number of ticks per move
			curSpeed = speed; //This is the number of ticks per move
		}
		virtual void move();
		virtual void setDir(int a) //Sets direction to either Left(0), Right(1), Up(2) or Down(3)
		{
			curDir = a;
		}
		virtual int getDir()
		{
			return curDir;
		}
		virtual int getSpeed()
		{
			return m_t;
		}
		void decWait() //The zumi does nothing for x-1 turns, then moves and repeat.
		{
			m_t--;
		}
		void resetWait() //If zumi changes direction, the timer is reset
		{
			m_t = curSpeed;
		}
		virtual ~Zumi(){}
	private:
		int curDir;
		int m_t;
		int curSpeed;
};

class SimpleZumi : public Zumi
{
	public:
		SimpleZumi(int startX, int startY,StudentWorld * c,int speed) : Zumi(IID_SIMPLE_ZUMI,startX,startY,c,speed)
		{
			setVisible(true);		
		}
		virtual void doSomething();
		virtual unsigned int newGetID()
		{
			return IID_SIMPLE_ZUMI;
		}	

		virtual ~SimpleZumi(){}
};

class ComplexZumi : public Zumi
{
	public:
		ComplexZumi(int startX, int startY,StudentWorld * c,int speed) : Zumi(IID_COMPLEX_ZUMI,startX,startY,c,speed)
		{
			setVisible(true);			
		}
		virtual void doSomething();

		virtual unsigned int newGetID()
		{
			return IID_COMPLEX_ZUMI;
		}
		//These are helper functions to determine CZ's movement
		bool pathExists(int playerx, int playery, int cx, int cy,int &rx,int&ry);
		//void explore(char maze[][VIEW_HEIGHT], queue<point*>& toDo, int r, int c, point * a);

		virtual ~ComplexZumi(){}
};

//-----------------DOOMED TO DIE CLASS---------------------
//ABSTRACT BASE CLASS: Main Timed Actor Class. 
//---------------------------------------------------------
class DoomedtoDie : public Actor
{
	public:
		DoomedtoDie(int imageID, int startX, int startY, StudentWorld * c, int timeToD) : Actor(imageID, startX, startY)
		{
			setWorld(c);
			m_timeToD = timeToD; //Number of ticks actor is alive for. This depends on specific object(ex: Bug Spray has life of 3 ticks)
		}
		virtual void doSomething()=0;
		virtual unsigned int newGetID()=0;
		virtual void decTime()
		{
			m_timeToD--;
		}
		virtual int getTime()
		{
			return m_timeToD;
		}
		virtual void setTime(int a)
		{
			m_timeToD = a;
		}
		virtual ~DoomedtoDie(){}
	private:
		int m_timeToD;
};

class BugSprayer : public DoomedtoDie
{
	public:
		BugSprayer(int startX, int startY, StudentWorld * c,int t) : DoomedtoDie(IID_BUGSPRAYER, startX,startY,c,t)
		{
			setVisible(true);
		}
		virtual void doSomething();

		virtual unsigned int newGetID()
		{
			return IID_BUGSPRAYER;
		}
		virtual ~BugSprayer(){}
};

class BugSpray : public DoomedtoDie
{
	public:
		BugSpray(int startX, int startY, StudentWorld * c, BugSprayer * a,int t) : DoomedtoDie(IID_BUGSPRAY, startX, startY,c,t)
		{
			setVisible(true);
			m_bs = a;
		}
		virtual void doSomething();
		
		virtual unsigned int newGetID()
		{
			return IID_BUGSPRAY;
		}

		BugSprayer * getBS() //This function sets a pointer to the bugsprayer that created the bugspray
		{
			return m_bs;
		}
		virtual ~BugSpray(){}
	private:
		BugSprayer * m_bs;
};

class GameExit : public Actor
{
	public:
		GameExit(int startX,int startY) : Actor(IID_EXIT,startX,startY)
		{
			setVisible(false); //Exit is only actor that is constructed being invisible. Exit is only revelead when no zumis are left
		}
		virtual unsigned int newGetID()
		{
			return IID_EXIT;
		}
		virtual void doSomething(){} //An exit cannot do anything
		virtual ~GameExit(){}
	private:
};

//-----------------GOODIE CLASS---------------------
//Main Goodie Class. 
//---------------------------------------------------------
class Goodie : public DoomedtoDie
{
	public:
		Goodie(int imageID, int startX, int startY, StudentWorld * c,int t) : DoomedtoDie(imageID, startX, startY,c,t){}  //t is the time goodie is alive for
		virtual void doSomething() //Checks to see if lifetime has past. 
		{
			if (getTime()==0)
			{
				setVisible(false);
				kill();
			}
			decTime();
		}		

		virtual ~Goodie(){}
};

class LifeGoodie : public Goodie //GRANTS PLAYER EXTRA LIFE
{
	public:
		LifeGoodie(int startX, int startY, StudentWorld * c,int lt) : Goodie(IID_EXTRA_LIFE_GOODIE, startX,startY,c,lt)
		{
			setVisible(true);
		}
		virtual unsigned int newGetID()
		{
			return IID_EXTRA_LIFE_GOODIE;
		}
		virtual ~LifeGoodie(){}
};

class Ghost : public Goodie //LETS PLAYER WALK THROUGH WALLS
{
	public:
		Ghost(int startX, int startY, StudentWorld * c,int gt) : Goodie(IID_WALK_THRU_GOODIE, startX, startY,c,gt)
		{
			setVisible(true);
		}
		virtual unsigned int newGetID()
		{
			return IID_WALK_THRU_GOODIE;
		}
		virtual ~Ghost(){}
};

class Killer : public Goodie //GRANTS PLAYER MORE BUG SPRAYERS
{
	public:
		Killer(int startX, int startY, StudentWorld * c,int st) : Goodie(IID_INCREASE_SIMULTANEOUS_SPRAYER_GOODIE, startX, startY,c,st)
		{
			setVisible(true);
		}
		virtual unsigned int newGetID()
		{
			return IID_INCREASE_SIMULTANEOUS_SPRAYER_GOODIE;
		}
		virtual ~Killer(){}
};
#endif // ACTOR_H_
