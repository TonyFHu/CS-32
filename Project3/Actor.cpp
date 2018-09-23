#include "Actor.h"
#include "StudentWorld.h"
#include <ostream>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

void Actor::move(double deltaX, double deltaY)
{
	moveTo(getX() + deltaX, getY() + deltaY);
}

//move the star 1 unit left and flag it for removal if it moves off the left side of the screen
void Star::doSomething()
{
	move(-1, 0);
	if (getX() < 0)
		setStatus(true);
}

//increase the size of the explosion while it's lifetime is > 0, otherwise, flag for removal
void Explosion::doSomething()
{
	if (m_lifetime <= 0)
		setStatus(true);
	else
		setSize(1.5*getSize());
	m_lifetime--; //lower remaining lifetime
}

//move and handle the collision for all projectiles
void Projectiles::doSomething()
{
	if (deadOrOffscreen()) //if dead or offscreen, do nothing
		return;
	if (getWorld()->collision(this)) //handle collision, and if it collides, skip the rest
		return;
	move(velocity, 0); //move the projectile based on its velocity
	setDirection(getDirection() + rotation); //rotate
	getWorld()->collision(this); //handle collision again
}

//fires a cabbage if the player has enough CP
bool NachenBlaster::fireCabbage()
{
	if (getCP() >= 5)
	{
		alterCP(-5);
		getWorld()->newActor(new Cabbage(getX() + 12, getY(), getWorld()));
		getWorld()->playSound(SOUND_PLAYER_SHOOT);
		return true;
	}
	return false;
}

//fires a torpedo if the player has any
bool NachenBlaster::fireTorpedo()
{
	if (ammo() > 0)
	{
		addAmmo(-1);
		getWorld()->newActor(new Torpedo(getX() + 12, getY(), getWorld(), 0, 8));
		getWorld()->playSound(SOUND_TORPEDO);
		return true;
	}
	return false;
}

void NachenBlaster::doSomething()
{
	if (!done())
	{
		int ch;
		if (getWorld()->getKey(ch))
		{
			// user hit a key during this tick!
			switch (ch)
			{
			case KEY_PRESS_LEFT:
				if (getX() - 6 >= 0)
				{
					move(-6, 0); //move left
					getWorld()->collision(this); //check collision
				}
				break;
			case KEY_PRESS_RIGHT:
				if (getX() + 6 < VIEW_WIDTH)
				{
					move(6, 0); //move right
					getWorld()->collision(this); //check collision
				}
				break;
			case KEY_PRESS_UP:
				if (getY() + 6 < VIEW_HEIGHT)
				{
					move(0, 6); //move up
					getWorld()->collision(this); //check collision
				}
				break;
			case KEY_PRESS_DOWN:
				if (getY() - 6 >= 0)
				{
					move(0, -6); //move down
					getWorld()->collision(this); //check collision
				}
				break;
			case KEY_PRESS_SPACE:
				fireCabbage(); //attempt to fire a cabbage
				break;
			case KEY_PRESS_TAB:
				fireTorpedo(); //attempt to fire a torpedo
				break;
			default:
				break;
			}
		}
		if (getCP() < getMaxCP()) //increase player's CP by one every tick
			alterCP(1);

	}
}

//handles an enemy's attempt to shoot
//returns true if the enemy fired, false otherwise
bool Enemy::fire()
{
	if (dynamic_cast<Snagglegon*>(this) != nullptr) //if the enemy is a Snagglegon
	{
		if (randInt(1, (15 / getWorld()->getLevel()) + 10) == 1) //some chance
		{
			//fire a torpedo
			getWorld()->newActor(new Torpedo(getX() - 14, getY(), getWorld(), 1, -8, 180));
			getWorld()->playSound(SOUND_TORPEDO);
			return true;
		}
	}
	//if the enemy is a Smallgon or Smoregon
	else if (randInt(1, (20 / getWorld()->getLevel()) + 5) == 1) //some chance
	{
		//fire a turnip
		getWorld()->newActor(new Turnip(getX() - 14, getY(), getWorld()));
		getWorld()->playSound(SOUND_ALIEN_SHOOT);
		return true;
	}
	return false;
}

//changes flightplan if flightLength <= 0, or the Enemy reaches the top/bottom of the screen
void Enemy::adjustPlan()
{
	if (flightLength() <= 0 || getY() >= VIEW_HEIGHT - 1 || getY() < 0)
	{
		if (getY() >= VIEW_HEIGHT - 1) 
			setFlightplan(3); //down and left
		else if (getY() < 0)
			setFlightplan(1); //up and left
		else
			setFlightplan(randInt(1, 3)); //random path
		setFlightLength(randInt(1, 32)); //random length
	}
}

//overrides Enemy::adjustPlan()
//only changes flightplan if the Snagglegon reaches the top/bottom of the screen
void Snagglegon::adjustPlan()
{
	if (getY() >= VIEW_HEIGHT - 1)
		setFlightplan(3); //down and left
	else if (getY() < 0)
		setFlightplan(1); //up and left
	//no straight left
}

void Enemy::doSomething()
{
	if (deadOrOffscreen())	//if dead or offscreen, do nothing
		return;
	getWorld()->collision(this); //check/handle collision

	if (done()) //if the previous collision caused the Enemy to die, do nothing else
		return;

	adjustPlan();	//attempt to change the flightplan

	//if the Enemy is within 4 y-units of the player and is to the right of the player
	if (getWorld()->isClose(this, 4))
	{
		if (!fire()) //attempt to fire and if it doesn't, move the ship
			moveAlongPath();
		if (dynamic_cast<Smoregon*>(this) != nullptr && randInt(1, (20 / getWorld()->getLevel()) + 5) == 1) //if the ship is a Smoregon, chance to change flight to 'ramming mode'
		{
			setFlightplan(2);
			setFlightLength(VIEW_WIDTH);
			setFlightSpeed(5);
		}
	}
	else
		moveAlongPath(); //if the player is not within the 4 y-units, just move (also checks collision)
	
}

//handles the movement for all Enemies
void Enemy::moveAlongPath()
{
	switch (m_flightplan)
	{
	case 1:
		move(-m_flightSpeed, m_flightSpeed); //up and left
		break;
	case 2:
		move(-m_flightSpeed, 0); //left
		break;
	case 3:
		move(-m_flightSpeed, -m_flightSpeed); //down and left
		break;
	}
	setFlightLength(flightLength() - 1); //lower flightlength
	getWorld()->collision(this); //check collision after movement
}

void Goody::doSomething()
{
	if (deadOrOffscreen() || getY() < 0) //if dead or offscreen, do nothing
	{
		setStatus(true);
		return;
	}
	if (getWorld()->collision(this)) //if it collides, do nothing else
		return;
	move(-.75, -.75); //move
	getWorld()->collision(this); //handle collision again
}

bool Actor::deadOrOffscreen()
{
	if (done()) //if dead, return true
		return true;
	if (getX() < 0 || getX() >= VIEW_WIDTH) //if offscreen, set dead and return true
	{
		setStatus(true);
		return true;
	}
	return false; //otherwise, return false
}

