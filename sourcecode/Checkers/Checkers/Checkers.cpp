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
	unused,
	moveTo
};

// Container for information about a piece
struct piece
{
	bool king;
	int row;
	int column;
};

// Container for tracking moves
struct move
{
	int startRow;
	int startColumn;
	int endRow;
	int endColumn;
	bool king;
	bool taken;
	piece takenPiece;
};

space board[8][8];	// stores the current board configuration
char c_for_e[11];	// characters that represent spaces on the board
bool player1;		// True for player, false for AI
bool player2;		// True for player, false for AI
std::list<piece> playerPieces[2];	// All the pieces that belong to players
int winner;			// When a winner is determined this gets assigned to either 1 or 2
std::deque<move> history;	// Contains all moves made in a game
std::queue<move> redoQ;	// Contains all moves that have been undone




// == operator for comaring piece structs
bool operator ==(const piece & lhs, const piece & rhs)
{
	return std::tie(lhs.row, lhs.column, lhs.king) == std::tie(rhs.row, rhs.column, rhs.king);
}



// Finds a piece based on coordinates / If piece is not found returns a made up piece at 9, 9
piece findPiece(int row, int col, int player)
{
	for (piece p : playerPieces[player - 1])
		if (p.row == row && p.column == col)
			return p;
	piece p;
	p.row = 9;
	p.column = 9;
	p.king = false;
	return p;
}



// Prints the current board
void printBoard()
{
	// Print some empty space to separate from previous prints
	std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] == black || board[i][j] == blackKing || board[i][j] == selectedBlack || board[i][j] == selectedBlackKing)
				std::cout << (char)201 << (char)205 << (char)205 << (char)205 << (char)187;
			else if (board[i][j] == white || board[i][j] == whiteKing || board[i][j] == selectedWhite || board[i][j] == selectedWhiteKing)
				std::cout << (char)218 << (char)196 << (char)196 << (char)196 << (char)191;
			else if (board[i][j] == moveTo)
				std::cout << "     ";
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
			else if (board[i][j] == moveTo)
				std::cout << " " << " " << c_for_e[board[i][j]] << " " << " ";
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
			else if (board[i][j] == moveTo)
				std::cout << "     ";
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
	int own;
	int ownKing;
	if (player == 1)
	{
		opponent = black;
		opponentKing = blackKing;
		own = white;
		ownKing = whiteKing;
	}
	else
	{
		opponent = white;
		opponentKing = whiteKing;
		own = black;
		ownKing = blackKing;
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
			// Enough space down
			if (p.row < 6)
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
					if (p.column - i > 0)
					{
						// Can take up-left
						if ((board[p.row - i][p.column - i] == opponent || board[p.row - i][p.column - i] == opponentKing)
							&& board[p.row - i - 1][p.column - i - 1] == empty)
						{
							l.push_back(p);
							break;
						}
						else if (board[p.row - i][p.column - i] == own || board[p.row - i][p.column - i] == ownKing)
							break;	// Break if blocked by own piece
					}
					if (p.column + i < 7)
					{
						// Can take up-right
						if ((board[p.row - i][p.column + i] == opponent || board[p.row - i][p.column + i] == opponentKing)
							&& board[p.row - i - 1][p.column + i + 1] == empty)
						{
							l.push_back(p);
							break;
						}
						else if (board[p.row - i][p.column + i] == own || board[p.row - i][p.column + i] == ownKing)
							break;	// Break if blocked by own piece
					}
				}
				// Enough space down
				else if (p.row + i < 7)
				{
					if (p.column - i > 0)
					{
						// Can take down-left
						if ((board[p.row + i][p.column - i] == opponent || board[p.row + i][p.column - i] == opponentKing)
							&& board[p.row + i + 1][p.column - i - 1] == empty)
						{
							l.push_back(p);
							break;
						}
						else if (board[p.row + i][p.column - i] == own || board[p.row + i][p.column - i] == ownKing)
							break;	// Break if blocked by own piece
					}
					// Can take down-right
					if (p.column + i < 7)
					{
						if ((board[p.row + i][p.column + i] == opponent || board[p.row + i][p.column + i] == opponentKing)
							&& board[p.row + i + 1][p.column + i + 1] == empty)
						{
							l.push_back(p);
							break;
						}
						else if (board[p.row + i][p.column + i] == own || board[p.row + i][p.column + i] == ownKing)
							break;	// Break if blocked by own piece
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
						continue;
					}
					// Can move up-right
					if (p.column < 7 && board[p.row - 1][p.column + 1] == empty)
					{
						l.push_back(p);
						continue;
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
						continue;
					}
					// Can move down-right
					if (p.column < 7 && board[p.row + 1][p.column + 1] == empty)
					{
						l.push_back(p);
						continue;
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
					if (p.column + i <= 7 && board[p.row - i][p.column + i] == empty)
					{
						l.push_back(p);
						break;
					}
				}
				if (p.row + i <= 7)
				{
					// Can move up-left
					if (p.column - i >= 0 && board[p.row + i][p.column - i] == empty)
					{
						l.push_back(p);
						break;
					}
					// Can move up-right
					if (p.column + i <= 7 && board[p.row + i][p.column + i] == empty)
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
			else if (board[i][j] == moveTo)
				board[i][j] = empty;
		}
}



