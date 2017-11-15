#include "stdafx.h"
#include "GameLogic.h"



// == operator for comaring piece structs
bool operator ==(const piece & lhs, const piece & rhs)
{
	return std::tie(lhs.row, lhs.column, lhs.king) == std::tie(rhs.row, rhs.column, rhs.king);
}


// Constructor initializes the board sets up starting variables
GameLogic::GameLogic()
{
	playerPieces[0] = std::list<piece>();;
	playerPieces[1] = std::list<piece>();;
	history = std::deque<move>();
	redoStack = std::stack<move>();
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

	// Debug pieces
	/*{
		board[2][3] = whiteKing;
		piece p;
		p.row = 2;
		p.column = 3;
		p.king = true;
		playerPieces[0].push_back(p);
	}
	{
		board[3][4] = black;
		piece p;
		p.row = 3;
		p.column = 4;
		p.king = false;
		playerPieces[1].push_back(p);
	}
	{
		board[1][2] = blackKing;
		piece p;
		p.row = 1;
		p.column = 2;
		p.king = true;
		playerPieces[1].push_back(p);
	}*/

	player = 1;
	updatePieceChoices();
}

GameLogic::~GameLogic()
{
}




// Finds a piece based on coordinates / If piece is not found returns a made up piece at 9, 9
piece GameLogic::findPiece(int row, int col, int player)
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



// Returns a list of pieces which can take another piece (therefore player has to move one of them)
std::vector<piece> GameLogic::haveToMove()
{
	std::vector<piece> ch = std::vector<piece>();
	for (piece p : playerPieces[player - 1])
		if (movesAvailable(p, true).size() > 0)
			ch.push_back(p);
	return ch;
}



// Returns a list of pieces which can move, but not take
std::vector<piece> GameLogic::canMove()
{
	std::vector<piece> ch = std::vector<piece>();
	for (piece p : playerPieces[player - 1])
		if (movesAvailable(p, false).size() > 0)
			ch.push_back(p);
	return ch;
}



// Makes all the pieces on the board unselected
void GameLogic::clearSelection()
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
std::vector<move> GameLogic::movesHelperKingTake(piece selected_piece, int opponent, int opponentKing, std::vector<move> mv, int iterator, int leftRight, int upDown)
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
			// Checks whether the move would end beyond the board
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
					mv.push_back(m);
				}
				else
					break;
			}
		}
	}
	return mv;
}



// Part of movesAvailble method to reduce ammount of code / Returns all available non-taking moves for a selected king
std::vector<move> GameLogic::movesHelperKingMove(piece selected_piece, std::vector<move> mv, int iterator, int leftRight, int upDown)
{
	int i = iterator;
	bool enoughSpace = false;
	// Checks whether the move would end beyond the board
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
			mv.push_back(m);
		}
	}
	return mv;
}



// Part of movesAvailble method to reduce ammount of code / Returns all available taking moves for a selected non-king
std::vector<move> GameLogic::movesHelperTake(piece selected_piece, int opponent, int opponentKing, std::vector<move> mv, int leftRight, int upDown)
{
	bool enoughSpace = false;
	// Checks whether the move would end beyond the board
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
		mv.push_back(m);
	}
	return mv;
}




// Part of movesAvailble method to reduce ammount of code / Returns all available non-taking moves for a selected non-king
std::vector<move> GameLogic::movesHelperMove(piece selected_piece, std::vector<move> mv, int leftRight, int upDown)
{
	bool enoughSpace = false;
	// Checks whether the move would end beyond the board
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
		mv.push_back(m);
	}
	return mv;
}



