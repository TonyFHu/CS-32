#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, StudentWorld* world, Direction dir = 0, double size = 1.0, unsigned int depth = 0)
		: GraphObject(imageID, startX, startY, dir, size, depth), sw(world), status(false) {}
	//simple constructor?

	StudentWorld* getWorld() const { return sw; }
	virtual bool isEnemy() const { return false; }
	virtual bool isGoody() const { return false; }
	virtual bool isProjectile() const { return false; }

	bool deadOrOffscreen();
	bool done() const { return status; }
	void setStatus(bool status) { this->status = status; } //flag the object for removal/not removal

	void move(double deltaX, double deltaY); //calls moveTo, but allows use of a relative change rather than absolute coordinates

	virtual void doSomething() = 0;
private:
	

	StudentWorld* sw; //pointer back into the StudentWorld
	bool status; //true if the actor needs to be removed
};

class Explosion : public Actor
{
public:
	Explosion(double startX, double startY, StudentWorld* world, Direction dir = 0, double size = 1.0, unsigned int depth = 0)
		: Actor(IID_EXPLOSION, startX, startY, world, dir, size, depth), m_lifetime(3) {}

	virtual void doSomething();
private:
	int m_lifetime;
};

class Star : public Actor
{
public:
	Star(double startX, double startY, StudentWorld* world, Direction dir = 0, double size = 1.0, unsigned int depth = 3)
		: Actor(IID_STAR, startX, startY, world, dir, size, depth) {}
	virtual void doSomething();
};



class Ship : public Actor
{
public:
	Ship(int imageID, double startX, double startY, int hp, StudentWorld* world, Direction dir = 0, double size = 1.0, unsigned int depth = 1)
		: Actor(imageID, startX, startY, world, dir, size, depth), m_hp(hp), m_maxHP(hp) {}

	int getHP() const { return m_hp; } //accessors
	int getMaxHP() const { return m_maxHP; }

	void alterHP(int dhp) { m_hp += dhp; } //mutators
	virtual void doSomething() = 0;
private:
	int m_hp;
	int m_maxHP;
};


class NachenBlaster : public Ship
{
public:
	NachenBlaster(StudentWorld* world)
		: Ship(IID_NACHENBLASTER, 0, 128, 50, world, 0, 1, 0), m_torpedoes(0), m_cp(30), m_maxCP(30) {}

	int getCP() const { return m_cp; }
	int getMaxCP() const { return m_maxCP; }

	void alterCP(int dcp) { m_cp += dcp; }

	int ammo() const { return m_torpedoes; }
	void addAmmo(int num) { m_torpedoes += num; }

	virtual void doSomething();
private:
	bool fireCabbage();
	bool fireTorpedo();

	int m_torpedoes;
	int m_cp;
	int m_maxCP;
};

class Enemy : public Ship
{
public:
	Enemy(int imageID, double startX, double startY, int hp, StudentWorld* world, int plan, int length, double speed, int points, int damage, Direction dir = 0, double size = 1.0, unsigned int depth = 1)
		: Ship(imageID, startX, startY, hp, world, dir, size, depth), m_flightplan(plan), m_flightLength(length), m_flightSpeed(speed), m_pointValue(points), m_crashDamage(damage) {}

	int flightLength() const { return m_flightLength; } //accessors
	int flightplan() const { return m_flightplan; }
	int pointValue() const { return m_pointValue; }
	int crashValue() const { return m_crashDamage; }

	void setFlightSpeed(int speed) { m_flightSpeed = speed; } //mutators
	void setFlightplan(int plan) { m_flightplan = plan; }
	void setFlightLength(int length) { m_flightLength = length; }

	void moveAlongPath();
	bool fire();

	virtual void doSomething();
	virtual bool isEnemy() const { return true; }
private:
	virtual void adjustPlan();

	int m_flightplan; //1 for up-left, 2 for straight-left, 3 for down-left
	int m_flightLength;
	double m_flightSpeed;
	int m_pointValue;
	int m_crashDamage;
};

class Smallgon : public Enemy
{
public:
	Smallgon(double startX, double startY, int hp, StudentWorld* world)
		: Enemy(IID_SMALLGON, startX, startY, hp, world, 2, 0, 2, 250, -5, 0, 1.5, 1) {}
};


class Snagglegon : public Enemy
{
public:
	Snagglegon(double startX, double startY, int hp, StudentWorld* world)
		: Enemy(IID_SNAGGLEGON, startX, startY, hp, world, 3, 0, 1.75, 1000, -15, 0, 1.5, 1) {}
	virtual void adjustPlan();
};


class Smoregon : public Enemy
{
public:
	Smoregon(double startX, double startY, int hp, StudentWorld* world)
		: Enemy(IID_SMOREGON, startX, startY, hp, world, 2, 0, 2, 250, -5, 0, 1.5, 1) {}
};



class Projectiles : public Actor
{
public:
	Projectiles(int imageID, double startX, double startY, StudentWorld* world, int team, int speed, int rot, int damage, Direction dir = 0, double size = .5, unsigned int depth = 1)
		: Actor(imageID, startX, startY, world, dir, size, depth), affiliation(team), velocity(speed), damageValue(damage), rotation(rot) {}

	int getTeam() const { return affiliation; }
	int getDamageValue() const { return damageValue; }
	
	virtual bool isProjectile() const { return true; }

	virtual void doSomething();
private:
	int affiliation; //0 for player's projectiles and 1 for enemy's
	int velocity; //how fast to move the projectile
	int damageValue; //damage the projectile does
	int rotation; //rotation per tick of the projectile
};

class Cabbage : public Projectiles
{
public:
	Cabbage(double startX, double startY, StudentWorld* world)
		:Projectiles(IID_CABBAGE, startX, startY, world, 0, 8, 20, -2) {}
};

class Turnip : public Projectiles
{
public:
	Turnip(double startX, double startY, StudentWorld* world)
		:Projectiles(IID_TURNIP, startX, startY, world, 1, -6, 20, -2) {}

};

class Torpedo : public Projectiles
{
public:
	Torpedo(double startX, double startY, StudentWorld* world, int team, int speed, Direction dir = 0)
		:Projectiles(IID_TORPEDO, startX, startY, world, team, speed, 0, -8, dir) {}
};


class Goody : public Actor
{
public:
	Goody(int imageID, double startX, double startY, StudentWorld* world, int ID, Direction dir = 0, double size = .5, unsigned int depth = 1)
		: Actor(imageID, startX, startY, world, dir, size, depth), goodieID(ID) {}

	virtual bool isGoody() const { return true; }
	int getID() const { return goodieID; }

	virtual void doSomething();
private:
	int goodieID; //0: Torpedo, 1: Repair, 2: Life
};

class gTorpedo : public Goody
{
public:
	gTorpedo(double startX, double startY, StudentWorld* world)
		: Goody(IID_TORPEDO_GOODIE, startX, startY, world, 0) {}
};

class gRepair : public Goody
{
public:
	gRepair(double startX, double startY, StudentWorld* world)
		: Goody(IID_REPAIR_GOODIE, startX, startY, world, 1) {}
};

class gLife : public Goody
{
public:
	gLife(double startX, double startY, StudentWorld* world)
		: Goody(IID_LIFE_GOODIE, startX, startY, world, 2) {}
};
#endif // ACTOR_H_
