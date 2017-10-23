// Checkers.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

enum space
{
	white,
	whiteKing,
	black,
	blackKing,
	empty,
	unused
};

space board [8][8];
char c_for_e[6];	// characters that represent spaces on the board



// Prints the current board
void printBoard()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] != empty && board[i][j] != unused)
				std::cout << (char)201 << (char)205 << (char)205 << (char)205 << (char)187;
			else
				std::cout << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]];
		}
		std::cout << std::endl;
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] != empty && board[i][j] != unused)
				std::cout << (char)186 << " " << c_for_e[board[i][j]] << " " << (char)186;
			else
				std::cout << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]];
		}
		std::cout << std::endl;
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] != empty && board[i][j] != unused)
				std::cout << (char)200 << (char)205 << (char)205 << (char)205 << (char)188;
			else
				std::cout << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]];
		}
		std::cout << std::endl;
	}
}



int main()
{
	// Initialise the board
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (i < 3 && (i + j) % 2 == 1)
				board[i][j] = black;
			else if (i >= 5 && (i + j) % 2 == 1)
				board[i][j] = white;
			else if ((i + j) % 2 == 1)
				board[i][j] = empty;
			else
				board[i][j] = unused;
		}
	// Assign chars for enums
	c_for_e[white] = 'w';
	c_for_e[whiteKing] = 'W';
	c_for_e[black] = 'b';
	c_for_e[blackKing ] = 'B';
	c_for_e[empty] = ' ';
	c_for_e[unused] = 219;

	printBoard();

	int wait;
	std::cin >> wait;
    return 0;
}

