#ifndef MYHASH_H
#define MYHASH_H

#include <string>
#include <iostream>



template <class KeyType, class ValueType> //assume standard KeyType
class MyHash
{
public:
	MyHash(double maxLoadFactor = 0.5);
	~MyHash();
	void reset();
	void associate(const KeyType& key, const ValueType& value);
	const ValueType* find(const KeyType& key) const;
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyHash*>(this)->find(key));
	}
	int getNumItems()	const;
	double getLoadFactor()	const;

	MyHash(const MyHash&) = delete;
	MyHash& operator=(const MyHash&) = delete;
private:
	class Pair
	{
	public:
		Pair(const KeyType& key, const ValueType& value) : m_key(key), m_value(value), m_nextPair(nullptr) {}
		~Pair()
		{
			if (m_nextPair != nullptr)
				delete m_nextPair;
		}

		Pair* next() const { return m_nextPair; }
		ValueType* valuePtr() { return &m_value; }
		KeyType key() const { return m_key; }

		void setValue(ValueType newValue) { m_value = newValue; }
		void setNext(Pair* next) { m_nextPair = next; }
	private:
		const KeyType m_key;
		ValueType m_value;
		Pair* m_nextPair;
	};

	void resize();
	void clear();
	unsigned int hashR(const KeyType& k) const;

	Pair** m_buckets;
	int m_numBuckets;
	double m_maxLoadFactor;
	int m_numItems;
};

template <class KeyType, class ValueType>
MyHash<KeyType, ValueType>::MyHash(double maxLoadFactor) : m_buckets(nullptr)
{
	m_buckets = new Pair*[100];
	m_numBuckets = 100;
	for (int i = 0; i < m_numBuckets; i++)
		m_buckets[i] = nullptr;

	if (maxLoadFactor <= 0)
		m_maxLoadFactor = .5;
	else if (maxLoadFactor > 2)
		m_maxLoadFactor = 2;
	else
		m_maxLoadFactor = maxLoadFactor;
}

template <class KeyType, class ValueType>
void MyHash<KeyType, ValueType>::clear()
{
	for (int i = 0; i < m_numBuckets; i++)
	{
		delete m_buckets[i];
		m_buckets[i] = nullptr;
	}
	delete[] m_buckets;
	m_buckets = nullptr;
}

template <class KeyType, class ValueType>
MyHash<KeyType, ValueType>::~MyHash()
{
	clear();
}

template <class KeyType, class ValueType>
void MyHash<KeyType, ValueType>::reset()
{
	clear();
	m_buckets = new Pair*[100];
	m_numBuckets = 100;
	m_numItems = 0;
	for (int i = 0; i < m_numBuckets; i++)
		m_buckets[i] = nullptr;
}

template <class KeyType, class ValueType>
void MyHash<KeyType, ValueType>::associate(const KeyType& key, const ValueType &value)
{
	Pair* ptr = new Pair(key, value);
	int index = hashR(key);
	ValueType* valuePtr = find(key);
	if (valuePtr == nullptr)
	{
		ptr->setNext(m_buckets[index]);
		m_buckets[index] = ptr;
		m_numItems++;
		if (getLoadFactor() > m_maxLoadFactor)
			resize();
		return;
	}
	else
		delete ptr;
	*valuePtr = value;
}

template <class KeyType, class ValueType>
void MyHash<KeyType, ValueType>::resize()
{
	int newSize = m_numBuckets * 2;
	Pair** resizedHashMap = new Pair*[newSize];
	for (int i = 0; i < newSize; i++)
		resizedHashMap[i] = nullptr;

	int oldSize = m_numBuckets; //the hash function uses m_numBuckets to calculate the index, so m_numBuckets needs to change first
	m_numBuckets = newSize;
	for (int i = 0; i < oldSize; i++)
	{
		for (Pair* ptr = m_buckets[i]; ptr != nullptr; ptr = ptr->next())
		{
			Pair* newPair = new Pair(ptr->key(), *ptr->valuePtr());
			newPair->setNext(resizedHashMap[hashR(ptr->key())]);
			resizedHashMap[hashR(ptr->key())] = newPair;
		}
	}
	m_numBuckets = oldSize; //this is a hack, but it works...
	clear();
	m_numBuckets = newSize;
	m_buckets = resizedHashMap;
}

template <class KeyType, class ValueType>
const ValueType* MyHash<KeyType, ValueType>::find(const KeyType& key) const
{
	for (Pair* ptr = m_buckets[hashR(key)]; ptr != nullptr; ptr = ptr->next())
		if (ptr->key() == key)
			return ptr->valuePtr();
	return nullptr;
}

template <class KeyType, class ValueType>
int MyHash<KeyType, ValueType>::getNumItems() const
{
	return m_numItems;
}

template <class KeyType, class ValueType>
double MyHash<KeyType, ValueType>::getLoadFactor()	const
{
	return (double)(m_numItems) / m_numBuckets;
}

template <class KeyType, class ValueType>
unsigned int MyHash<KeyType, ValueType>::hashR(const KeyType& key) const
{
	unsigned int hash(const KeyType& key);

	unsigned int x = hash(key);
	int y = m_numBuckets;
	return x % y;
}

#endif