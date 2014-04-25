#include "Actor.h"
#include "StudentWorld.h"
#include <queue>
#include <iostream>
using namespace std;
//AANISH PATEL SIKORA WEDNESDAY 9pm
//MODIFICATIONS BEGIN HERE
struct point //Linked List for CZ implementation. Parent points to previous point.
{
	pair<int,int> cord;
	point * parent;
};


void Player::doSomething()
	{
		if (!isAlive()){return;} //DO NOTHING IF DEAD
			
		int x = getX();
		int y = getY();

		if (getWorld()->getNumZumi()==0){getWorld()->showExit();}

		if (GhostTime()==0){setGhost(false,getWorld()->getWalkThruLifeTime());}
		if (KillTime()==0){giveLessKill();}
		if (isGhost()){decTGhost();}
		if (isKiller()){decTKill();}

		//FOR TESTING!!!!: COMMENT NEXT LINE FOR INVINCIBILITY 
		if (getWorld()->betterCheck(x,y)==IID_BUGSPRAY || getWorld()->betterCheck(x,y)==IID_SIMPLE_ZUMI || getWorld()->betterCheck(x,y)==IID_COMPLEX_ZUMI){getWorld()->killPlayer();}
		//If player touches bug spray, simple zumi or complex zumi, player dies
		if (!isGhost() && getWorld()->betterCheck(x,y) == IID_DESTROYABLE_BRICK){getWorld()->killPlayer();}
		if (getWorld()->betterCheck(x,y) ==IID_EXTRA_LIFE_GOODIE) //Grant Player more lives
		{
			getWorld()->playSound(SOUND_GOT_GOODIE);
			getWorld()->incLives(); //Fountain of Youth Activated
			getWorld()->increaseScore(1000); //PLAYER SCORE INCREASES BY X AMOUNT
			getWorld()->killActor(x,y); //KILL GOODIE
		}
		if (getWorld()->betterCheck(x,y) == IID_WALK_THRU_GOODIE) //Player can walk through dBricks
		{
			getWorld()->playSound(SOUND_GOT_GOODIE);
			setGhost(true,getWorld()->getWalkThruLifeTime()); //Ghost Mode Activated
			getWorld()->increaseScore(1000); //PLAYER SCORE INCREASES BY X AMOUNT
			getWorld()->killActor(x,y);
		}
		if (getWorld()->betterCheck(x,y) == IID_INCREASE_SIMULTANEOUS_SPRAYER_GOODIE) //Player can drop more sprayers
		{
			getWorld()->playSound(SOUND_GOT_GOODIE);
			giveMoreKill(getWorld()->getSprayLifeTime(),getWorld()->getMaxSpray()); //Schwarzenegger Mode Activated
			getWorld()->increaseScore(1000); 
			getWorld()->killActor(x,y);
		}
		int ch;

		if (getWorld()->getKey(ch)) //Player input
		{				
			if (ch== KEY_PRESS_LEFT)
			{
				if (canMove(x-1,y)){moveTo(x-1,y);} //If valid, move LEFT
			}
			if(ch == KEY_PRESS_RIGHT)
			{
				if (canMove(x+1,y)){moveTo(x+1,y);} //If valid, move RIGHT
			}
			if (ch== KEY_PRESS_DOWN)
			{
				if (canMove(x,y-1)){moveTo(x,y-1);} //If valid, move DOWN
			}
			if(ch == KEY_PRESS_UP)
			{
				if (canMove(x,y+1)){moveTo(x,y+1);} //If valid, move UP
			}				
			if (ch== KEY_PRESS_SPACE)
			{
				if (numBS()) //If Player hasn't already dropped max number of sprayers
				{
					getWorld()->addBugSprayer(x,y);
					addBS();
				}
			}
		}	
	}

bool Player::canMove(int x, int y) //Checks if moving to (x,y) is valid
{
	if (!isGhost()) //Cannot walk through dBricks
	{
		if (getWorld()->betterCheck(x,y)!=IID_PERMA_BRICK && getWorld()->betterCheck(x,y)!=IID_DESTROYABLE_BRICK)
		{
			return true;
			//Can only walk if empty,exit or goodie
		}	
	}
	else 
	{
		if (getWorld()->betterCheck(x,y)!=IID_PERMA_BRICK)
		{
			return true;
			//Can walk through anything except pBricks
		}
	}
	return false;
}

