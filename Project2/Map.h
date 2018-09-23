#ifndef MAP_H
#define MAP_H

#include <string>

using KeyType = std::string;
using ValueType = double;

class Node;

class Map
{
public:
	Map();

	Map(const Map& target);

	Map& operator=(const Map& target);

	~Map();

	bool empty() const;
	int size() const;
	bool insert(const KeyType& key, const ValueType& value);
	bool update(const KeyType& key, const ValueType& value);
	bool insertOrUpdate(const KeyType& key, const ValueType& value);
	bool erase(const KeyType& key);
	bool contains(const KeyType& key) const;
	bool get(const KeyType& key, ValueType& value) const;
	bool get(int i, KeyType& key, ValueType& value) const;
	void swap(Map& other);
private:
	class Node
	{
	public:
		Node(Node* previous, ValueType value, KeyType key, Node* next = nullptr);
		Node(Node* target, Node* previous = nullptr);

		ValueType value() const; //accessors
		KeyType key() const;
		Node* nextNode() const;
		Node* previousNode() const;

		void setValue(ValueType value); //mutators
		void setNextNode(Node* next);
		void setPreviousNode(Node* previous);

	private:
		Node* m_previous;
		Node* m_next;
		ValueType m_value;
		KeyType m_key;
	};

	Node* find(const KeyType& key) const;
	Node* lastNode() const;

	Node* m_head;
	int m_size;
	
};

bool combine(const Map& m1, const Map& m2, Map& result);
void subtract(const Map& m1, const Map& m2, Map& result);

#endif