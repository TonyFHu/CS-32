#include "Map.h"

////////////////////////////
////NON-MEMBER FUNCTIONS////
////////////////////////////
bool combine(const Map& m1, const Map& m2, Map& result)
{
	bool success = true;

	KeyType key;
	ValueType value;
	ValueType other;

	Map results = m1;
	for (int i = 0; i < m2.size(); i++)
	{
		m2.get(i, key, value); //get keys for each node in m2
		if (results.get(key, other))	//check keys against results (m1) //get(...) returns false if key isn't in the list
		{
			if (other != value)			//if the keys are the same but values are not, remove that node from results
			{
				results.erase(key);
				success = false;
			}
		}
		else
			results.insert(key, value);	//add a node if the key doesn't match any in result
	}
	result = results;
	return success;
}

void subtract(const Map& m1, const Map& m2, Map& result)
{
	KeyType key;
	ValueType value;
	Map results = m1;
	for (int i = 0; i < m2.size(); i++) 
	{
		m2.get(i, key, value); //with the loop, get every key from m2
		results.erase(key);	//attempt to delete from results every key taken from m2
	}
	result = results;
}

///////////////////////////
////NODE IMPLEMENTATION////
///////////////////////////
Map::Node::Node(Node* previous, ValueType value, KeyType key, Node* next)
{
	m_previous = previous;
	m_next = next;
	m_value = value;
	m_key = key;
}

//constructor that copies the node pointed to by target
//needs to be given the pointer to the previous node since target's previous node isn't the copy
Map::Node::Node(Node* target, Node* previous)
{
	m_value = target->m_value; //copies basic values
	m_key = target->m_key;
	m_previous = previous;
	if (target->m_next != nullptr) //since each node points to a next node, each node builds the next node after it, until it reaches the end
		m_next = new Node(target->m_next, this);
	else
		m_next = nullptr;
}

KeyType Map::Node::key() const
{
	return m_key;
}

ValueType Map::Node::value() const
{
	return m_value;
}

Map::Node* Map::Node::nextNode() const
{
	return m_next;
}

Map::Node* Map::Node::previousNode() const
{
	return m_previous;
}

void Map::Node::setNextNode(Node* next)
{
	m_next = next;
}

void Map::Node::setPreviousNode(Node* previous)
{
	m_previous = previous;
}

void Map::Node::setValue(ValueType value)
{
	m_value = value;
}


//////////////////////////
////MAP IMPLEMENTATION////
//////////////////////////

//initialize map with nullptr for head and size = 0
Map::Map()
{
	m_head = nullptr;
	m_size = 0;
}

//copy constructor
Map::Map(const Map& target)
{
	m_size = target.m_size;
	if (target.m_head != nullptr)
		m_head = new Map::Node(target.m_head); //deep copy of the head node, which ends up making copies of all the nodes after the head
	else
		m_head = nullptr;
}

//assignment operator
Map& Map::operator=(const Map& target)
{ 
	Map temp = target;
	swap(temp); //copy and swap
	return *this;
}

//destructor
Map::~Map()
{
	Map::Node* nextPtr;
	for (Map::Node* ptr = m_head; ptr != nullptr; ) //iterate through the link list, starting from the head
	{
		nextPtr = ptr->nextNode(); //store the pointer to the next node
		delete ptr; //delete the current node
		ptr = nextPtr; //set ptr to the next node
	}
}



bool Map::empty() const
{
	return m_size == 0;
}

int Map::size() const
{
	return m_size;
}


// If key is not equal to any key currently in the map, and if the
// key/value pair can be added to the map, then do so and return true.
// Otherwise, make no change to the map and return false (indicating
// that either the key is already in the map, or the map has a fixed
// capacity and is full).
bool Map::insert(const KeyType& key, const ValueType& value)
{
	Map::Node* previousNode = lastNode();
	Map::Node* ptr = find(key);

	if (previousNode == nullptr) //if previousNode == null, it means the list is empty, so set m_head to a new node
	{
		m_head = new Map::Node(nullptr, value, key);
		m_size++;
		return true;
	}
	else if (ptr == nullptr) //if the list is not empty, and there is no node with the key, add a new node
	{
		//set the nextnode of the last node to the pointer to the node created
		previousNode->setNextNode(new Map::Node(previousNode, value, key)); //creates a new node with the value, key, and pointer to the previous node. the next node is default set to nullptr
		m_size++;
		return true;
	}
	return false;
}