// Part of movesAvailble method to reduce ammount of code / Returns all available taking moves for a selected king
std::vector<move> movesHelperKingTake(piece selected_piece, int opponent, int opponentKing, std::vector<move> moves, int iterator, int leftRight, int upDown)
{
	int i = iterator;
	if ((board[selected_piece.row + i * upDown][selected_piece.column + i * leftRight] == opponent
		|| board[selected_piece.row + i * upDown][selected_piece.column + i * leftRight] == opponentKing)
		&& board[selected_piece.row + (i + 1) * upDown][selected_piece.column + (i + 1) * leftRight] == empty)
	{
		move m;
		m.startRow = selected_piece.row;
		m.startColumn = selected_piece.column;
		m.endRow = selected_piece.row + (i + 1)  * upDown;
		m.endColumn = selected_piece.column + (i + 1) * leftRight;
		m.king = selected_piece.king;
		m.taken = true;
		m.takenPiece = findPiece(selected_piece.row + i * upDown, selected_piece.column + i * leftRight, opponent == white ? 1 : 2);
		for (int j = i + 1; j < 8; j++)
		{
			bool enoughSpace = false;
			// Enough space
			if (upDown < 0 && selected_piece.row - j >= 0)
			{
				if (leftRight < 0 && selected_piece.column - j >= 0)
					enoughSpace = true;
				else if (leftRight > 0 && selected_piece.column + j <= 7)
					enoughSpace = true;
			}
			else if (upDown > 0 && selected_piece.row + j <= 7)
				if (leftRight < 0 && selected_piece.column - j >= 0)
					enoughSpace = true;
				else if (leftRight > 0 && selected_piece.column + j <= 7)
					enoughSpace = true;

			if (enoughSpace)
			{
				if (board[selected_piece.row + j * upDown][selected_piece.column + j * leftRight] == empty)
				{
					m.endRow = selected_piece.row + j * upDown;
					m.endColumn = selected_piece.column + j * leftRight;
					moves.push_back(m);
				}
				else
					break;
			}
		}
	}
	return moves;
}



// Part of movesAvailble method to reduce ammount of code / Returns all available non-taking moves for a selected king
std::vector<move> movesHelperKingMove(piece selected_piece, std::vector<move> moves, int iterator, int leftRight, int upDown)
{
	int i = iterator;
	bool enoughSpace = false;
	// Enough space
	if (upDown < 0 && selected_piece.row - i >= 0)
	{
		if (leftRight < 0 && selected_piece.column - i >= 0)
			enoughSpace = true;
		else if (leftRight > 0 && selected_piece.column + i <= 7)
			enoughSpace = true;
	}
	else if (upDown > 0 && selected_piece.row + i <= 7)
		if (leftRight < 0 && selected_piece.column - i >= 0)
			enoughSpace = true;
		else if (leftRight > 0 && selected_piece.column + i <= 7)
			enoughSpace = true;

	if (enoughSpace)
	{
		if (board[selected_piece.row + i * upDown][selected_piece.column + i * leftRight] == empty)
		{
			move m;
			m.startRow = selected_piece.row;
			m.startColumn = selected_piece.column;
			m.endRow = selected_piece.row + i * upDown;
			m.endColumn = selected_piece.column + i * leftRight;
			m.king = selected_piece.king;
			m.taken = false;
			moves.push_back(m);
		}
	}
	return moves;
}



