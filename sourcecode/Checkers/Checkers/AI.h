#pragma once

#include "stdafx.h"
#include "GameLogic.h"

// Tree structurre all possible moves for a turn
struct node
{
	node * up;
	std::vector<node*> down;
	move m;
};

class AI
{
public:
	// Constructor far an AI object / Dificulty determines how many moves in advance the AI checks
	AI(int player, int dificulty);
	~AI() {}

	// Gets the best turn
	std::list<move> getBestTurn();
	// Updates the AI's gameLogic object
	void playerMove(GameLogic gl);

private:
	GameLogic aigl;
	int depth;
	int player;


	// Returns the score value a given turn
	int turnScore(std::list<move> turn);
	// Returns a vector of all available turns stored in lists of moves
	std::vector<std::list<move>> generateTurns();
	// Geats all the end-nodes for a tree
	std::list<node *> getLeaves(node * root);
	// Returns a list of moves that make up a single valid turn
	void generateTurn(node * leaf, std::list<move> &container);
	// Makes a tree for all valid turns starting with a given move
	node * makeTurnTree(move m, node * parrent);
	// Score all possible turns for a certain depth
	int deepTurnScore(int score, int depth, bool opponentsTurn);

	// Point values
	const int take = 2;
	const int takeKing = 6;
	const int king = 5;
};