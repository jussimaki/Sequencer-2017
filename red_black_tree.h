#ifndef RED_BLACK_TREE
#define RED_BLACK_TREE


template <class dataType>
struct RedBlackNode
{
	dataType data;
	long value;

	bool black;
	RedBlackNode <dataType> *father;
	RedBlackNode <dataType> *leftChild, *rightChild;


};

template <class dataType>
class RedBlackTree
{
public:
	RedBlackTree() : root_(0) {}

	RedBlackNode <dataType> *getRoot()
	{
		return root_;
	}
	
	RedBlackNode <dataType> *find(long value)
	{
		RedBlackNode <dataType> *pointer = root_;

		while (pointer)
		{
			if (pointer->value == value)
			{

				return pointer;
			}

			else if (pointer->value > value) pointer = pointer->leftChild;
			else pointer = pointer->rightChild;

		}

		return 0;
	}



	RedBlackNode <dataType> *getUncle(RedBlackNode <dataType> *node)
	{
		RedBlackNode <dataType> *father = node->father;

		if (father)
		{
			if (father->father)
			{
				if (father->father->leftChild == father)
					return father->father->rightChild;
				else return father->father->leftChild;
			}
		}

		return 0;
	}


	bool BSTInsert(RedBlackNode <dataType> **root, RedBlackNode <dataType> *node, RedBlackNode <dataType> *father = 0)
	{
		if (*root == 0) { *root = node; node->father = father; return true; }

		if (node->value < (*root)->value)
			return BSTInsert(&((*root)->leftChild), node, *root);
		else if (node->value >(*root)->value)
			return BSTInsert(&((*root)->rightChild), node, *root);

		return false;
	}


	RedBlackNode <dataType> *minValueNode(RedBlackNode <dataType> *node)
	{
		RedBlackNode <dataType> *current = node;

		while (current->leftChild)
			current = current->leftChild;

		return current;
	}

	void replaceNodeInParent(RedBlackNode <dataType> *node, RedBlackNode <dataType> *newValue)
	{
		if (node->father)
		{
			if (node == node->father->leftChild)
			{
				node->father->leftChild = newValue;
			}
			else
			{
				node->father->rightChild = newValue;
			}
		}
		else
		{
			root_ = newValue;
		}

		if (newValue)
		{
			newValue->father = node->father;
		}
	}

	// takes pointer to the node that holds the value to be removed.
	// returns the node that is actually removed.
	RedBlackNode <dataType> *BSTRemove(RedBlackNode <dataType> *node, RedBlackNode <dataType> **replacer)
	{
		if (!node) return 0;


		// two children
		if (node->leftChild && node->rightChild)
		{
			// find the right minimum
			RedBlackNode <dataType> *successor = minValueNode(node->rightChild);

			dataType tempData = node->data;
			long tempValue = node->value;

			node->data = successor->data;
			node->value = successor->value;


			successor->data = tempData;
			successor->value = tempValue;

			return BSTRemove(successor, replacer);
		}
		else if (node->leftChild) // only left child
		{
			replaceNodeInParent(node, node->leftChild);
			*replacer = node->leftChild;

			return node;
		}
		else if (node->rightChild) // only right child
		{
			replaceNodeInParent(node, node->rightChild);
			*replacer = node->rightChild;

			return node;
		}
		else // no children 
		{
			replaceNodeInParent(node, 0);
			*replacer = 0;

			return node;
		}
	}

	void leftRotate(RedBlackNode <dataType> *node, RedBlackNode <dataType> **root)
	{
		RedBlackNode <dataType> *rightChild = node->rightChild;

		node->rightChild = rightChild->leftChild;

		if (node->rightChild != 0)
			node->rightChild->father = node;

		rightChild->father = node->father;

		if (node->father == 0)
			*root = rightChild;

		else if (node == node->father->leftChild)
			node->father->leftChild = rightChild;
		else
			node->father->rightChild = rightChild;

		rightChild->leftChild = node;
		node->father = rightChild;
	}


	void rightRotate(RedBlackNode <dataType> *node, RedBlackNode <dataType> **root)
	{
		RedBlackNode <dataType> *leftChild = node->leftChild;

		node->leftChild = leftChild->rightChild;

		if (node->leftChild != 0)
			node->leftChild->father = node;

		leftChild->father = node->father;

		if (node->father == 0)
			*root = leftChild;

		else if (node == node->father->leftChild)
			node->father->leftChild = leftChild;
		else
			node->father->rightChild = leftChild;

		leftChild->rightChild = node;
		node->father = leftChild;
	}

