#include <string>
#include "MyHash.h"
#include "provided.h"

class TranslatorImpl
{
public:
	TranslatorImpl();
	~TranslatorImpl();
	bool pushMapping(std::string ciphertext, std::string plaintext);
	bool popMapping();
	std::string getTranslation(const std::string& ciphertext) const;

	void mapStatus();
private:
	class Node
	{
	public:
		Node(MyHash<char, char>& mapping) : nextNode(nullptr) 
		{
			for (int i = 0; i < 26; i++)
			{
				if (mapping.find('a' + i) != nullptr)
					map.associate('a' + i, *mapping.find('a' + i));
			}
		}

		~Node()
		{
			if (nextNode != nullptr)
				delete nextNode;
		}

		void setNext(Node* next) { nextNode = next; }

		MyHash<char, char>& value() { return map; }
		Node* next() { return nextNode; }
	private:
		MyHash<char, char> map;
		Node* nextNode;
	};

	Node* mappings;
	MyHash<char, char>* currentMap;
	MyHash<char, char>* reverseMap;
};

void TranslatorImpl::mapStatus()
{
	for (int i = 0; i < 26; i++)
	{
		std::cout << (char)('a' + i) << ": ";
		if (currentMap->find('a' + i) != nullptr)
			std::cout << *currentMap->find('a' + i);
		std::cout << std::endl;
	}
}

TranslatorImpl::TranslatorImpl()
{
	currentMap = new MyHash<char, char>;
	reverseMap = new MyHash<char, char>;
	mappings = nullptr;
}

TranslatorImpl::~TranslatorImpl()
{
	delete currentMap;
	delete reverseMap;
	delete mappings;
}

bool TranslatorImpl::pushMapping(std::string ciphertext, std::string plaintext)
{
	if (ciphertext.size() != plaintext.size())
		return false;
	for (int i = 0; i < ciphertext.size(); i++)
	{
		if (!isalpha(ciphertext[i]) || !isalpha(plaintext[i]))
			return false;
		if (currentMap->find(tolower(ciphertext[i])) != nullptr)
			if (*currentMap->find(tolower(ciphertext[i])) != tolower(plaintext[i]))
				return false;
		if (reverseMap->find(tolower(plaintext[i])) != nullptr) 
			if(*reverseMap->find(tolower(plaintext[i])) != tolower(ciphertext[i]))
				return false;
	}	
	for (int i = 0; i < ciphertext.size(); i++)
	{
		currentMap->associate(tolower(ciphertext[i]), tolower(plaintext[i]));
		reverseMap->associate(tolower(plaintext[i]), tolower(ciphertext[i]));
	}

	Node* node = new Node(*currentMap);
	node->setNext(mappings);
	mappings = node;
	return true;
}

bool TranslatorImpl::popMapping()
{
	if (mappings == nullptr)
		return false;
	Node* toRemove = mappings;
	mappings = mappings->next();
	toRemove->setNext(nullptr);
	delete toRemove;

	MyHash<char, char>* newMap = new MyHash<char, char>;
	MyHash<char, char>* newRMap = new MyHash<char, char>;
	if(mappings != nullptr)
		for (int i = 0; i < 26; i++)
			if (mappings->value().find('a' + i) != nullptr)
			{
				newMap->associate('a' + i, *(mappings->value().find('a' + i)));
				newRMap->associate(*(mappings->value().find('a' + i)), 'a' + i);
			}
	delete currentMap;
	delete reverseMap;
	currentMap = newMap;
	reverseMap = newRMap;
	return true;
}

std::string TranslatorImpl::getTranslation(const std::string& ciphertext) const
{
	std::string translation;
	for (int i = 0; i < ciphertext.size(); i++)
	{
		if (!isalpha(ciphertext[i]))
			translation += ciphertext[i];
		else if (currentMap->find(tolower(ciphertext[i])) == nullptr)
			translation += '?';
		else if (isupper(ciphertext[i]))
			translation += toupper(*currentMap->find(tolower(ciphertext[i])));
		else if (islower(ciphertext[i]))
			translation += tolower(*currentMap->find(tolower(ciphertext[i])));
	}
	return translation;
}

using namespace std;
//******************** Translator functions ************************************

// These functions simply delegate to TranslatorImpl's functions.
// You probably don't want to change any of this code.

Translator::Translator()
{
    m_impl = new TranslatorImpl;
}

Translator::~Translator()
{
    delete m_impl;
}

bool Translator::pushMapping(string ciphertext, string plaintext)
{
    return m_impl->pushMapping(ciphertext, plaintext);
}

bool Translator::popMapping()
{
    return m_impl->popMapping();
}

string Translator::getTranslation(const string& ciphertext) const
{
    return m_impl->getTranslation(ciphertext);
}
