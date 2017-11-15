#pragma once
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




class GameLogic
{
public:
	GameLogic();
	~GameLogic();

	// Executes the selected move / Returns -1 if more moves available, 0 if game is not over, returns winner otherwise
	int executeMove();
	// Executes given move / Returns -1 if more moves available, 0 if game is not over, returns winner otherwise
	int executeMove(move m);
	// Undoes a move
	void undo();
	// Undoes a turn
	void undoMove();
	// Redoes a move
	void redo();

	// Increments piece selection index
	void nextPiece();
	// Decrements piece selection index
	void prevPiece();
	// Increments move selection index
	void nextMove();
	// Decrements move selection index
	void prevMove();
	// Sets up available moves and returns selected piece
	void selectPiece();


	// Returns all available moves for one piece
	std::vector<move> movesAvailable(piece selected_piece, bool can_take);

	// Returns all pieces tha can move and whether the pieces have to take
	std::vector<piece> getPieceChoices(bool &take);

	// Getters
	int getPlayer() { return player; }
	std::array<std::array<space, 8>, 8> getBoard() { return board; }
	std::deque<move> getHistory() { return history; }
	std::stack<move> getRedoStack() { return redoStack; }
	std::vector<move> getMoves() { return moves; }
	int getWinner() { return winner; }

private:
	std::array<std::array<space, 8>, 8> board;	// stores the current board configuration
	std::list<piece> playerPieces[2];	// All the pieces that belong to players
	int winner = 0;			// When a winner is determined this gets assigned to either 1 or 2
	std::deque<move> history;	// Contains all moves made in a game
	std::stack<move> redoStack;	// Contains all moves that have been undone

	// Finds a piece based on coordinates / If piece is not found returns a made up piece at 9, 9
	piece findPiece(int row, int col, int player);
	// Makes all the pieces on the board unselected
	void clearSelection();
	// Part of movesAvailble method to reduce ammount of code / Returns all available taking moves for a selected king
	std::vector<move> movesHelperKingTake(piece selected_piece, int opponent, int opponentKing, std::vector<move> moves, int iterator, int leftRight, int upDown);
	// Part of movesAvailble method to reduce ammount of code / Returns all available non-taking moves for a selected king
	std::vector<move> movesHelperKingMove(piece selected_piece, std::vector<move> moves, int iterator, int leftRight, int upDown);
	// Part of movesAvailble method to reduce ammount of code / Returns all available taking moves for a selected non-king
	std::vector<move> movesHelperTake(piece selected_piece, int opponent, int opponentKing, std::vector<move> moves, int leftRight, int upDown);
	// Part of movesAvailble method to reduce ammount of code / Returns all available non-taking moves for a selected non-king
	std::vector<move> movesHelperMove(piece selected_piece, std::vector<move> moves, int leftRight, int upDown);
	// Removes invalid moves when extra takes are available
	std::vector<move> validateKingTake(std::vector<move> moves);
	// Returns a list of pieces which can take another piece (therefore player has to move one of them)
	std::vector<piece> haveToMove();
	// Returns a list of pieces which can move, but not take
	std::vector<piece> canMove();
	// Recreates the choices vector
	void updatePieceChoices();
	// Changes the indicated piece on the board to selected
	void showSelectedPiece();
	// Calls showSelectedPiece and changes the end coordinate of selected move to the moveTo enum
	void showSelectedMove();

	// Internal state variables
	std::vector<piece> choices;	// Pieces the player can choose from
	std::vector<move> moves;	// Moves the player can choose from
	int player;					// Active player
	bool can_take;				// Whether the selected piece can take or not
	int selectedPiece;			// Piece selection iterator
	int selectedMove;			// Move selection iterator
};