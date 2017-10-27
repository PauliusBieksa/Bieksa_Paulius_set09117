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
	selectedWhite,
	selectedWhiteKing,
	selectedBlack,
	selectedBlackKing,
	empty,
	unused
};

// Container for information about a piece
struct piece
{
	bool king;
	int row;
	int column;
};

//struct piece
//{
//	bool king;
//	int row;
//	int column;
//};

space board[8][8];	// stores the current board configuration
char c_for_e[10];	// characters that represent spaces on the board
bool player1;		// True for player, false for AI
bool player2;		// True for player, false for AI
std::list<piece> playerPieces[2];	// All the pieces that belong to players
int winner;			// When a winner is determined this gets assigned to either 1 or 2




// Prints the current board
void printBoard()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] == black || board[i][j] == blackKing || board[i][j] == selectedBlack || board[i][j] == selectedBlackKing)
				std::cout << (char)201 << (char)205 << (char)205 << (char)205 << (char)187;
			else if (board[i][j] == white || board[i][j] == whiteKing || board[i][j] == selectedWhite || board[i][j] == selectedWhiteKing)
				std::cout << (char)218 << (char)196 << (char)196 << (char)196 << (char)191;
			else
				std::cout << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]];
		}
		std::cout << std::endl;
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] == black || board[i][j] == blackKing)
				std::cout << (char)186 << " " << c_for_e[board[i][j]] << " " << (char)186;
			else if (board[i][j] == white || board[i][j] == whiteKing)
				std::cout << (char)179 << " " << c_for_e[board[i][j]] << " " << (char)179;
			else if (board[i][j] == selectedBlack || board[i][j] == selectedBlackKing)
				std::cout << (char)186 << "(" << c_for_e[board[i][j]] << ")" << (char)186;
			else if (board[i][j] == selectedWhite || board[i][j] == selectedWhiteKing)
				std::cout << (char)179 << "(" << c_for_e[board[i][j]] << ")" << (char)179;
			else
				std::cout << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]];
		}
		std::cout << std::endl;
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] == black || board[i][j] == blackKing || board[i][j] == selectedBlack || board[i][j] == selectedBlackKing)
				std::cout << (char)200 << (char)205 << (char)205 << (char)205 << (char)188;
			else if (board[i][j] == white || board[i][j] == whiteKing || board[i][j] == selectedWhite || board[i][j] == selectedWhiteKing)
				std::cout << (char)192 << (char)196 << (char)196 << (char)196 << (char)217;
			else
				std::cout << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]] << c_for_e[board[i][j]];
		}
		std::cout << std::endl;
	}
}