void Zumi::move() //Zumi picks random direction, moves as much as possible, then re-picks
{
	int x = getX();
	int y = getY();
	if (getSpeed()<0){resetWait();} //Time reset to max.
	if (getSpeed()==0) //Once every x ticks
	{
		if (getDir()==0 && getWorld()->betterCheck(x-1,y) ==0) //LEFT
		{
			moveTo(x-1,y);
		}
		else if (getDir()==1 && getWorld()->betterCheck(x+1,y) ==0 ) //RIGHT
		{
			moveTo(x+1,y);
		}
		else if (getDir()==2 && getWorld()->betterCheck(x,y-1) ==0) //DOWN
		{
			moveTo(x,y-1);
		}
		else if (getDir()==3 && getWorld()->betterCheck(x,y+1) ==0) //UP
		{
			moveTo(x,y+1);
		}
		else
		{
			setDir(rand() % 4); //Switch direction to new random number
			resetWait(); //Zumi must wait full number of ticks again
		}
	}
	decWait();

}

void SimpleZumi::doSomething()
{
	int x = getX();
	int y = getY();
	
	if (!isAlive()){return;} //Can't do anything if dead
	if (getWorld()->betterCheck(x,y) == IID_BUGSPRAY) 
	{
		getWorld()->playSound(SOUND_ENEMY_DIE);
		getWorld()->killZumi(x,y);
		//Bug Spray kills Zumis
	}
	Zumi::move();
}

void BugSprayer::doSomething()
{
	if (!isAlive()){return;} //Can't do anything if dead. 
	if (getTime()>0){decTime();} //Timer till D-day
	int x = getX();
	int y = getY();
	if (getWorld()->betterCheck(x,y)==IID_BUGSPRAY){setTime(0);} //CHAIN REACTION
	if (getTime()==0) //Timer has run down
	{
		//Bug sprayer adds bug spray in + formation, two squares each way. A pBrick will halt progress of bug spray
		getWorld()->playSound(SOUND_SPRAY);
		getWorld()->addBugSpray(x,y,this);
		getWorld()->addBugSpray(x+1,y,this); 
		if (getWorld()->betterCheck(x+1,y) != IID_PERMA_BRICK) 
			getWorld()->addBugSpray(x+2,y,this);

		getWorld()->addBugSpray(x,y+1,this); 
		if (getWorld()->betterCheck(x,y+1) != IID_PERMA_BRICK)
		getWorld()->addBugSpray(x,y+2,this);

		getWorld()->addBugSpray(x-1,y,this); 
		if (getWorld()->betterCheck(x-1,y) != IID_PERMA_BRICK)
			getWorld()->addBugSpray(x-2,y,this);

		getWorld()->addBugSpray(x,y-1,this); 
		if (getWorld()->betterCheck(x,y-1) != IID_PERMA_BRICK)
			getWorld()->addBugSpray(x,y-2,this);

		kill(); //Zumi is dead now
	}
}

void BugSpray::doSomething()
{
	if (!isAlive()){return;} //Can't do anything if dead
	decTime(); //Time left alive
	int x = getX();
	int y = getY();

	if (getTime()==0) //Bug Spray no longer Active
	{
		setVisible(false);
		kill();
	}
	else //Bug Spray will kill dBricks and Players
	{
		if (getWorld()->betterCheck(x,y) == IID_DESTROYABLE_BRICK){getWorld()->killActor(x,y);}
		else if (getWorld()->betterCheck(x,y) == IID_PLAYER){getWorld()->killPlayer();}
	}
}

