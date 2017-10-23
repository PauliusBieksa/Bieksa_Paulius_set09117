// Checkers.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// Enum for all possible values on a single space on a board
enum space
{
	white,
	whiteKing,
	black,
	blackKing,
	empty,
	unused
};

struct piece
{
	bool king;
	int row;
	int column;
};

space board[8][8];	// stores the current board configuration
char c_for_e[6];	// characters that represent spaces on the board
bool player1;		// True for player, false for AI
bool player2;		// True for player, false for AI
std::list<piece> p1_pieces;
std::list<piece> p2_pieces;




// Prints the current board
void printBoard()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] == empty && board[i][j] == unused)
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




//// Prints the current board
//void printBoard()
//{
//	for (int i = 0; i < 8; i++)
//	{
//		for (int j = 0; j < 8; j++)
//		{
//			if (board[i][j] != empty && board[i][j] != unused)
//				std::cout << (char)201 << (char)205 << (char)205 << (char)205 << (char)187;
//			else
//				std::cout << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]];
//		}
//		std::cout << std::endl;
//		for (int j = 0; j < 8; j++)
//		{
//			if (board[i][j] != empty && board[i][j] != unused)
//				std::cout << (char)186 << " " << c_for_e[board[i][j]] << " " << (char)186;
//			else
//				std::cout << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]];
//		}
//		std::cout << std::endl;
//		for (int j = 0; j < 8; j++)
//		{
//			if (board[i][j] != empty && board[i][j] != unused)
//				std::cout << (char)200 << (char)205 << (char)205 << (char)205 << (char)188;
//			else
//				std::cout << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]];
//		}
//		std::cout << std::endl;
//	}
//}




// Returns a list of pieces which can take another piece (therefore player has to move one of them)
std::list<piece> haveToMove(int player)
{
	std::list<piece> l = std::list<piece>();
	int opponent;
	int opponentKing;
	if (player == 1)
	{
		opponent = black;
		opponentKing = blackKing;
	}
	else
	{
		opponent = white;
		opponentKing = whiteKing;
	}

	// Find all mandatory moves
	for (piece p : p1_pieces)
	{
		// if not king
		if (!p.king)
		{
			// Enough space up
			if (p.row > 1)
			{
				// Can take up-left
				if (p.column > 1
					&& (board[p.row - 1][p.column - 1] == opponent || board[p.row - 1][p.column - 1] == opponentKing)
					&& board[p.row - 2][p.column - 2] == empty)
				{
					l.push_back(p);
				}
				// Can take up-right
				else if (p.column < 6
					&& (board[p.row - 1][p.column + 1] == opponent || board[p.row - 1][p.column + 1] == opponentKing)
					&& board[p.row - 2][p.column + 2] == empty)
				{
					l.push_back(p);
				}
			}
		}
		// If king
		else
		{
			for (int i = 1; i < 6; i++)
			{
				// Enough space up
				if (p.row - i > 0)
				{
					// Can take up-left
					if (p.column - i > 0
						&& (board[p.row - i][p.column - i] == opponent || board[p.row - i][p.column - i] == opponentKing)
						&& board[p.row - i - 1][p.column - i - 1] == empty)
					{
						l.push_back(p);
						break;
					}
					// Can take up-right
					else if (p.column + i < 7
						&& (board[p.row - i][p.column + i] == opponent || board[p.row - i][p.column + i] == opponentKing)
						&& board[p.row - i - 1][p.column + i + 1] == empty)
					{
						l.push_back(p);
						break;
					}
				}
				// Enough space down
				else if (p.row + i < 7)
				{
					// Can take down-left
					if (p.column - i > 0
						&& (board[p.row + i][p.column - i] == opponent || board[p.row + i][p.column - i] == opponentKing)
						&& board[p.row + i - 1][p.column - i - 1] == empty)
					{
						l.push_back(p);
						break;
					}
					// Can take down-right
					else if (p.column + i < 7
						&& (board[p.row + i][p.column + i] == opponent || board[p.row + i][p.column + i] == opponentKing)
						&& board[p.row + i - 1][p.column + i + 1] == empty)
					{
						l.push_back(p);
						break;
					}
				}
			}
		}
	}
	return l;
}




// Main
int main()
{
	p1_pieces = std::list<piece>();
	p2_pieces = std::list<piece>();
	// Initialize the board
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (i < 3 && (i + j) % 2 == 1)
			{
				board[i][j] = black;
				piece p;
				p.row = i;
				p.column = j;
				p.king = false;
				p2_pieces.push_back(p);
			}
			else if (i >= 5 && (i + j) % 2 == 1)
			{
				board[i][j] = white;
				piece p;
				p.row = i;
				p.column = j;
				p.king = false;
				p1_pieces.push_back(p);
			}
			else if ((i + j) % 2 == 1)
				board[i][j] = empty;
			else
				board[i][j] = unused;
		}


	// debug piece(s)
	{
		board[4][3] = black;
		piece p;
		p.row = 4;
		p.column = 3;
		p.king = false;
		p1_pieces.push_back(p);
	}

	// Assign chars for enums
	c_for_e[white] = 'w';
	c_for_e[whiteKing] = 'W';
	c_for_e[black] = 'b';
	c_for_e[blackKing] = 'B';
	c_for_e[empty] = ' ';
	c_for_e[unused] = 219;
	
	std::list<piece> choices = std::list<piece>();	// A list containing all pieces a player can move (updated each turn)
	int player = 1;
	while (p1_pieces.size() > 0 && p2_pieces.size() > 0)
	{
		printBoard();
		choices = haveToMove(player);
		

		int wait;
		std::cin >> wait;
		if (wait == 0)
			return 0;
	}

	return 0;
}