// Returns all available moves for one piece
std::vector<move> GameLogic::movesAvailable(piece selected_piece, bool can_take)
{
	std::vector<move> mv = std::vector<move>();
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
			int tmp;
			tmp = mv.size();
			for (int i = 1; i < 6; i++)
			{
				// Enough space up
				if (selected_piece.row - i - 1 >= 0)
				{
					// Enough space left
					if (selected_piece.column - i - 1 >= 0 && !upLeft)
					{
						if (board[selected_piece.row - i][selected_piece.column - i] == own
							|| board[selected_piece.row - i][selected_piece.column - i] == ownKing
							|| (board[selected_piece.row - i][selected_piece.column - i] != empty
								&& board[selected_piece.row - i - 1][selected_piece.column - i - 1] != empty))
							upLeft = true;
						else
						{
							// Can take up-left
							mv = movesHelperKingTake(selected_piece, opponent, opponentKing, mv, i, -1, -1);
							if (tmp < mv.size())
								upLeft = true;
						}

					}
					// Enough space right
					if (selected_piece.column + i + 1 <= 7 && !upRight)
					{
						if (board[selected_piece.row - i][selected_piece.column + i] == own
							|| board[selected_piece.row - i][selected_piece.column + i] == ownKing
							|| (board[selected_piece.row - i][selected_piece.column + i] != empty
								&& board[selected_piece.row - i - 1][selected_piece.column + i + 1] != empty))
							upRight = true;
						else
						{
							// Can take up-right
							mv = movesHelperKingTake(selected_piece, opponent, opponentKing, mv, i, +1, -1);
							if (tmp < mv.size())
								upRight = true;
						}
					}
				}
				// Enough space down
				if (selected_piece.row + i + 1 <= 7)
				{
					// Enough space left
					if (selected_piece.column - i - 1 >= 0 && !downLeft)
					{
						if (board[selected_piece.row + i][selected_piece.column - i] == own
							|| board[selected_piece.row + i][selected_piece.column - i] == ownKing
							|| (board[selected_piece.row + i][selected_piece.column - i] != empty
								&& board[selected_piece.row + i + 1][selected_piece.column - i - 1] != empty))
							downLeft = true;
						else
						{
							// Can take down-left
							mv = movesHelperKingTake(selected_piece, opponent, opponentKing, mv, i, -1, 1);
							if (tmp < mv.size())
								downLeft = true;
						}
					}
					// Enough space right
					if (selected_piece.column + i + 1 <= 7 && !downRight)
					{
						if (board[selected_piece.row + i][selected_piece.column + i] == own
							|| board[selected_piece.row + i][selected_piece.column + i] == ownKing
							|| (board[selected_piece.row + i][selected_piece.column + i] != empty
								&& board[selected_piece.row + i + 1][selected_piece.column + i + 1] != empty))
							downRight = true;
						else
						{
							// Can take down-right
							mv = movesHelperKingTake(selected_piece, opponent, opponentKing, mv, i, 1, 1);
							if (tmp < mv.size())
								downRight = true;
						}
					}
				}
			}
		}
		// Moves for a king piece that can't take
		else
		{
			// Blocked diagonals
			upLeft = false;
			upRight = false;
			downLeft = false;
			downRight = false;
			int tmp;
			tmp = mv.size();
			for (int i = 1; i < 7; i++)
			{
				// Get moves for up-left, up-right, down-left, down-right respectively
				if (!upLeft)
				{
					mv = movesHelperKingMove(selected_piece, mv, i, -1, -1);
					if (tmp == mv.size())
						upLeft = true;
				}
				if (!upRight)
				{
					mv = movesHelperKingMove(selected_piece, mv, i, 1, -1);
					if (tmp == mv.size())
						upRight = true;
				}
				if (!downLeft)
				{
					mv = movesHelperKingMove(selected_piece, mv, i, -1, 1);
					if (tmp == mv.size())
						downLeft = true;
				}
				if (!downRight)
				{
					mv = movesHelperKingMove(selected_piece, mv, i, 1, 1);
					if (tmp == mv.size())
						downRight = true;
				}
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
			mv = movesHelperTake(selected_piece, opponent, opponentKing, mv, -1, -1);
			mv = movesHelperTake(selected_piece, opponent, opponentKing, mv, 1, -1);
			mv = movesHelperTake(selected_piece, opponent, opponentKing, mv, -1, 1);
			mv = movesHelperTake(selected_piece, opponent, opponentKing, mv, 1, 1);
		}
		// Moves for a non-king piece that can't take
		else
		{
			// Player 1 only moves up
			if (player == 1)
			{
				// Get moves for up-left, up-right respectively
				mv = movesHelperMove(selected_piece, mv, -1, -1);
				mv = movesHelperMove(selected_piece, mv, 1, -1);
			}
			// Player 2 only moves down
			else
			{
				// Get moves for down-left, down-right respectively
				mv = movesHelperMove(selected_piece, mv, -1, 1);
				mv = movesHelperMove(selected_piece, mv, 1, 1);
			}
		}
	}
	return mv;
}



