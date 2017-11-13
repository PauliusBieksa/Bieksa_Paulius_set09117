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


	// Returns a list of pieces which can take another piece (therefore player has to move one of them)
	std::vector<piece> haveToMove(int player);
	// Returns a list of pieces which can move, but not take
	std::vector<piece> canMove(int player);
	// Returns all available moves for one piece
	std::vector<move> movesAvailable(int player, piece selected_piece, bool can_take);
	// Executes the selected move
	int executeMove();
	// Undoes a move
	void undo();




	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void nextPiece();
	void prevPiece();

	void nextMove();
	void prevMove();

	void selectPiece();



	int getPlayer() { return player; }
	std::array<std::array<space, 8>, 8> getBoard() { return board; }
	std::deque<move> getHistory() { return history; }
	std::queue<move> getRedoQueue() { return redoQ; }


private:

	//space board[8][8];	// stores the current board configuration
	std::array<std::array<space, 8>, 8> board;	// stores the current board configuration
	std::list<piece> playerPieces[2];	// All the pieces that belong to players
	int winner = 0;			// When a winner is determined this gets assigned to either 1 or 2
	std::deque<move> history;	// Contains all moves made in a game
	std::queue<move> redoQ;	// Contains all moves that have been undone

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
	std::vector<move> validateKingTake(std::vector<move> moves, int player);



	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::vector<piece> choices;
	std::vector<move> moves;
	int player;
	bool can_take;
	int selectedPiece;
	int selectedMove;



	void updatePieceChoices();


	void showSelectedPiece();


	void showSelectedMove();

};