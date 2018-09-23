#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>
#include <sstream>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(string assetDir)
: GameWorld(assetDir)
{
	shipsRemaining = 6 + (4 * getLevel());
	numEnemies = 0;
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}

int StudentWorld::init()
{
	srand(time(0)); //seed the rng

	shipsRemaining = 6 + (4*getLevel());
	numEnemies = 0;

	player = new NachenBlaster(this); //place player

	for (int i = 0; i < 30; i++) //place 30 stars
		actors.push_back(new Star(randInt(0, VIEW_WIDTH - 1), randInt(0, VIEW_HEIGHT - 1), this, 0, random(.05, .51)));	

    return GWSTATUS_CONTINUE_GAME;
}

bool StudentWorld::isClose(Actor* actor, int deltaY) const
{
	//returns true if player is to the left of actor and within 4 units on the Y-axis
	return (player->getX() < actor->getX() && (actor->getY() >= player->getY() - deltaY && actor->getY() <= player->getY() + deltaY ));
}

bool StudentWorld::collide(Actor* a1, Actor* a2)
{
	//returns true if the distance between the actors are less than .75 * (sum of radii)
	return (sqrt(pow(a1->getX() - a2->getX(), 2) + pow(a1->getY() - a2->getY(), 2)) < .75 * (a1->getRadius() + a2->getRadius()));
}

void StudentWorld::enemyDeath(Enemy* enemy)
{
	//cerr << enemy << " " << time(NULL) << endl;

	enemy->setStatus(true);	//set Enemy to dead
	increaseScore(enemy->pointValue());
	playSound(SOUND_DEATH);
	actors.push_back(new Explosion(enemy->getX(), enemy->getY(), this)); //EXPLOSION!
	shipsRemaining--;

	int x = randInt(1, 6);	//randomInt from 1 to 6
	if (dynamic_cast<Smoregon*>(enemy) != nullptr) //enemy is a Smoregon
	{
		if (x == 1)
			actors.push_back(new gRepair(enemy->getX(), enemy->getY(), this)); // 1/6 chance for Repair
		else if (x == 2)
			actors.push_back(new gTorpedo(enemy->getX(), enemy->getY(), this)); // 1/6 chance for Torpedos
	}
	else if (dynamic_cast<Snagglegon*>(enemy) != nullptr) //enemy is a Snagglegon
	{
		if (x == 1)
			actors.push_back(new gLife(enemy->getX(), enemy->getY(), this)); // 1/6 chance for extra Life
	}
}

void StudentWorld::shipCollision(NachenBlaster* p, Enemy* e)
{
	if (e->done())	//if the enemy is already dead, do nothing
		return;
	p->alterHP(e->crashValue()); //lower the player's HP
	if (p->getHP() <= 0)	//if the hit kills the player, mark the player as dead
		p->setStatus(true);
	enemyDeath(e);	//destroy the enemy
}

void StudentWorld::projectileCollision(Ship* s, Projectiles* p)
{
	if (p->done() || s->done()) //if the ship or projectile is already marked as dead, do nothing
		return;
	s->alterHP(p->getDamageValue()); //deal the damage to the ship
	p->setStatus(true);	//set the projectile as dead
	if (s->getHP() <= 0)
	{
		if (s->isEnemy()) //if the dead ship is an enemy
			enemyDeath(dynamic_cast<Enemy*>(s));
		else
			s->setStatus(true); //if the dead ship is the player
		return; //do not play SOUND_BLAST if a ship died
	}
	playSound(SOUND_BLAST);
}

void StudentWorld::goodie(Goody* g)
{
	increaseScore(100);
	g->setStatus(true); //mark the goodie as dead/used
	playSound(SOUND_GOODIE);
	switch (g->getID()) //getID() returns 1, 2, or 3 depending on the type of goodie
	{
	case 0:
		player->addAmmo(5);
		break;
	case 1:
		//adds 10 HP to the player and then subtracts the amount over the maxhp, if it goes over the maxhp
		player->alterHP(10);
		if (player->getHP() > player->getMaxHP())
			player->alterHP(-(player->getHP() - player->getMaxHP()));
		break;
	case 2:
		incLives();
		break;
	default:
		cerr << "Invalid goodieID" << endl;
	}
}

