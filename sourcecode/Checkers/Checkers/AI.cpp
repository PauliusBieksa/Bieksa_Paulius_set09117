#include "stdafx.h"
#include "AI.h"

// Destructor for the tree structure
void deleteTree(node * root)
{
	for (node * n : root->down)
		deleteTree(n);
	delete root;
	return;
}


// Constructor far an AI object / Dificulty determines how many moves in advance the AI checks
AI::AI(int player, int dificulty)
{
	aigl = GameLogic();
	this->player = player;
	depth = dificulty <= 10 ? dificulty : 10;
}



// Returns the score value a given turn
int AI::turnScore(std::list<move> turn)
{
	int score = 0;
	for (move m : turn)
	{
		if (m.taken)
			if (m.takenPiece.king)
				score += takeKing;
			else
				score += take;
		if ((player == 1 && m.endRow == 0) || (player == 2 & m.endRow == 7))
			score += king;
	}
	return score;
}



// Returns a vector of all available turns stored in lists of moves
std::vector<std::list<move>> AI::generateTurns()
{
	bool can_take;
	std::vector<piece> pieces = aigl.getPieceChoices(can_take);
	std::vector<move> startingMoves = std::vector<move>();
	// Get all the starting moves
	for (piece p : pieces)
	{
		std::vector<move> v = aigl.movesAvailable(p, can_take);
		startingMoves.insert(startingMoves.end(), v.begin(), v.end());
	}
	// Construct turn trees
	std::list<node*> turnTrees = std::list<node*>();
	for (move m : startingMoves)
	{
		node * root = nullptr;
		turnTrees.push_back(makeTurnTree(m, root));
	}
	// Get all the leaf nodes
	std::list<node*> leaves = std::list<node*>();
	for (node * n : turnTrees)
	{
		std::list<node*> l = getLeaves(n);
		leaves.insert(leaves.end(), l.begin(), l.end());
	}
	std::vector<std::list<move>> vs = std::vector<std::list<move>>();
	for (node * n : leaves)
	{
		std::list<move> s = std::list<move>();
		generateTurn(n, s);
		vs.push_back(s);
	}
	for (node * n : turnTrees)
		deleteTree(n);
	return vs;
}



// Returns a list of moves that make up a single valid turn
void AI::generateTurn(node * leaf, std::list<move> &container)
{
	container.push_front(leaf->m);
	if (leaf->up != nullptr)
		generateTurn(leaf->up, container);
}



// Geats all the end-nodes for a tree
std::list<node *> AI::getLeaves(node * root)
{
	std::list<node*> l = std::list<node*>();
	if (root->down.size() == 0)
	{
		l.push_back(root);
		return l;
	}
	else
		for (node * n : root->down)
		{
			std::list<node*> tmp = getLeaves(n);
			l.push_back(tmp.front());
		}
	return l;
}



// Makes a tree for all valid turns starting with a given move
node * AI::makeTurnTree(move m, node * parent)
{
	node * tree = new node();
	tree->up = parent;
	tree->m = m;
	tree->down = std::vector<node*>();
	int moveOutcome = aigl.executeMove(m);
	if (moveOutcome >= 0)
	{
		aigl.undoMove();
		return tree;
	}
	else
	{
		for (move mv : aigl.getMoves())
			tree->down.push_back(makeTurnTree(mv, tree));
	}
	aigl.undoMove();
	return tree;
}



// Makes a tree for all valid turns starting with a given move for a give turn depth (how many turns in advance to consider)
int AI::deepTurnScore(int score, int depth, bool opponentsTurn)
{
	if (depth > 0)
	{
		std::vector<std::list<move>> turns = generateTurns();
		for (std::list<move> l : turns)
		{
			bool win = false;
			score += (opponentsTurn ? -1 : 1) * turnScore(l);
			for (move m : l)
			{
				if (aigl.executeMove(m) > 0)
					win = true;
			}
			if (!win)
				score += deepTurnScore(score, depth - 1, !opponentsTurn);
			aigl.undo();
		}
		return score;
	}
	else
		return score;
}



// Gets the best turn
std::list<move> AI::getBestTurn()
{
	std::vector<std::list<move>> turns = generateTurns();
	std::vector<int> scores;

	for (int i = 0; i < turns.size(); i++)
	{
		scores.push_back(turnScore(turns[i]));
		for (move m : turns[i])
			if (aigl.executeMove(m) > 0)
			{
				aigl.undo();
				return turns[i];
			}
		scores[i] += deepTurnScore(scores[i], depth, true);
		aigl.undo();
	}
	int max = INT_MIN;
	int n = 0;
	for (int i = 0; i < scores.size(); i++)
		if (scores[i] > max)
		{
			max = scores[i];
			n = i;
		}
	return turns[n];
}



// Updates the AI's gameLogic object with the move of the opponent
void AI::playerMove(GameLogic gl)
{
	aigl = gl;
}