// Part of movesAvailble method to reduce ammount of code / Returns all available taking moves for a selected non-king
std::vector<move> movesHelperTake(piece selected_piece, int opponent, int opponentKing, std::vector<move> moves, int leftRight, int upDown)
{
	bool enoughSpace = false;
	// Enough space
	if (upDown < 0 && selected_piece.row - 2 >= 0)
	{
		if (leftRight < 0 && selected_piece.column - 2 >= 0)
			enoughSpace = true;
		else if (leftRight > 0 && selected_piece.column + 2 <= 7)
			enoughSpace = true;
	}
	else if (upDown > 0 && selected_piece.row + 2 <= 7)
		if (leftRight < 0 && selected_piece.column - 2 >= 0)
			enoughSpace = true;
		else if (leftRight > 0 && selected_piece.column + 2 <= 7)
			enoughSpace = true;

	if (enoughSpace
		&& (board[selected_piece.row + upDown][selected_piece.column + leftRight] == opponent
			|| board[selected_piece.row + upDown][selected_piece.column + leftRight] == opponentKing)
		&& board[selected_piece.row + 2 * upDown][selected_piece.column + 2 * leftRight] == empty)
	{
		move m;
		m.startRow = selected_piece.row;
		m.startColumn = selected_piece.column;
		m.endRow = selected_piece.row + 2 * upDown;
		m.endColumn = selected_piece.column + 2 * leftRight;
		m.king = selected_piece.king;
		m.taken = true;
		m.takenPiece = findPiece(selected_piece.row + upDown, selected_piece.column + leftRight, opponent == white ? 1 : 2);
		moves.push_back(m);
	}
	return moves;
}



// Part of movesAvailble method to reduce ammount of code / Returns all available non-taking moves for a selected non-king
std::vector<move> movesHelperMove(piece selected_piece, std::vector<move> moves, int leftRight, int upDown)
{
	bool enoughSpace = false;
	// Enough space
	if (upDown < 0 && selected_piece.row - 1 >= 0)
	{
		if (leftRight < 0 && selected_piece.column - 1 >= 0)
			enoughSpace = true;
		else if (leftRight > 0 && selected_piece.column + 1 <= 7)
			enoughSpace = true;
	}
	else if (upDown > 0 && selected_piece.row + 1 <= 7)
		if (leftRight < 0 && selected_piece.column - 1 >= 0)
			enoughSpace = true;
		else if (leftRight > 0 && selected_piece.column + 1 <= 7)
			enoughSpace = true;

	if (enoughSpace && board[selected_piece.row + upDown][selected_piece.column + leftRight] == empty)
	{
		move m;
		m.startRow = selected_piece.row;
		m.startColumn = selected_piece.column;
		m.endRow = selected_piece.row + upDown;
		m.endColumn = selected_piece.column + leftRight;
		m.king = selected_piece.king;
		m.taken = false;
		moves.push_back(m);
	}
	return moves;
}