	long countBlackHeight(RedBlackNode <dataType> *node, long height = 0)
	{
		if (node->black) {
			height++;
			
			//std::cout << node->value << "BLACK, ";
		}
		else
		{
			//std::cout << node->value << "RED, ";
		}

		if (node->father) return countBlackHeight(node->father, height);

		else return height;
	}

	bool doubleRedError(RedBlackNode <dataType> *node, RedBlackNode <dataType> **errorNode)
	{
		if (!node->black && node->father && !node->father->black) {
			*errorNode = node; return true;
		}

		if (node->leftChild)
			if (doubleRedError(node->leftChild, errorNode)) return true;

		if (node->rightChild)
			if (doubleRedError(node->rightChild, errorNode)) return true;

		return false;
	}

	void swapColors(RedBlackNode <dataType> *node1, RedBlackNode <dataType> *node2)
	{
		bool tempBlack = node1->black;
		node1->black = node2->black;
		node2->black = tempBlack;
	}

	RedBlackNode <dataType> *getSibling(RedBlackNode <dataType> *node, RedBlackNode <dataType> *father)
	{
		if (!father) return 0;

		if (node == father->leftChild) return father->rightChild;
		else if (node == father->rightChild) return father->leftChild;
		else return 0;
	}

	// takes pointer to the node that holds the value to be removed.
	// returns the node that is actually removed.
	RedBlackNode <dataType> *remove(RedBlackNode <dataType> *node)
	{
		RedBlackNode <dataType> *origRoot = root_;
		RedBlackNode <dataType> *replacer = 0;
		RedBlackNode <dataType> *removedNode = BSTRemove(node, &replacer);

		if (!removedNode) return 0;


		// if either the removed node or replacing node is red

		bool eitherRed = false;

		if (!removedNode->black) eitherRed = true;

		if (replacer)
		{
			if (!replacer->black) eitherRed = true;

			// replacing node is going to be at least single black anyway
			replacer->black = true;
		}



		// if either removed or replacing node is red just color the replacing node black (as what happened above) and return
		if (eitherRed) return removedNode;


		// if both removed and replaced nodes were black
		bool doubleBlack = true;

		RedBlackNode <dataType> *vNodesFather = removedNode->father;
		RedBlackNode <dataType> *uNode = replacer;

		int iteration = 0;
		while (doubleBlack && (uNode != root_)) // case 1
		{
			iteration++;
			RedBlackNode <dataType> *sibling = 0;

			RedBlackNode <dataType> *currentsFather = 0;



			if (vNodesFather) currentsFather = vNodesFather;
			else currentsFather = uNode->father;

			sibling = getSibling(uNode, currentsFather);
			
			//if (sibling && sibling->father != vNodesFather) std::cout << "SOMETHING WRONG!" << std::endl;

			bool siblingRed = false;

			bool siblingsChildIsRed = false;
			bool leftChildIsRed = false;
			bool rightChildIsRed = false;

			RedBlackNode <dataType> *redChild = 0;

			/*
			if (!currentsFather->black)
			{
				if (sibling)
				{
					sibling->black = !sibling->black;
				}

				currentsFather->black = true;

				break;
			}

			*/


			if (sibling)
			{
				if (!sibling->black) siblingRed = true;

				// case 2
				if (siblingRed) // there must be a non-null sibling if it's red
				{
					// convert to black sibling case
					RedBlackNode <dataType> *siblingsFather = sibling->father;

					if ((sibling == siblingsFather->leftChild))
						rightRotate(siblingsFather, &root_);
					else
						leftRotate(siblingsFather, &root_);

					swapColors(sibling, siblingsFather);

					siblingRed = false;


					sibling = getSibling(uNode, currentsFather);
				}

				// sibling is now black, do some tests
				if (sibling)
				{
					if (sibling->leftChild)
					{
						if (!sibling->leftChild->black)
						{
							siblingsChildIsRed = true;
							leftChildIsRed = true;
						}
					}

					if (sibling->rightChild)
					{
						if (!sibling->rightChild->black)
						{
							siblingsChildIsRed = true;
							rightChildIsRed = true;
						}
					}

				}

			}
			
			// black sibling with at least one red child
			if (siblingsChildIsRed) // father has to be black if silbling is red!!
			{
				if ((sibling == sibling->father->leftChild))
				{
					// LEFT GENERAL CASE (sibling is left child)
					
					if (!leftChildIsRed && rightChildIsRed) // case 5, left right case
					{
						redChild = sibling->rightChild;

						//leftRotate(redChild, &root_);
						leftRotate(sibling, &root_);

						swapColors(redChild, sibling);

					}

					// case 6

					sibling = getSibling(uNode, currentsFather);

					sibling->black = currentsFather->black;
					currentsFather->black = true;


					// left left case

					sibling->leftChild->black = true;
					//rightRotate(sibling, &root_);
					rightRotate(currentsFather, &root_);

					
				}
				else
				{

					// RIGHT GENERAL CASE (sibling is right child)

					if (!rightChildIsRed && leftChildIsRed) // case 5, right left case
					{
						redChild = sibling->leftChild;

						//rightRotate(redChild, &root_);
						rightRotate(sibling, &root_);

						swapColors(redChild, sibling);
					}

					// case 6

					sibling = getSibling(uNode, currentsFather);

					sibling->black = currentsFather->black;
					currentsFather->black = true;


					// right right case

					sibling->rightChild->black = true;
					//leftRotate(sibling, &root_);
					leftRotate(currentsFather, &root_);

					
				}

				doubleBlack = false;
				
			}
			else // black sibling and its both children are black
			{
				// case 3 or case 4
				
					// perform recoloring
					if(sibling) sibling->black = false;

					if (uNode)
					{
						if (uNode->father)
						{
							if (!uNode->father->black) // case 4
							{
								uNode->father->black = true;

								doubleBlack = false;
							}

							uNode = uNode->father;
						}
					}
					else
					{
						if (vNodesFather)
						{
							if (!vNodesFather->black) // case 4
							{
								vNodesFather->black = true;

								doubleBlack = false;
							}

							uNode = vNodesFather;
							vNodesFather = 0;
						}
					}
				}
			
		}


		if (uNode == root_)
		{
			if(uNode) uNode->black = true;
		}

		if (root_) root_->black = true;

		return removedNode;
	}

