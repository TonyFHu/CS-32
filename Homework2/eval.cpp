#include "Map.h"
#include <iostream>
#include <string>
#include <stack>
#include <cctype>
#include <cassert>
using namespace std;

const int ERROR_INVALID_SYNTAX = 1;
const int ERROR_UNMAPPED_VALUE = 2;
const int ERROR_DIVIDE_BY_ZERO = 3;

const int NUMBER_OF_OPERATORS = 4;
const char OPERATORS[NUMBER_OF_OPERATORS] = { '+', '-', '*', '/'};
const int OPERATOR_PRECEDENCE[NUMBER_OF_OPERATORS] = { 1, 1, 2, 2 };

int checkOperator(char test);

int evaluate(string infix, const Map& values, string& postfix, int& result)
{
	stack<char> op;
	string post;
	char previous = 0;
	int opPrecedence;

	for (size_t i = 0; i < infix.size(); i++)
	{
		char current = infix[i];
		if (current == ' ')	//skip whitespace
			continue;

		//syntax checking
		if (checkOperator(infix[infix.size() - 1])) //an operator may not be last
			return ERROR_INVALID_SYNTAX;
		if (previous != '\0') 
		{
			bool isOperator = checkOperator(current);
			bool prevIsOperator = checkOperator(previous);
			bool isOperand = islower(current);
			bool prevIsOperand = islower(previous);

			if (isOperator && prevIsOperator) //two operators in a row is not allowed
				return ERROR_INVALID_SYNTAX;
			if (isOperand && prevIsOperand) //two operands in a row is not allowed
				return ERROR_INVALID_SYNTAX;
			if (isOperator && previous == '(') //operator may not follow an open parenthesis
				return ERROR_INVALID_SYNTAX;
			if (current == ')' && prevIsOperator) //operator may not precede a close parenthesis
				return ERROR_INVALID_SYNTAX;
			if (current == '(' && prevIsOperand) //no implicit multiplication; operand may not precede an open parenthesis
				return ERROR_INVALID_SYNTAX;
			if (isOperand && previous == ')') //no implicit multiplication; operand may not follow a close parenthesis
				return ERROR_INVALID_SYNTAX;
		}
		else if (current == ')' || checkOperator(current)) //a close parenthesis or operator may not be first
			return ERROR_INVALID_SYNTAX;


		if (islower(current))
		{
			post += current;
			previous = current;
		}
		else if (current == '(')
			op.push(current);
		else if (current == ')')
		{
			while (op.top() != '(')
			{
				post += op.top();
				op.pop();
				if (op.empty())
					return ERROR_INVALID_SYNTAX;
			}
			op.pop();
		}
		else if ( (opPrecedence = checkOperator(current)) ) //checkOperator returns a non-zero value if the argument is an operator
		{
			while (!op.empty() && op.top() != '(' && 
				OPERATOR_PRECEDENCE[opPrecedence-1] <= OPERATOR_PRECEDENCE[checkOperator(op.top())-1])
			{
				post += op.top();
				op.pop();
			}
			op.push(current);
		}
		else
			return ERROR_INVALID_SYNTAX;

		previous = current;
	}
	while (!op.empty())
	{
		post += op.top();
		op.pop();
	}

	if (post.size() == 0)
		return ERROR_INVALID_SYNTAX;
	postfix = post;

	//evaluation
	stack<int> operands;
	int operand1;
	int operand2;
	int value;

	for (size_t i = 0; i < postfix.size(); i++)
	{

	}
	for (size_t i = 0; i < postfix.size(); i++)
	{
		if (postfix[i] == '(')
			return ERROR_INVALID_SYNTAX;
		if (checkOperator(postfix[i]))
		{
			operand2 = operands.top();
			operands.pop();
			operand1 = operands.top();
			operands.pop();
			switch (postfix[i])
			{
			case '+':	operands.push(operand1 + operand2);	break;
			case '-':	operands.push(operand1 - operand2); break;
			case '*':	operands.push(operand1 * operand2); break;
			case '/':
				if (operand2 == 0)
					return ERROR_DIVIDE_BY_ZERO;
				operands.push(operand1 / operand2);
				break;
			}
		}
		else if(values.get(postfix[i], value))
			operands.push(value);
		else
			return ERROR_UNMAPPED_VALUE;
	}
	result = operands.top();
	return 0;
}

//returns the index+1 corresponding to the operator in OPERATORS[], 0 if test is not an operator
int checkOperator(char test)
{
	for (int i = 0; i < NUMBER_OF_OPERATORS; i++)
	{
		if (test == OPERATORS[i])
		{
			return i+1;
		}
	}
	return 0;
}

int main()
{
	Map map;
	for (int i = 0; i < 26; i++)
	{
		map.insert('a' + i, i + 1);
	}
	char vars[] = { 'a', 'e', 'i', 'o', 'u', 'y', '#' };
	int  vals[] = { 3,  -9,   6,   2,   4,   1 };
	Map m;
	for (int k = 0; vars[k] != '#'; k++)
		m.insert(vars[k], vals[k]);
	string pf;
	int answer;

	assert(evaluate("a+ e", m, pf, answer) == 0 &&
		pf == "ae+"  &&  answer == -6);
	answer = 999;
	assert(evaluate("", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("a+", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("a i", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("ai", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("()", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("y(o+u)", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("a+E", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("(a+(i-o)", m, pf, answer) == 1 && answer == 999);
	// unary operators not allowed:
	assert(evaluate("-a", m, pf, answer) == 1 && answer == 999);
	assert(evaluate("a*b", m, pf, answer) == 2 &&
		pf == "ab*"  &&  answer == 999);
	assert(evaluate("y +o *(   a-u)  ", m, pf, answer) == 0 &&
		pf == "yoau-*+"  &&  answer == -1);
	answer = 999;
	assert(evaluate("o/(y-y)", m, pf, answer) == 3 &&
		pf == "oyy-/"  &&  answer == 999);
	assert(evaluate(" a  ", m, pf, answer) == 0 &&
		pf == "a"  &&  answer == 3);
	assert(evaluate("((a))", m, pf, answer) == 0 &&
		pf == "a"  &&  answer == 3);

	string test = "((a+b)/(d/(d))"; //"a+b+\u0061" really, any utf encodings fail
	assert(evaluate(test, map, pf, answer) == 1);// && pf == "ab+dd//" && answer == 3);
	cout << "Passed all tests" << endl;
}