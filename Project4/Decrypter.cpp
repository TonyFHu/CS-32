#include "provided.h"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>


class DecrypterImpl
{
public:
	DecrypterImpl()
	{
		tokenizer = new Tokenizer(",;:.!()[]{}-\"#$%^& 0123456789"); 
		words = new WordList();
		translator = new Translator();
	}

	~DecrypterImpl()
	{
		delete words;
		delete tokenizer;
		delete translator;
	}

	bool load(std::string filename);
	std::vector<std::string> crack(const std::string& ciphertext);


private:
	std::string selectWord(const std::string& ciphertext);
	std::vector<std::string> fullyTranslatedWords(const std::vector<std::string>& words) const;
	bool validWords(const std::vector<std::string>& words) const;

	WordList* words;
	Tokenizer* tokenizer;
	Translator* translator;
};

bool DecrypterImpl::load(std::string filename)
{
	return words->loadWordList(filename);
}

std::vector<std::string> DecrypterImpl::crack(const std::string& ciphertext)
{						//step 1.
	std::vector<std::string> translations;

	std::string maxUnknowns = selectWord(ciphertext);

	std::vector<std::string> possibleMatches = words->findCandidates(maxUnknowns, translator->getTranslation(maxUnknowns));		//step 3, step 4.
	for (int i = 0; i < possibleMatches.size(); i++)
	{
		if (!translator->pushMapping(maxUnknowns, possibleMatches[i]))					//step 6a.
			continue;

		std::string proposedTranslation = translator->getTranslation(ciphertext);
		std::vector<std::string> proposedTokens = tokenizer->tokenize(proposedTranslation);
		std::vector<std::string> fullyTranslated = fullyTranslatedWords(proposedTokens);
		if (validWords(fullyTranslated))
		{
			if (proposedTokens.size() == fullyTranslated.size())
			{
				translations.push_back(proposedTranslation);
				translator->popMapping();
			}
			else
			{
				std::vector<std::string> validTranslations = crack(ciphertext);
				translations.insert(translations.end(), validTranslations.begin(), validTranslations.end());
			}
		}
		else
			translator->popMapping();
	}
	translator->popMapping();	
	std::sort(translations.begin(), translations.end());
	return translations;
}


std::vector<std::string> DecrypterImpl::fullyTranslatedWords(const std::vector<std::string>& words) const
{
	std::vector<std::string> translated;
	for (int i = 0; i < words.size(); i++)
	{
		bool fullyTranslated = true;
		for (int j = 0; j < words[i].size(); j++)
			if (words[i][j] == '?')
				fullyTranslated = false;
		if (fullyTranslated)
			translated.push_back(words[i]);
	}
	return translated;
}

//select the word with the most unknown characters
std::string DecrypterImpl::selectWord(const std::string& ciphertext)
{
	std::vector<std::string> tokens = tokenizer->tokenize(ciphertext);	//step 2.
	std::string maxUnknowns;
	int max = -1;
	for (std::vector<std::string>::iterator i = tokens.begin(); i != tokens.end(); i++)
	{
		int unknowns = 0;
		std::string word = translator->getTranslation(*i);
		for (int j = 0; j < word.size(); j++)
		{
			if (word[j] == '?')
				unknowns++;
		}
		if (unknowns > max)
		{
			max = unknowns;
			maxUnknowns = (*i);
		}
	}
	return maxUnknowns;
}

bool DecrypterImpl::validWords(const std::vector<std::string>& words) const
{
	if (words.size() == 0)
		return false;
	for (int i = 0; i < words.size(); i++)
	{
		if (!this->words->contains(words[i]))
			return false;
	}
	return true;
}

using namespace std;
//******************** Decrypter functions ************************************

// These functions simply delegate to DecrypterImpl's functions.
// You probably don't want to change any of this code.

Decrypter::Decrypter()
{
    m_impl = new DecrypterImpl;
}

Decrypter::~Decrypter()
{
    delete m_impl;
}

bool Decrypter::load(string filename)
{
    return m_impl->load(filename);
}

vector<string> Decrypter::crack(const string& ciphertext)
{
   return m_impl->crack(ciphertext);
}
