#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Ship;
class Actor;
class Enemy;
class NachenBlaster;
class Projectiles;
class Goody;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
	~StudentWorld();
    virtual int init();	//initializes a round: places stars and player, sets shipsRemaining and numRemaining
    virtual int move(); //handles all the things that should happen every tick
    virtual void cleanUp();	//remove objects and free memory

	bool collision(Actor* actor);	//handles collisions between every actor and the passed Actor pointer
	void newActor(Actor* actor);	//adds a new actor to actors
	bool isClose(Actor* actor, int deltaY) const;	//checks if the passed Actor pointer is within deltaY units of player's Y-coord

private:
	bool collide(Actor* a1, Actor* a2);	//checks to see if two objects are close enough to collide
	
	void shipCollision(NachenBlaster* p, Enemy* e);	//handles a collision between the player and an enemy ship
	void projectileCollision(Ship* s, Projectiles* p);	//handles a collision between any ship and a projectile
	void enemyDeath(Enemy* enemy);	//handles the death of an enemy ship
	void goodie(Goody* g);	//handles the collision between the player and a goodie

	int numEnemies; //number of enemies on the screen
	int shipsRemaining; //number of enemies that need to be destroyed to advance a level
	NachenBlaster* player;	//the NachenBlaster
	std::vector<Actor*> actors; //holds all the actors, excluding the player
};

double random(double min, double max);

#endif // STUDENTWORLD_H_
