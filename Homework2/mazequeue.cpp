#include <string>
#include <queue>
#include <iostream>
#include <time.h>
using namespace std;

void display(string maze[], int nRows);
void clearScreen();

class Coord
{
public:
	Coord(int rr, int cc) : m_r(rr), m_c(cc) {}
	int r() const { return m_r; }
	int c() const { return m_c; }
private:
	int m_r;
	int m_c;
};

bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec)
{
	queue<Coord> s;
	s.push(Coord(sr, sc));
	maze[sr][sc] = ' ';
	while (!s.empty())
	{
		Coord current = s.front();
		int r = current.r();
		int c = current.c();
		maze[r][c] = '@';
		s.pop();

		if (r == er && c == ec)
		{
//			display(maze, nRows);
			return true;
		}
		if (c + 1 < nCols && maze[r][c + 1] == '.')
		{
			s.push(Coord(r, c + 1));
			maze[r][c + 1] = '-';
		}
		if (r + 1 < nRows && maze[r + 1][c] == '.')
		{
			s.push(Coord(r + 1, c));
			maze[r + 1][c] = '-';
		}
		if (c - 1 >= 0 && maze[r][c - 1] == '.')
		{
			s.push(Coord(r, c - 1));
			maze[r][c - 1] = '-';
		}
		if (r - 1 >= 0 && maze[r - 1][c] == '.')
		{
			s.push(Coord(r - 1, c));
			maze[r - 1][c] = '-';
		}
//		display(maze, nRows);
		maze[r][c] = ' ';
	}
	return false;
}

void wait(int seconds)
{
	int n = time(nullptr);
	while (time(nullptr) - n < seconds);
}

void display(string maze[], int nRows)
{
	clearScreen();
	for (int i = 0; i < nRows; i++)
	{
		cerr << maze[i] << endl;
	}
	wait(1);
}

int main()
{
	string maze[10] = {
		"XXXXXXXXXX",
		"X...X..X.X",
		"X..XX....X",
		"X.X.XXXX.X",
		"XXX......X",
		"X...X.XX.X",
		"X.X.X..X.X",
		"X.XXXX.X.X",
		"X..X...X.X",
		"XXXXXXXXXX"
	};

	if (pathExists(maze, 10, 10, 4, 3, 1, 8))
		cout << "Solvable!" << endl;
	else
		cout << "Out of luck!" << endl;
}

#ifdef _MSC_VER  //  Microsoft Visual C++

#include <windows.h>

void clearScreen()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	COORD upperLeft = { 0, 0 };
	DWORD dwCharsWritten;
	FillConsoleOutputCharacter(hConsole, TCHAR(' '), dwConSize, upperLeft,
		&dwCharsWritten);
	SetConsoleCursorPosition(hConsole, upperLeft);
}

#else  // not Microsoft Visual C++, so assume UNIX interface

#include <iostream>
#include <cstring>
#include <cstdlib>

void clearScreen()  // will just write a newline in an Xcode output window
{
	static const char* term = getenv("TERM");
	if (term == nullptr || strcmp(term, "dumb") == 0)
		cout << endl;
	else
	{
		static const char* ESC_SEQ = "\x1B[";  // ANSI Terminal esc seq:  ESC [
		cout << ESC_SEQ << "2J" << ESC_SEQ << "H" << flush;
	}
}

#endif