// Returns all available moves for one piece
std::vector<move> movesAvailable(int player, piece selected_piece, bool can_take)
{
	std::vector<move> moves = std::vector<move>();
	int opponent;
	int opponentKing;
	int own;
	int ownKing;
	if (player == 1)
	{
		opponent = black;
		opponentKing = blackKing;
		own = white;
		ownKing = whiteKing;
	}
	else
	{
		opponent = white;
		opponentKing = whiteKing;
		own = black;
		ownKing = blackKing;
	}
	// Multipliers that can be 1 or -1 for dealing with signs
	int leftRight = 1;
	int upDown = 1;
	// Moves for a king piece
	if (selected_piece.king)
	{
		// Blocked diagonals
		bool upLeft = false;
		bool upRight = false;
		bool downLeft = false;
		bool downRight = false;
		// Moves for a king piece that can take
		if (can_take)
		{
			for (int i = 1; i < 6; i++)
			{
				// Enough space up
				if (selected_piece.row - i - 1 >= 0)
				{
					// Enough space left
					if (selected_piece.column - i - 1 >= 0 && !upLeft)
					{
						if (board[selected_piece.row - i - 1][selected_piece.column - i - 1] == own
							|| board[selected_piece.row - i - 1][selected_piece.column - i - 1] == ownKing
							|| (board[selected_piece.row - i][selected_piece.column - i] != empty
								&& board[selected_piece.row - i - 1][selected_piece.column - i - 1] != empty))
							upLeft = true;
						else
							// Can take up-left
							moves = movesHelperKingTake(selected_piece, opponent, opponentKing, moves, i, -1, -1);
					}
					// Enough space right
					if (selected_piece.column + i + 1 <= 7 && !upRight)
					{
						if (board[selected_piece.row - i - 1][selected_piece.column + i + 1] == own
							|| board[selected_piece.row - i - 1][selected_piece.column + i + 1] == ownKing
							|| (board[selected_piece.row - i][selected_piece.column + i] != empty
								&& board[selected_piece.row - i - 1][selected_piece.column + i + 1] != empty))
							upRight = true;
						else
							// Can take up-right
							moves = movesHelperKingTake(selected_piece, opponent, opponentKing, moves, i, +1, -1);
					}
				}
				// Enough space down
				if (selected_piece.row + i + 1 <= 7)
				{
					// Enough space left
					if (selected_piece.column - i - 1 >= 0 && !downLeft)
					{
						if (board[selected_piece.row + i + 1][selected_piece.column - i - 1] == own
							|| board[selected_piece.row + i + 1][selected_piece.column - i - 1] == ownKing
							|| (board[selected_piece.row + i][selected_piece.column - i] != empty
								&& board[selected_piece.row + i + 1][selected_piece.column - i - 1] != empty))
							downLeft = true;
						else
							// Can take down-left
							moves = movesHelperKingTake(selected_piece, opponent, opponentKing, moves, i, -1, 1);
					}
					// Enough space right
					if (selected_piece.column + i + 1 <= 7 && !downRight)
					{
						if (board[selected_piece.row + i + 1][selected_piece.column + i + 1] == own
							|| board[selected_piece.row + i + 1][selected_piece.column + i + 1] == ownKing
							|| (board[selected_piece.row + i][selected_piece.column + i] != empty
								&& board[selected_piece.row + i + 1][selected_piece.column + i + 1] != empty))
							downRight = true;
						else
							// Can take down-right
							moves = movesHelperKingTake(selected_piece, opponent, opponentKing, moves, i, 1, 1);
					}
				}
			}
		}
		// Moves for a king piece that can't take
		else
		{
			for (int i = 1; i < 7; i++)
			{
				// Get moves for up-left, up-right, down-left, down-right respectively
				moves = movesHelperKingMove(selected_piece, moves, i, -1, -1);
				moves = movesHelperKingMove(selected_piece, moves, i, 1, -1);
				moves = movesHelperKingMove(selected_piece, moves, i, -1, 1);
				moves = movesHelperKingMove(selected_piece, moves, i, 1, 1);
			}
		}
	}
	// Moves for a non-king piece
	else
	{
		// Moves for a non-king piece that can take
		if (can_take)
		{
			// Get moves for up-left, up-right, down-left, down-right respectively
			moves = movesHelperTake(selected_piece, opponent, opponentKing, moves, -1, -1);
			moves = movesHelperTake(selected_piece, opponent, opponentKing, moves, 1, -1);
			moves = movesHelperTake(selected_piece, opponent, opponentKing, moves, -1, 1);
			moves = movesHelperTake(selected_piece, opponent, opponentKing, moves, 1, 1);
		}
		// Moves for a non-king piece that can't take
		else
		{
			// Player 1 only moves up
			if (player == 1)
			{
				// Get moves for up-left, up-right respectively
				moves = movesHelperMove(selected_piece, moves, -1, -1);
				moves = movesHelperMove(selected_piece, moves, 1, -1);
			}
			// Player 2 only moves down
			else
			{
				// Get moves for down-left, down-right respectively
				moves = movesHelperMove(selected_piece, moves, -1, 1);
				moves = movesHelperMove(selected_piece, moves, 1, 1);
			}
		}
	}
	return moves;
}