// If key is equal to a key currently in the map, then make that key no
// longer map to the value it currently maps to, but instead map to
// the value of the second parameter; return true in this case.
// Otherwise, make no change to the map and return false.
bool Map::update(const KeyType& key, const ValueType& value)
{
	Map::Node* ptr = find(key); //get the pointer to the node with they key
	if (ptr != nullptr) //avoid dereferencing nullptr
	{
		ptr->setValue(value);
		return true;
	}
	return false;
}


// If key is equal to a key currently in the map, then make that key no
// longer map to the value it currently maps to, but instead map to
// the value of the second parameter; return true in this case.
// If key is not equal to any key currently in the map and if the
// key/value pair can be added to the map, then do so and return true.
// Otherwise, make no change to the map and return false (indicating
// that the key is not already in the map and the map has a fixed
// capacity and is full, (which is impossible)).
bool Map::insertOrUpdate(const KeyType& key, const ValueType& value)
{
	if (insert(key, value)) //attempt to insert
		return true;
	return update(key, value); //update if insert fails
}


// If key is equal to a key currently in the map, remove the key/value
// pair with that key from the map and return true.  Otherwise, make
// no change to the map and return false.
bool Map::erase(const KeyType& key)
{
	Map::Node* ptr = find(key); //get the pointer to the node with the key
	if (ptr != nullptr) //avoid dereferencing nullptr
	{
		if (ptr->previousNode() == nullptr) //if there is no previous node (first node), set the head to the next node
		{
			m_head = ptr->nextNode();
			if (ptr->nextNode() != nullptr) //avoid dereferencing a null pointer (if this is the only node in the list)
				(ptr->nextNode())->setPreviousNode(nullptr); //set the next node's previous node to null (since the node to be deleted is the first one)
		}
		else if (ptr->nextNode() == nullptr) //if there is no next node (last node), set the previous node's next node to null
			(ptr->previousNode())->setNextNode(nullptr);
		else //for any node that isn't first or last
		{
			(ptr->previousNode())->setNextNode(ptr->nextNode()); //sets the previous node's next node to this node's next node, essentially skipping over this node
			(ptr->nextNode())->setPreviousNode(ptr->previousNode()); //sets the next nodes' previous node to this node's previous node, essentially skipping over this node
		}
		delete ptr; //release memory
		m_size--;
		return true;
	}
	return false;
}


// Return true if key is equal to a key currently in the map, otherwise
// false.
bool Map::contains(const KeyType& key) const
{
	//find(...) returns nullptr if it can't find a node with the key
	return (find(key) != nullptr);
}


// If key is equal to a key currently in the map, set value to the
// value in the map that that key maps to, and return true.  Otherwise,
// make no change to the value parameter of this function and return
// false.
bool Map::get(const KeyType& key, ValueType& value) const
{
	Map::Node* ptr = find(key); //gets the pointer to the node with the key value
	if (ptr != nullptr) //avoid dereferencing nullptr
	{
		value = ptr->value();
		return true;
	}
	return false;
}


// If 0 <= i < size(), copy into the key and value parameters the
// key and value of one of the key/value pairs in the map and return
// true.  Otherwise, leave the key and value parameters unchanged and
// return false.  (See below for details about this function.)
bool Map::get(int i, KeyType& key, ValueType& value) const
{
	if (i < 0 || i >= size()) //avoid running through the entire list if i is out of the correct range
		return false;

	int k = 0; //used to keep count of nodes gone through
	for (Map::Node* ptr = m_head; ptr != nullptr; ptr = ptr->nextNode())
	{
		//once the number of nodes passed through equals i, set the key/value to the key/value of the node
		if (k == i)
		{
			key = ptr->key();
			value = ptr->value();
			return true;
		}
		k++;
	}
	return false;
}


// Exchange the contents of this map with the other one.
void Map::swap(Map& other)
{
	//swaps the ints
	int tempInt = other.m_size;
	other.m_size = this->m_size;
	this->m_size = tempInt;

	//swaps the pointers to the head
	Map::Node* temp = other.m_head;
	other.m_head = this->m_head;
	this->m_head = temp;
}


//Find the node with key and return the pointer to it
//Return nullptr if no node with key is found
Map::Node* Map::find(const KeyType& key) const
{
	for (Map::Node* ptr = m_head; ptr != nullptr; ptr = ptr->nextNode())
	{
		//if the node's value matches key, return it
		if (ptr->key() == key)
			return ptr;
	}
	return nullptr;
}

//Find the last node in the list and return its pointer
//If there is no node, return nullptr
Map::Node* Map::lastNode() const
{
	for (Map::Node* ptr = m_head; ptr != nullptr; ptr = ptr->nextNode())
	{
		//if the next node's next node is nullptr, then the current node is last
		if (ptr->nextNode() == nullptr)
			return ptr;
	}
	return nullptr;
}
