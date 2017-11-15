#pragma once

#include "stdafx.h"
#include "GameLogic.h"

class AI
{
public:
	// Constructor far an AI object / Dificulty determines how many moves in advance the AI checks
	AI(int player, int dificulty);
	~AI() {}


private:
	GameLogic aigl;
	std::vector<int> points;
	int depth;
	int player;

	// Returns the score value a given turn
	int turnScore(std::list<move> turn);
	// Returns a vector of all available turns stored in lists of moves
	std::vector<std::list<move>> generateTurns(node * root);
	// Geats all the end-nodes for a tree
	std::list<node *> getLeaves(node * root);
	// Returns a list of moves that make up a single valid turn
	void generateTurn(node * leaf, std::list<move> &container);
	// Makes a tree for all valid turns startinf with a given move
	node * makeTurnTree(move m, node * parrent);

	// Point values
	const int take = 1;
	const int takeKing = 4;
	const int king = 3;
};