// Executes the selected move
void executeMove(move m, int player)
{
	history.push_back(m);
	for (piece &p : playerPieces[player == 1 ? 0 : 1])
		if (p.row == m.startRow && p.column == m.startColumn)
		{
			// Updatet player pieces
			p.row = m.endRow;
			p.column = m.endColumn;
			// Update board
			board[m.startRow][m.startColumn] = empty;
			if (p.king)
				if (player == 1)
					board[m.endRow][m.endColumn] = whiteKing;
				else
					board[m.endRow][m.endColumn] = blackKing;
			else
				if (player == 1)
					if (m.endRow == 0)
					{
						board[m.endRow][m.endColumn] = whiteKing;
						p.king = true;
					}
					else
						board[m.endRow][m.endColumn] = white;
				else
					if (m.endRow == 7)
					{
						board[m.endRow][m.endColumn] = blackKing;
						p.king = true;
					}
					else
						board[m.endRow][m.endColumn] = black;
			break;
		}
	// If a piece is taken update opponents pieces and board
	if (m.taken)
	{
		for (piece p : playerPieces[player == 1 ? 1 : 0])
			if (p.row == m.takenPiece.row && p.column == m.takenPiece.column)
			{
				playerPieces[player == 1 ? 1 : 0].remove(p);
				break;
			}
		board[m.takenPiece.row][m.takenPiece.column] = empty;
	}
}



// Removes invalid moves when extra takes are available
std::vector<move> validateKingTake(std::vector<move> moves, int player)
{
	std::vector<move> movesWithTakes = std::vector<move>();
	std::vector<move> movesValidated = std::vector<move>();
	std::vector<std::pair<int, int>> takenVal = std::vector<std::pair<int, int>>();

	for (move m : moves)
	{
		piece p;
		p.row = m.endRow;
		p.column = m.endColumn;
		space temp = board[m.takenPiece.row][m.takenPiece.column];
		board[m.takenPiece.row][m.takenPiece.column] = player == 1 ? white : black;
		std::vector<move> takingMoves = movesAvailable(player, p, true);
		if (takingMoves.size() > 0)
			movesWithTakes.push_back(m);
		board[m.takenPiece.row][m.takenPiece.column] = temp;
	}
	for (move m : movesWithTakes)
		movesValidated.push_back(m);
	for (move m : moves)
	{
		bool in = false;
		for (move mwt : movesWithTakes)
			if (m.takenPiece.row == mwt.takenPiece.row && m.takenPiece.column == mwt.takenPiece.column)
				in = true;
		if (!in)
			movesValidated.push_back(m);
	}
	return movesValidated;
}



// Undoes a move
void undo(int player)
{
	bool samePlayer = true;
	while (true)
	{
		if (history.size() == 0)
			break;
		move m = history.back();
		space s = board[m.endRow][m.endColumn];
		if (((s == white || s == whiteKing) && player == 1) || ((s == black || s == blackKing) && player == 2))
			//samePlayer = false;
			break;
		// Move the 'move' to redo queue
		redoQ.push(m);
		history.pop_back();
		// If kinged this turn make the piece not a king
		if (s == whiteKing && !m.king)
			s = white;
		else if (s == blackKing && !m.king)
			s = black;
		// Update board
		board[m.endRow][m.endColumn] = empty;
		board[m.startRow][m.startColumn] = s;
		for (piece &p : playerPieces[player == 2 ? 0 : 1])
			if (m.endRow == p.row && m.endColumn == p.column)
			{
				p.row = m.startRow;
				p.column = m.startColumn;
				p.king = m.king;
			}
		// Return piece to the piece list if it was taken
		if (m.taken)
		{
			playerPieces[player == 1 ? 0 : 1].push_back(m.takenPiece);
			if (m.takenPiece.king)
				board[m.takenPiece.row][m.takenPiece.column] = player == 2 ? blackKing : whiteKing;
			else
				board[m.takenPiece.row][m.takenPiece.column] = player == 2 ? black : white;
		}
	}
}