bool StudentWorld::collision(Actor* actor)
{
	bool collided = false; 
	if (actor->done()) //if the actor is already marked as dead, do nothing
		return collided;
	if (actor == player) //if the passed actor is the player
	{
		for (int i = 0; i < actors.size(); i++)
		{
			if (actors[i]->done()) //skip any objects already marked as dead
				continue;
			if (actors[i]->isEnemy())
				if (collide(actor, actors[i])) 
				{
					shipCollision(player, dynamic_cast<Enemy*>(actors[i])); //if actors[i] is an enemy and the objects collide, then ships have collided
					collided = true;
				}
			if (actors[i]->isProjectile())
				if (dynamic_cast<Projectiles*>(actors[i])->getTeam() == 1 && collide(actor, actors[i]))
				{
					projectileCollision(player, dynamic_cast<Projectiles*>(actors[i])); //if actors[i] is a projectile on the enemy team (1) and the objects collide, then the projectile has hit a ship
					collided = true;
				}
		}
	}
	else if (actor->isEnemy()) //if actor is an Enemy
	{
		if (collide(player, actor))
		{
			shipCollision(player, dynamic_cast<Enemy*>(actor)); //if the player and enemy collide, then ships have collided
			collided = true;
		}

		for (int i = 0; i < actors.size(); i++)
		{
			if (actors[i]->isProjectile())
				if (dynamic_cast<Projectiles*>(actors[i])->getTeam() == 0 && collide(actors[i], actor) && !actor->done())
				{
					projectileCollision(dynamic_cast<Ship*>(actor), dynamic_cast<Projectiles*>(actors[i])); //if the projectile and enemy collie, then the projectile has hit a ship
					collided = true;
				}
		}
	}
	else if (actor->isProjectile()) //if actor is a Projectiles
	{
		Projectiles* p = dynamic_cast<Projectiles*>(actor);
		if (p->getTeam() == 0) //team 0 (player)
		{
			for (int i = 0; i < actors.size(); i++)
				if (actors[i]->isEnemy() && !actors[i]->done()) 
					if (collide(p, actors[i])) //if the projectile hits an enemy ship
					{
						projectileCollision(dynamic_cast<Ship*>(actors[i]), p); //handle projectile collision
						collided = true;
					}
		}
		else if (p->getTeam() == 1) //team 1 (enemy)
			if (collide(actor, player)) //if the projectile hits the player
			{
				projectileCollision(player, p);
				collided = true;
			}
	}
	else if (actor->isGoody()) //if actor is a Goody
		if (collide(actor, player)) //if the goodie hits a player
		{
			goodie(dynamic_cast<Goody*>(actor));  //handle goodie collision
			collided = true;
		}
	return collided;
}

int StudentWorld::move()
{
	int x = actors.size(); //doSomething might add an object to the vector; avoid tick on objects created this tick
	for (int i = 0; i < x; i++)
		if (!actors[i]->done())
		{
			actors[i]->doSomething(); //do the things if they're not dead
			if (player->done()) //if player died this tick
			{
				decLives(); //decrease lives and return the death value
				return GWSTATUS_PLAYER_DIED;
			}
			if (shipsRemaining <= 0) //check if level is complete
			{
				playSound(SOUND_FINISHED_LEVEL);
				return GWSTATUS_FINISHED_LEVEL;
			}
		}


	player->doSomething(); //do the things
	if (player->done()) //if player died this tick
	{
		decLives(); //decrease lives and return the death value
		return GWSTATUS_PLAYER_DIED;
	}
	if (shipsRemaining <= 0) //check if level is complete
	{
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}


	for (int i = 0; i < actors.size(); i++)
	{
		//remove dead objects
		Actor* current = actors[i];
		if (current->done()) 
		{
			if (current->isEnemy()) //if the dead object is an enemy, decrement numEnemies
				numEnemies--;
			delete current; //free memory
			actors.erase(actors.begin() + i); //remove pointer from vector
			i--; //decrement i since the elements of the vector have been shifted back
		}
	}

	//add a star at a 1/15 chance
	if (rand() % 15 == 0) //   1/15 chance
	{
		actors.push_back(new Star(VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1), this, 0, random(.05, .51))); //add a new star at the somewhere along the right edge of the screen	
	}


	//add an enemy if the current number of enemies on the screen is less than the lower of: shipsRemaining or 4 + .5*level
	if (numEnemies < min(floor(4 + .5*getLevel()), 1.0*shipsRemaining))
	{
		//stuff for probability
		int s1 = 60;
		int s2 = 20 + getLevel() * 5;
		int s3 = 5 + getLevel() * 10;
		int s = s1 + s2 + s3;

		int x = randInt(1, s);
		if (x <= 60) {
			actors.push_back(new Smallgon(VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1), 5 * (1 + (getLevel() - 1)*.1), this)); //create Smallgon
		}
		else if (x <= 60 + s2) {
			actors.push_back(new Smoregon(VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1), 5 * (1 + (getLevel() - 1)*.1), this)); //create Smoregon
		}
		else {
			actors.push_back(new Snagglegon(VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1), 10 * (1 + (getLevel() - 1)*.1), this)); //create Snagglegon
		}
		numEnemies++; //increment number of enemies on screen
	}

	//form the output string with the relevant info
	ostringstream output;
	output.setf(ios::fixed);
	output.precision(0);
	output << "Lives: " << getLives() << "  Health: "
		<< static_cast<double>(player->getHP()) / (player->getMaxHP()) * 100
		<< "%  Score: " << getScore() << "  Level: " << getLevel() << "  Cabbages: "
		<< static_cast<double>(player->getCP()) / (player->getMaxCP()) * 100
		<< "%  Torpedoes: " << player->ammo() << endl;

	setGameStatText(output.str()); 

    return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::cleanUp()
{
	//delete the player if it hasn't already been deleted
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	//delete everything in the vector
	for (int i = actors.size() - 1; i >= 0; i--)		
		delete actors[i];
	//'reset' the vector
	actors.clear();
}

void StudentWorld::newActor(Actor* actor)
{
	actors.push_back(actor);
}

//returns a random double in the range [min, max) with precision to 2 decimal places
double random(double min, double max)
{
	int range = 100*(max - min);
	return (( static_cast<double> (rand() % range) / 100) + min);
}
