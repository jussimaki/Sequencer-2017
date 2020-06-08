#ifndef POOL_H
#define POOL_H

#include <iostream>


template <typename dataType>
class Node
{
	

public:

	~Node()
	{
		//std::cout << "delete node " << std::endl;
	}
	Node *next, *last;
	dataType data;
};


template <typename dataType>
class Pool
{
public:
	Pool() : begin_(0), last_(0) {}
	bool empty();
	void free();
	void clear();
	void addToBegin(Node <dataType> *node);
	void addToEnd(Node <dataType> *node);

	void emptyToAnotherPool(Pool <dataType> *pool);
	Node <dataType> *extractFirstNode();
	Node <dataType> *extractLastNode();
	void extractNode(Node <dataType> * node);
	Node <dataType> *getFirstNode();
	Node <dataType> *getNextNode(Node <dataType> *node);
	Node <dataType> **getBeginAddress() { return &begin_; }
private:
	Node <dataType> *begin_;
	Node <dataType> *last_;
};


template <typename dataType>
void Pool<dataType>::emptyToAnotherPool(Pool <dataType> *pool)
{
	while (!empty())
	{
		Node <dataType> *node = extractFirstNode();
		//node->data = 0;
		node->last = 0;
		node->next = 0;

		pool->addToBegin(node);
	}
}

template <typename dataType>
void Pool<dataType>::free()
{
	Node <dataType> *node;

	node = begin_;

	while (node)
	{

		Node <dataType> *next = node->next;

		if (node->data) delete node->data;
	
		delete node;
		node = next;
	}


	begin_ = 0;
	last_ = 0;
}

template <typename dataType>
void Pool<dataType>::clear()
{
	Node <dataType> *node;

	node = begin_;

	while (node)
	{
		Node <dataType> *next = node->next;

		delete node;
		node = next;
	}

	begin_ = 0;
	last_ = 0;
}

template <typename dataType>
bool Pool<dataType>::empty()
{
	return !begin_;
}

template <typename dataType>
Node <dataType> *Pool<dataType>::extractFirstNode()
{
	if (!begin_) return 0;

	Node <dataType> *first = begin_;
	begin_ = first->next;

	first->next = 0;
	first->last = 0;

	if (first == last_) last_ = 0;

	return first;
}


template <typename dataType>
Node <dataType> *Pool<dataType>::extractLastNode()
{
	if (!last_) return 0;

	Node <dataType> *last = last_;
	last_ = last->last;

	last->next = 0;
	last->last = 0;

	if (last == begin_) begin_ = 0;

	return last;
}

template <typename dataType>
void Pool<dataType>::extractNode(Node <dataType> * node)
{
	if (!node) return;

	if (begin_ == node)
	{
		begin_ = node->next;
	}
	
	if (last_ == node)
	{
		last_ = node->last;
	}


	if (node->last)
	{
		node->last->next = node->next;
	}

	if (node->next)
	{
		node->next->last = node->last;
	}

	
}


template <typename dataType>
Node <dataType> *Pool<dataType>::getFirstNode()
{
	return begin_;
}

template <typename dataType>
Node<dataType> *Pool<dataType>::getNextNode(Node <dataType> *node)
{
	if (!node) return 0;

	return node->next;
}


template <typename dataType>
void Pool<dataType>::addToBegin(Node <dataType> *node)
{
	if (!node) return;

	node->next = begin_;
	node->last = 0;

	if (begin_)
	{
		begin_->last = node;
	}

	begin_ = node;

	if (!last_)
	{
		last_ = node;
	}
}

template <typename dataType>
void Pool<dataType>::addToEnd(Node <dataType> *node)
{
	if (!node) return;

	node->next = 0;
	node->last = last_;

	if (last_)
	{
		last_->next = node;
	}

	last_ = node;

	if (!begin_)
	{
		begin_ = node;
	}

}


#endif