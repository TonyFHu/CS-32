#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <functional>
#include "MyHash.h"
#include "provided.h"

class WordListImpl
{
public:
	WordListImpl();
	bool loadWordList(std::string dictFilename);
	bool contains(std::string word)	const;
	std::vector<std::string> findCandidates(std::string cipherWord, std::string currTranslation) const;
private:
	std::string toLower(std::string word) const;
	std::string getPattern(const std::string& word) const;

	MyHash<std::string, std::string> wordMap;
	MyHash<std::string, std::list<std::string>> patternIndex;
};

WordListImpl::WordListImpl()
{
	
}

bool WordListImpl::loadWordList(std::string dictFilename) //O(N), N = number of words (NOT UNIQUE)
{
	wordMap.reset();
	patternIndex.reset();
	std::ifstream file(dictFilename);
	if (!file)
		return false;
	std::string word;
	while (file >> word)
	{
		if (wordMap.find(word) != nullptr)
			continue;
		word = toLower(word);
		wordMap.associate(word, word);
		std::string pattern = getPattern(word);
		std::list<std::string>* wordsWithPattern = patternIndex.find(pattern);
		if (wordsWithPattern != nullptr)
			wordsWithPattern->push_back(*wordMap.find(word));
		else
		{
			std::list<std::string> firstWord;
			firstWord.push_back(word);
			patternIndex.associate(pattern, firstWord);
		}
	}
	return true;
}

bool WordListImpl::contains(std::string word) const //O(1)
{
	word = toLower(word);
	return (wordMap.find(word) != nullptr);
}

std::vector<std::string> WordListImpl::findCandidates(std::string cipherWord, std::string currTranslation) const
{
 	std::list<std::string> words;		//list for constant time removal (already knowing the pointer to the node)

	if (cipherWord.size() == currTranslation.size())
	{
		cipherWord = toLower(cipherWord);
		currTranslation = toLower(currTranslation);
		std::string pattern = getPattern(cipherWord);

		MyHash<char, char> cipher;
		for (int i = 0; i < currTranslation.size(); i++)
			if (isalpha(currTranslation[i]))
				cipher.associate(pattern[i], currTranslation[i]);

		const std::list<std::string>* wordsPtr = patternIndex.find(pattern);
		if (wordsPtr != nullptr)
			words = *wordsPtr;
		for (std::list<std::string>::iterator i = words.begin(); i != words.end();)
		{
			bool match = true;
			for (int j = 0; j < (*i).size(); j++)
			{
				char* ch = cipher.find(pattern[j]);
				if (ch != nullptr && (*cipher.find(pattern[j]) != (*i)[j]))
				{
					i = words.erase(i);
					match = false;
					break;
				}
				else if ((currTranslation[j] == '\'' && cipherWord[j] != '\'') || (isalpha(currTranslation[j]) && (!isalpha(cipherWord[j]))))
				{
					words.clear(); //will end up returning an empty vector/list
					i = words.begin(); //reinitialize the iterator since the list was just cleared
					match = false;
					break;
				}
			}
			if (match)
				i++;
		}
	}
	std::vector<std::string> matches;
	for (std::list<std::string>::const_iterator i = words.begin(); i != words.end(); i++)
		matches.push_back(*i);
	return matches;
}

std::string WordListImpl::getPattern(const std::string& word) const		//O(1)
{
	std::string pattern = "";
	MyHash<char, char> uniqueChars;
	for (int i = 0; i < word.size(); i++)
	{
		if (uniqueChars.find(word[i]) == nullptr)
			uniqueChars.associate(word[i], 'a' + i);
		pattern += *uniqueChars.find(word[i]);
	}
	return pattern;
}

std::string WordListImpl::toLower(std::string word) const
{
	for (int i = 0; i < word.size(); i++)
		word[i] = tolower(word[i]);
	return word;
}

/*
int main()
{
	WordList test;
	if (!test.loadWordList("C:/Users/Tony/source/repos/Project5/Project5/Debug/wordlist.txt"))
	{
		std::cout << "Unable to load word list file " << std::endl;
		return false;
	}

	std::vector<std::string> candidates = test.findCandidates("pop", "???");

	std::string wait;
	getline(std::cin, wait);

	std::cerr << "MATCHES" << std::endl;
	for (std::vector<std::string>::const_iterator i = candidates.begin(); i != candidates.end(); i++)
		std::cerr << *i << std::endl;

	getline(std::cin, wait);
}
*/



//***** hash functions for string, int, and char *****

unsigned int hash(const std::string& s)
{
    std::hash<std::string> hash;
	return hash(s);
}

unsigned int hash(const int& i)
{
    std::hash<int> hash;
	return hash(i);
}

unsigned int hash(const char& c)
{
	std::hash<char> hash;
	return hash(c);
}


//******************** WordList functions ************************************

// These functions simply delegate to WordListImpl's functions.
// You probably don't want to change any of this code.

WordList::WordList()
{
    m_impl = new WordListImpl;
}

WordList::~WordList()
{
    delete m_impl;
}

bool WordList::loadWordList(std::string filename)
{
    return m_impl->loadWordList(filename);
}

bool WordList::contains(std::string word) const
{
    return m_impl->contains(word);
}

std::vector<std::string> WordList::findCandidates(std::string cipherWord, std::string currTranslation) const
{
   return m_impl->findCandidates(cipherWord, currTranslation);
}
