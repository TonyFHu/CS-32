#ifndef TOKENIZER_HC
#define TOKENIZER_HC

#include <string>
#include <vector>
#include <iostream>
#include "provided.h"
#include "MyHash.h"

class TokenizerImpl
{
public:
	TokenizerImpl(std::string separators);
	std::vector<std::string> tokenize(const std::string& s) const;
private:
	MyHash<char, int> m_separators;
};

TokenizerImpl::TokenizerImpl(std::string separators)
{
	for (int i = 0; i < separators.size(); i++)
		m_separators.associate(separators[i], 0);
}

std::vector<std::string> TokenizerImpl::tokenize(const std::string& s) const
{
	std::vector<std::string> tokens;
	std::string token;
	for (int i = 0; i < s.size(); i++)
	{
		if (m_separators.find(s[i]) == nullptr)
			token += s[i];
		else if (token.size() > 0)
		{
			tokens.push_back(token);
			token = "";
		}
	}
	if (token.size() > 0)
		tokens.push_back(token);
	return tokens;
}

using namespace std;

//******************** Tokenizer functions ************************************

// These functions simply delegate to TokenizerImpl's functions.
// You probably don't want to change any of this code.

Tokenizer::Tokenizer(string separators)
{
	m_impl = new TokenizerImpl(separators);
}

Tokenizer::~Tokenizer()
{
	delete m_impl;
}

vector<string> Tokenizer::tokenize(const std::string& s) const
{
	return m_impl->tokenize(s);
}

#endif