// Main
int main()
{
	playerPieces[0] = std::list<piece>();;
	playerPieces[1] = std::list<piece>();;
	history = std::deque<move>();
	redoQ = std::queue<move>();
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
	/*{
		board[1][4] = blackKing;
		piece p;
		p.row = 1;
		p.column = 4;
		p.king = true;
		playerPieces[1].push_back(p);
	}
	{
		board[4][3] = blackKing;
		piece p;
		p.row = 4;
		p.column = 3;
		p.king = true;
		playerPieces[1].push_back(p);
	}
	{
		board[4][1] = blackKing;
		piece p;
		p.row = 4;
		p.column = 1;
		p.king = true;
		playerPieces[1].push_back(p);
	}
	{
		board[0][3] = whiteKing;
		piece p;
		p.row = 0;
		p.column = 3;
		p.king = true;
		playerPieces[0].push_back(p);
	}*/

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
	c_for_e[moveTo] = 'X';

	std::vector<piece> choices = std::vector<piece>();	// A list containing all pieces a player can move (updated each turn)
	int player = 1;
	bool can_take = false;	// Used in move selection to to get available moves
	int selected = 0;	// Saves which piece is selected
	// Game loop
	while (playerPieces[0].size() > 0 && playerPieces[1].size() > 0)
	{
		// Get pieces that can move
		choices = haveToMove(player);
		can_take = true;
		if (choices.size() == 0)
		{
			can_take = false;
			choices = canMove(player);
			// If all movement is blocked the player loses
			if (choices.size() == 0)
			{
				winner = player == 1 ? 2 : 1;
				break;
			}
		}

		selected = 0;
		// Cycle through selections until a piece is selected
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
			printBoard();

			// Select next piece
			std::string tmp = "";
			std::cout << "Press ENTER for next available piece." << std::endl;
			std::cout << "Press + or # followed by ENTER for previous available piece." << std::endl;
			if (history.size() > 0)
				std::cout << "Press U to undo a move." << std::endl;
			std::cout << "Press SPACE or 0 followed by ENTER to choose the selected piece." << std::endl;

			std::cout << history.size() << std::endl;

			std::getline(std::cin, tmp);
			// Next piece
			if (tmp == "")
			{
				if (selected + 1 >= choices.size())
					selected = 0;
				else
					selected++;
			}
			// Previous piece
			else if (tmp == "+" || tmp == "#")
			{
				if (selected - 1 < 0)
					selected = choices.size() - 1;
				else
					selected--;
			}
			// Undo
			else if (tmp == "u" || tmp == "U")
			{
				clearSelection();
				undo(player);
				break;
			}
			// Select piece
			else if (tmp == " " || tmp == "0")
			{
				piece selectedPiece = choices[selected];
				std::vector<move> moves = movesAvailable(player, selectedPiece, can_take);

				// Cycle through available moves until a move is selected
				selected = 0;
				while (true)
				{
					// Validation for king pieces that can take and may be able to take after the first take
					if (selectedPiece.king && can_take)
						moves = validateKingTake(moves, player);
					// Show selection
					clearSelection();
					if (player == 1)
					{
						if (selectedPiece.king)
							board[selectedPiece.row][selectedPiece.column] = selectedWhiteKing;
						else
							board[selectedPiece.row][selectedPiece.column] = selectedWhite;
					}
					else
					{
						if (selectedPiece.king)
							board[selectedPiece.row][selectedPiece.column] = selectedBlackKing;
						else
							board[selectedPiece.row][selectedPiece.column] = selectedBlack;
					}
					board[moves[selected].endRow][moves[selected].endColumn] = moveTo;
					printBoard();

					// Select next move
					tmp = "";
					std::cout << "Press ENTER for next available move." << std::endl;
					std::cout << "Press + or # followed by ENTER for previous available move." << std::endl;
					std::cout << "Press SPACE or 0 followed by ENTER to choose the selected move." << std::endl;
					std::cout << "Press C followed by ENTER to cancel selection." << std::endl;
					std::getline(std::cin, tmp);

					// Next move
					if (tmp == "")
					{
						if (selected + 1 >= moves.size())
							selected = 0;
						else
							selected++;
					}
					// Previous move
					else if (tmp == "+" || tmp == "#")
					{
						if (selected - 1 < 0)
							selected = moves.size() - 1;
						else
							selected--;
					}
					// Move selected
					else if (tmp == " " || tmp == "0")
					{
						//	history.push_back(moves[selected]);
						executeMove(moves[selected], player);
						for (piece p : playerPieces[player == 1 ? 0 : 1])
							if (p.row == moves[selected].endRow && p.column == moves[selected].endColumn)
								selectedPiece = p;
						moves = movesAvailable(player, selectedPiece, true);
						if (moves.size() > 0 && can_take)
						{
							selected = 0;
							continue;
						}
						break;
					}
					// Cancel piece selection
					else if (tmp == "c" && tmp != "C")
						break;
					// Any other button pressed
					else
						continue;
				}
				if (tmp != "c" && tmp != "C")
					break;
			}
		}

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