void ComplexZumi::doSomething()
{
	int x = getX();
	int y = getY();
	if (!isAlive()){return;} //Can't do anything if dead
	if (getWorld()->betterCheck(x,y) == IID_PLAYER){getWorld()->killPlayer();} //Complex Zumi will aggressively move towards player and kill it
	//Bug Spray kills CZ
	if (getWorld()->betterCheck(x,y) == IID_BUGSPRAY)
	{
		getWorld()->killZumi(x,y);
	}

	int px = getWorld()->getPX();
	int py = getWorld()->getPY();

	int hDist; //Horizontal distance to player
	int vDist; //Vertical distance to player

	if (x>px){hDist = x-px;}
	else{hDist = px-x;}

	if (y>py){vDist = y-py;}
	else{vDist = py-y;}

	int sDist = getWorld()->getComplexZumiSearchDistance(); //Max. sniffing distance specified by level parameters
	if (getSpeed()<0){resetWait();} 
	if (hDist <= sDist && vDist <=sDist) //If CZ can smell player
	{
		int dirx;
		int diry;
		
		if (pathExists(px,py,x,y,dirx,diry)) //Check if a path exists between the player and zumi. if so, it sets dirx and diry to first move CZ should make
		{
			if (getSpeed()==0) //CZ only moves once every x number of ticks
			{
				int a = dirx - x; //X Distance between suggested move and current location 
				int b = diry - y; //Y Distance between suggested move and current location 
				//CZ will move in suggested direction if it is valid
				if (a==1 && getWorld()->betterCheck(x+1,y)==0)
				{
					moveTo(x+1,y);
				}
				else if (a==-1 && getWorld()->betterCheck(x-1,y)==0)
				{
					moveTo(x-1,y);
				}
				if (b==1 && getWorld()->betterCheck(x,y+1)==0)
				{
					moveTo(x,y+1);
				}
				else if (b==-1 && getWorld()->betterCheck(x,y-1)==0)
				{
					moveTo(x,y-1);
				}
				decWait();
			}
		}
		else //If no valid path, the CZ moves like simple Zumi
		{
			Zumi::move();
		}
	}
	//else //If CZ cannot smell player, it will move like simple zumi
	//{
	//	Zumi::move();
	//}
	Zumi::move();
	decWait();	
}

//CZ HELPER FUNCTION #2:
//MARKS POINT AND CREATES LINKED LIST BETWEEN POINTS
void explore(char maze[][VIEW_HEIGHT], queue<point*>& toDo, int x, int y, point * a)
{
	if (maze[y][x] != 'X' && maze[y][x]!='-')
    {
			point * s = new point;
			s->cord = make_pair(y,x);
			s->parent = a; //CURRENT POINT WILL REMEMBER THE POINT THAT LED TO IT
            toDo.push(s);
            maze[y][x] = '-'; 
    }
}

//CZ HELPER FUNCTION #1:
//CHECKS IF THERE EXISTS A VALID PATH BETWEEN PLAYER POS AND CZ POS
//IF THERE IS PATH, SETS rx and ry TO FIRST MOVE ZUMI SHOULD MAKE
bool ComplexZumi::pathExists(int playerx, int playery, int cx, int cy,int &rx,int&ry) 
{
	char maze[VIEW_WIDTH][VIEW_HEIGHT];
	//INITIALIZE THE MAZE TO REFLECT WHAT THE CURRENT BOARD LOOKS LIKE
	for (int i = VIEW_WIDTH-1 ; i >=0 ; i--)
	{
		for (int j = VIEW_HEIGHT-1 ; j >=0; j--)
		{
			if (getWorld()->betterCheck(j,i) == IID_DESTROYABLE_BRICK || getWorld()->betterCheck(j,i) ==IID_PERMA_BRICK)
			{
				maze[i][j] = 'X';
			}
			else{maze[i][j] = '.';}
		}
	}
	
	bool exitFound = false;
	//EACH POINT HAS (x,y) AND A POINTER TO ANOTHER POINT
	queue<point *> toDo;

	if (cx==playerx && cy==playery){return true;} 
	//INTIALIZES NEW POINT TO BE STARTING LOCATION. PARENT NODE IS NULL INDICATES THIS IS THE FIRST POINT
	point *s = new point;
	s->cord = make_pair(cy,cx);
	s->parent = nullptr;
	toDo.push(s);
	maze[cy][cx] = '-'; //MARK FIRST LOCATION HAS VISITED

	while (!toDo.empty() && !exitFound)
	{
		s = toDo.front();
		toDo.pop();

		const int ccx = s->cord.second;
		const int ccy = s->cord.first;

		if (ccx == playerx && ccy == playery)
		{
			maze[ccy][ccx] = '>';
			exitFound = true;
		}
		else
		{
			explore(maze,toDo,ccx-1,ccy,s);
			explore(maze,toDo,ccx+1,ccy,s);
			explore(maze,toDo,ccx,ccy-1,s);
			explore(maze,toDo,ccx,ccy+1,s);
		}
	}
	//THIS LOOP ITERATES THROUGH THE SHORTEST POSSIBLE PATH BETWEEN CZ AND PLAYER
	while (s->parent->parent !=nullptr)
	{
		maze[s->cord.first][s->cord.second] = 'A';
		s = s->parent;
	}
	if (exitFound==true) 
	{
		//THIS IS THE DIRECTION CZ SHOULD MOVE IN
		rx = s->cord.second;
		ry = s->cord.first;
	}
	return exitFound;
}