// Removes invalid moves when extra takes are available
std::vector<move> GameLogic::validateKingTake(std::vector<move> mv)
{
	std::vector<move> movesWithTakes = std::vector<move>();
	std::vector<move> movesValidated = std::vector<move>();
	std::vector<std::pair<int, int>> takenVal = std::vector<std::pair<int, int>>();

	// Gets all moves that can take after taking
	for (move m : mv)
	{
		piece p;
		p.row = m.endRow;
		p.column = m.endColumn;
		space temp = board[m.takenPiece.row][m.takenPiece.column];
		board[m.takenPiece.row][m.takenPiece.column] = empty;
		if (movesAvailable(p, true).size() > 0)
			movesWithTakes.push_back(m);
		board[m.takenPiece.row][m.takenPiece.column] = temp;
	}
	// All moves in movesWithTaking are valid
	for (move m : movesWithTakes)
		movesValidated.push_back(m);
	// Add all moves that that have no additional takes but are still valid
	for (move m : mv)
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



// Executes given move / Returns -1 if more moves available, 0 if game is not over, returns winner otherwise
int GameLogic::executeMove()
{
	move m = moves[selectedMove];
	return executeMove(m);
}



// Executes the selected move / Returns -1 if more moves available, 0 if game is not over, returns winner otherwise
int GameLogic::executeMove(move m)
{
	history.push_back(m);
	while (!redoStack.empty())
	{
		redoStack.pop();
	}
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
	



	piece tmp;
	for (piece p : playerPieces[player == 1 ? 0 : 1])
		if (p.row == m.endRow && p.column == m.endColumn)
			tmp = p;
	moves = movesAvailable(tmp, true);
	if (moves.size() > 0 && can_take)
	{
		updatePieceChoices();
		for (int i = 0; i < choices.size(); i++)
			if (choices[i] == tmp)
			{
				selectedPiece = i;
				selectedMove = 0;
				showSelectedMove();
				return -1;
			}
	}

	// If the player takes the last opponents piece the player wins
	if (playerPieces[player == 1 ? 1 : 0].size() == 0)
	{
		winner = player;
		return winner;
	}

	// Switch to next player
	player = player == 1 ? 2 : 1;
	updatePieceChoices();

	// If all movement is blocked the player loses
	if (choices.size() == 0)
		winner = player == 1 ? 2 : 1;

	return winner;
}



// Undoes a turn
void GameLogic::undo()
{
	if (history.size() == 0)
		return;
	clearSelection();
	while (history.size() > 0)
	{
		move m = history.back();
		space s = board[m.endRow][m.endColumn];
		if (((s == white || s == whiteKing) && player == 1) || ((s == black || s == blackKing) && player == 2))
			break;
		// Move the 'move' to redo stack
		redoStack.push(m);
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
	player = player == 1 ? 2 : 1;
	updatePieceChoices();
}



// Undoes a turn
void GameLogic::undoMove()
{
	if (history.size() == 0)
		return;
	clearSelection();
		move m = history.back();
		space s = board[m.endRow][m.endColumn];
		// Move the 'move' to redo stack
		redoStack.push(m);
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
		if (history.size() == 0)
		{
			player = 1;
			updatePieceChoices();
		}
		if (history.back().endRow == redoStack.top().startRow && history.back().endColumn == redoStack.top().startColumn)
		{
			choices.clear();
			choices.push_back(findPiece(history.back().endRow, history.back().endColumn, (s == white || s == whiteKing) ? 1 : 2));
		}
		else
		{
			player = player == 1 ? 2 : 1;
			updatePieceChoices();
		}
}



// Redoes a turn
void GameLogic::redo()
{
	if (redoStack.size() == 0)
		return;
	clearSelection();
	while (redoStack.size() > 0)
	{
		move m = redoStack.top();
		space s = board[m.startRow][m.startColumn];
		if (((s == white || s == whiteKing) && player == 2) || ((s == black || s == blackKing) && player == 1))
			break;
		// Move the 'move' to history
		history.push_back(m);
		redoStack.pop();
		// Make piece king if required
		if (s == white && m.endRow == 0)
		{
			s = whiteKing;
			m.king = true;
		}
		else if (s == black && m.endRow == 7)
		{
			s = blackKing;
			m.king = true;
		}
		// Update board
		board[m.startRow][m.startColumn] = empty;
		board[m.endRow][m.endColumn] = s;
		for (piece &p : playerPieces[player == 2 ? 1 : 0])
			if (m.startRow == p.row && m.startColumn == p.column)
			{
				p.row = m.endRow;
				p.column = m.endColumn;
				p.king = m.king;
			}
		// Remove piece if one was taken
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
	player = player == 1 ? 2 : 1;
	updatePieceChoices();
}



// Recreates the choices vector
void GameLogic::updatePieceChoices()
{
	// Get pieces that can move
	choices = haveToMove();
	can_take = true;
	if (choices.size() == 0)
	{
		can_take = false;
		choices = canMove();
	}
	selectedPiece = 0;
	showSelectedPiece();
}



// Changes the indicated piece on the board to selected
void GameLogic::showSelectedPiece()
{
	clearSelection();
	// Show selection
	if (player == 1)
	{
		if (choices[selectedPiece].king)
			board[choices[selectedPiece].row][choices[selectedPiece].column] = selectedWhiteKing;
		else
			board[choices[selectedPiece].row][choices[selectedPiece].column] = selectedWhite;
	}
	else
	{
		if (choices[selectedPiece].king)
			board[choices[selectedPiece].row][choices[selectedPiece].column] = selectedBlackKing;
		else
			board[choices[selectedPiece].row][choices[selectedPiece].column] = selectedBlack;
	}
}



// Increments piece selection index
void GameLogic::nextPiece()
{
	if (selectedPiece + 1 >= choices.size())
		selectedPiece = 0;
	else
		selectedPiece++;
	showSelectedPiece();
}



// Decrements piece selection index
void GameLogic::prevPiece()
{
	if (selectedPiece - 1 < 0)
		selectedPiece = choices.size() - 1;
	else
		selectedPiece--;
	showSelectedPiece();
}



// Sets up available moves and returns selected piece
void GameLogic::selectPiece()
{
	moves = movesAvailable(choices[selectedPiece], can_take);

	// Validation for king pieces that can take and may be able to take after the first take
	if (choices[selectedPiece].king && can_take)
		moves = validateKingTake(moves);

	selectedMove = 0;
	showSelectedMove();
}



// Increments move selection index
void GameLogic::nextMove()
{
	if (selectedMove + 1 >= moves.size())
		selectedMove = 0;
	else
		selectedMove++;
	showSelectedMove();
}



// Decrements move selection index
void GameLogic::prevMove()
{
	if (selectedMove - 1 < 0)
		selectedMove = moves.size() - 1;
	else
		selectedMove--;
	showSelectedMove();
}



// Calls showSelectedPiece and changes the end coordinate of selected move to the moveTo enum
void GameLogic::showSelectedMove()
{
	showSelectedPiece();
	board[moves[selectedMove].endRow][moves[selectedMove].endColumn] = moveTo;
}









// Returns all pieces tha can move and whether the pieces have to take
std::vector<piece> GameLogic::getPieceChoices(bool &take)
{
	choices.clear();
	if (winner > 0)
		return choices;
	// Get pieces that can move
	choices = haveToMove();
	can_take = true;
	if (choices.size() == 0)
	{
		can_take = false;
		choices = canMove();
	}
	take = can_take;
	return choices;
}