// Returns a list of pieces which can take another piece (therefore player has to move one of them)
std::vector<piece> haveToMove(int player)
{
	std::vector<piece> l = std::vector<piece>();
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
	for (piece p : playerPieces[player - 1])
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
			else if (p.row < 6)
			{
				// Can take down-left
				if (p.column > 1
					&& (board[p.row + 1][p.column - 1] == opponent || board[p.row + 1][p.column - 1] == opponentKing)
					&& board[p.row + 2][p.column - 2] == empty)
				{
					l.push_back(p);
				}
				// Can take down-right
				else if (p.column < 6
					&& (board[p.row + 1][p.column + 1] == opponent || board[p.row + 1][p.column + 1] == opponentKing)
					&& board[p.row + 2][p.column + 2] == empty)
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



// Returns a list of pieces which can move, but not take
std::vector<piece> canMove(int player)
{
	std::vector<piece> l = std::vector<piece>();
	for (piece p : playerPieces[player - 1])
	{
		if (!p.king)
		{
			// Player playing white
			if (player == 1)
			{
				// Enough space up
				if (p.row > 0)
				{
					// Can move up-left
					if (p.column > 0 && board[p.row - 1][p.column - 1] == empty)
					{
						l.push_back(p);
					}
					// Can move up-right
					else if (p.column < 7 && board[p.row - 1][p.column + 1] == empty)
					{
						l.push_back(p);
					}
				}
			}
			// Player playing black
			else
			{
				// Enough space down
				if (p.row < 7)
				{
					// Can move down-left
					if (p.column > 0 && board[p.row + 1][p.column - 1] == empty)
					{
						l.push_back(p);
					}
					// Can move down-right
					else if (p.column < 7 && board[p.row + 1][p.column + 1] == empty)
					{
						l.push_back(p);
					}
				}
			}
		}
		// Kings for both players can move in all directions
		else
		{
			for (int i = 1; i <= 7; i++)
			{
				// Enough space up
				if (p.row - i >= 0)
				{
					// Can move up-left
					if (p.column - i >= 0 && board[p.row - i][p.column - i] == empty)
					{
						l.push_back(p);
						break;
					}
					// Can move up-right
					else if (p.column + i <= 7 && board[p.row - i][p.column + i] == empty)
					{
						l.push_back(p);
						break;
					}
				}
				else if (p.row + i >= 7)
				{
					// Can move up-left
					if (p.column - i >= 0 && board[p.row + i][p.column - i] == empty)
					{
						l.push_back(p);
						break;
					}
					// Can move up-right
					else if (p.column + i <= 7 && board[p.row + i][p.column + i] == empty)
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



// Makes all the pieces on the board unselected
void clearSelection()
{
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] == selectedWhite)
				board[i][j] = white;
			else if (board[i][j] == selectedWhiteKing)
				board[i][j] = whiteKing;
			else if (board[i][j] == selectedBlack)
				board[i][j] = black;
			else if (board[i][j] == selectedBlackKing)
				board[i][j] = blackKing;
		}
}









// Main
int main()
{
	playerPieces[0] = std::list<piece>();;
	playerPieces[1] = std::list<piece>();;
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
				playerPieces[1].push_back(p);
			}
			else if (i >= 5 && (i + j) % 2 == 1)
			{
				board[i][j] = white;
				piece p;
				p.row = i;
				p.column = j;
				p.king = false;
				playerPieces[0].push_back(p);
			}
			else if ((i + j) % 2 == 1)
				board[i][j] = empty;
			else
				board[i][j] = unused;
		}


	// debug piece(s)
	{
		board[4][3] = blackKing;
		piece p;
		p.row = 4;
		p.column = 3;
		p.king = true;
		playerPieces[1].push_back(p);
	}

	// Assign chars for enums
	c_for_e[white] = 'w';
	c_for_e[whiteKing] = 'W';
	c_for_e[black] = 'b';
	c_for_e[blackKing] = 'B';
	c_for_e[selectedWhite] = 'w';
	c_for_e[selectedWhiteKing] = 'W';
	c_for_e[selectedBlack] = 'b';
	c_for_e[selectedBlackKing] = 'B';
	c_for_e[empty] = ' ';
	c_for_e[unused] = 219;

	std::vector<piece> choices = std::vector<piece>();	// A list containing all pieces a player can move (updated each turn)
	int player = 1;
	int selected = 0;	// Saves which piece is selected
	// Game loop
	while (playerPieces[0].size() > 0 && playerPieces[1].size() > 0)
	{
		printBoard();
		choices = haveToMove(player);
		if (choices.size() == 0)
		{
			choices = canMove(player);
			// If all movement is blocked the player loses
			if (choices.size() == 0)
			{
				winner = player == 1 ? 2 : 1;
				break;
			}
		}


		selected = 0;
		// Cycle through selections until break is called
		while (true)
		{
			// Unselect previously selected piece
			clearSelection();
			// Show selection
			if (player == 1)
			{
				if (choices[selected].king)
					board[choices[selected].row][choices[selected].column] = selectedWhiteKing;
				else
					board[choices[selected].row][choices[selected].column] = selectedWhite;
			}
			else
			{
				if (choices[selected].king)
					board[choices[selected].row][choices[selected].column] = selectedBlackKing;
				else
					board[choices[selected].row][choices[selected].column] = selectedBlack;
			}
			std::string tmp = "";
			std::cout << "Press ENTER for next available move." << std::endl;
			std::getline(std::cin, tmp);
			if (tmp.length() == 0)
			{
				if (selected + 1 >= choices.size())
					selected = 0;
				else
					selected++;
			}
			else
				break;
			printBoard();
		}
		


		std::cout << "End of debug loop. 0 to leave." << std::endl;
		int wait;
		std::cin >> wait;
		if (wait == 0)
			return 0;


		// If the player takes the last opponents piece the player wins
		if (playerPieces[player == 1 ? 1 : 0].size() == 0)
		{
			winner = player;
			break;
		}

		// Switch to next player
		if (player == 2)
			player = 1;
		else
			player = 2;
	}

	std::cout << "Player " << winner << " won!" << std::endl;
	int wait;
	std::cin >> wait;

	return 0;
}

