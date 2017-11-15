#include "stdafx.h"
#include "AI.h"

// Tree structurre all possible moves for a turn
struct node
{
	node * up;
	std::vector<node*> down;
	move m;
};

// Destructor for the tree structure
void deleteTree(node * root)
{
	for (node * n : root->down)
		deleteTree(n);
	delete root->up;
	delete root;
	return;
}


// Constructor far an AI object / Dificulty determines how many moves in advance the AI checks
AI::AI(int player, int dificulty)
{
	aigl = GameLogic();
	points = std::vector<int>();
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



// Returns a vector of all available turns stored in queues of moves
std::vector<std::list<move>> AI::generateTurns(node * root)
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
			l.insert(l.end(), getLeaves(n).begin(), getLeaves(n).end());
	return l;
}



// Makes a tree for all valid turns startinf with a given move
node * AI::makeTurnTree(move m, node * parrent)
{
	node * tree = new node();
	if (parrent != nullptr)
		tree->up = parrent;
	else
		tree->up = nullptr;
	tree->m = m;
	tree->down = std::vector<node*>();
	int moveOutcome = aigl.executeMove(m);
	if (moveOutcome >= 0)
		return tree;
	else
	{
		std::vector<move> moves = aigl.getMoves();
		for (move mv : moves)
			tree->down.push_back(makeTurnTree(mv, tree));
	}
	aigl.undo();
	return tree;
}