	bool insert(RedBlackNode <dataType> *node)
	{

		// insert a red node
		node->black = false;
		node->father = 0;
		node->leftChild = 0;
		node->rightChild = 0;

		bool value = BSTInsert(&root_, node);

		assert(value);
		if (!value) return false;

		// while the node and its father are red (can't have two consecutive red nodes in a balanced red black tree)
		while ((node != root_) && (!node->black) && (!node->father->black))
		{

			RedBlackNode <dataType> *father = node->father;
			RedBlackNode <dataType> *grandFather = father->father;

			// node's father is its father's left child, so uncle is grandfathers right child
			if (father == grandFather->leftChild)
			{
				RedBlackNode <dataType> *uncle = grandFather->rightChild;

				if (uncle && !uncle->black) // red uncle, "promotion"
				{
					grandFather->black = false;
					father->black = true;
					uncle->black = true;

					node = grandFather;
				}
				else
				{
					// Left right case (father is left child, node is right child)
					if (node == father->rightChild)
					{
						leftRotate(father, &root_);
						node = father;
						father = node->father;
					}

					// Left left case
					rightRotate(grandFather, &root_);
					swapColors(father, grandFather);

					node = father;
				}
			}
			else // node's father is its father's right child, so uncle is grandfathers left child
			{
				RedBlackNode <dataType> *uncle = grandFather->leftChild;

				if (uncle && !uncle->black) // red uncle, "promotion"
				{
					grandFather->black = false;
					father->black = true;
					uncle->black = true;

					node = grandFather;
				}
				else
				{
					// Right left case
					if (node == father->leftChild)
					{
						rightRotate(father, &root_);
						node = father;
						father = node->father;
					}

					// Right right case
					leftRotate(grandFather, &root_);
					swapColors(father, grandFather);
					node = father;
				}
			}
		}


		root_->black = true;

		return true;
	}

private:

	RedBlackNode <dataType> *root_;
};


#endif
