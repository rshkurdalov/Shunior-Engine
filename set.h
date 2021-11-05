#pragma once
#include "global_types.h"
#include "global_operators.h"

template<typename value_type> struct set_node
{
	value_type value;
	set_node *upper;
	set_node *left;
	set_node *right;
	int32 diff;
};

template<typename value_type>
void clear_node(set_node<value_type> *node)
{
	if(node->left != nullptr)
		clear_node(node->left);
	if(node->right != nullptr)
		clear_node(node->right);
	delete node;
}

template<typename value_type> struct set
{
	set_node<value_type> *root;
	uint64 size;

	set()
	{
		root = nullptr;
		size = 0;
	}

	~set()
	{
		clear();
	}

	set_node<value_type> *begin()
	{
		if(root == nullptr) return nullptr;
		set_node<value_type> *node = root;
		while(node->left != nullptr)
			node = node->left;
		return node;
	}

	set_node<value_type> *end()
	{
		if(root == nullptr) return nullptr;
		set_node<value_type> *node = root;
		while(node->right != nullptr)
			node = node->right;
		return node;
	}

	set_node<value_type> *next(set_node<value_type> *node)
	{
		if(node->right != nullptr)
		{
			node = node->right;
			while(node->left != nullptr)
				node = node->left;
			return node;
		}
		else
		{
			while(node->upper != nullptr && node->upper->right == node)
				node = node->upper;
			return node->upper;
		}
	}

	set_node<value_type> *prev(set_node<value_type> *node)
	{
		if(node->left != nullptr)
		{
			node = node->left;
			while(node->right != nullptr)
				node = node->right;
			return node;
		}
		else
		{
			while(node->upper != nullptr && node->upper->left == node)
				node = node->upper;
			return node->upper;
		}
	}

	set_node<value_type> *lower_bound(const key<value_type> &key_value)
	{
		if(root == nullptr) return nullptr;
		set_node<value_type> *node = root;
		while(true)
		{
			if(!(key<value_type>(node->value) < key_value))
			{
				if(node->left != nullptr)
					node = node->left;
				else break;
			}
			else
			{
				if(node->right != nullptr)
					node = node->right;
				else break;
			}
		}
		while(node != nullptr && key<value_type>(node->value) < key_value)
			node = next(node);
		return node;
	}

	set_node<value_type> *upper_bound(const key<value_type> &key_value)
	{
		if(root == nullptr) return nullptr;
		set_node<value_type> *node = root;
		while(true)
		{
			if(!(key_value < key<value_type>(node->value)))
			{
				if(node->right != nullptr)
					node = node->right;
				else break;
			}
			else
			{
				if(node->left != nullptr)
					node = node->left;
				else break;
			}
		}
		while(node != nullptr && key_value < key<value_type>(node->value))
			node = prev(node);
		return node;
	}

	set_node<value_type> *find(const key<value_type> &key_value)
	{
		set_node<value_type> *node = lower_bound(key_value);
		if(node == nullptr || key_value < key<value_type>(node->value))
			return nullptr;
		else return node;
	}

	void rotate_left(set_node<value_type> *node)
	{
		set_node<value_type> *t = node->right;
		node->right = node->right->left;
		if(node->right != nullptr)
			node->right->upper = node;
		t->left = node;
		t->upper = node->upper;
		node->upper = t;
		if(t->upper == nullptr) root = t;
		else
		{
			if(t->upper->left == node)
				t->upper->left = t;
			else t->upper->right = t;
		}
	}

	void rotate_right(set_node<value_type> *node)
	{
		set_node<value_type> *t = node->left;
		node->left = node->left->right;
		if(node->left != nullptr)
			node->left->upper = node;
		t->right = node;
		t->upper = node->upper;
		node->upper = t;
		if(t->upper == nullptr) root = t;
		else
		{
			if(t->upper->left == node)
				t->upper->left = t;
			else t->upper->right = t;
		}
	}

	void rebalance(set_node<value_type> *node)
	{
		if(node->diff == -2)
		{
			if(node->right->diff != 1)
			{
				if(node->right->diff == -1)
				{
					node->diff = 0;
					node->right->diff = 0;
				}
				else
				{
					node->diff = -1;
					node->right->diff = 1;
				}
				rotate_left(node);
			}
			else
			{
				if(node->right->left->diff == -1)
				{
					node->diff = 1;
					node->right->diff = 0;
				}
				else if(node->right->left->diff == 0)
				{
					node->diff = 0;
					node->right->diff = 0;
				}
				else
				{
					node->diff = 0;
					node->right->diff = -1;
				}
				node->right->left->diff = 0;
				rotate_right(node->right);
				rotate_left(node);
			}
		}
		else
		{
			if(node->left->diff != -1)
			{
				if(node->left->diff == 1)
				{
					node->diff = 0;
					node->left->diff = 0;
				}
				else
				{
					node->diff = 1;
					node->left->diff = -1;
				}
				rotate_right(node);
			}
			else
			{
				if(node->left->right->diff == 1)
				{
					node->diff = -1;
					node->left->diff = 0;
				}
				else if(node->left->right->diff == 0)
				{
					node->diff = 0;
					node->left->diff = 0;
				}
				else
				{
					node->diff = 0;
					node->left->diff = 1;
				}
				node->left->right->diff = 0;
				rotate_left(node->left);
				rotate_right(node);
			}
		}
	}

	void insert(const value_type &value)
	{
		set_node<value_type> *node, *prev_node, *new_node = new set_node<value_type>();
		new_node->value = value;
		new_node->left = nullptr;
		new_node->right = nullptr;
		new_node->diff = 0;
		size++;
		if(root == nullptr)
		{
			root = new_node;
			root->upper = nullptr;
		}
		else
		{
			key<value_type> key_value(value);
			node = root;
			while(true)
			{
				if(key_value < key<value_type>(node->value))
				{
					if(node->left != nullptr)
						node = node->left;
					else
					{
						node->left = new_node;
						break;
					}
				}
				else
				{
					if(node->right != nullptr)
						node = node->right;
					else
					{
						node->right = new_node;
						break;
					}
				}
			}
			new_node->upper = node;
		}
		node = new_node;
		while(true)
		{
			prev_node = node;
			node = node->upper;
			if(node == nullptr) break;
			if(node->left == prev_node)
				node->diff++;
			else node->diff--;
			if(node->diff == 0) break;
			else if(node->diff == 2 || node->diff == -2)
			{
				rebalance(node);
				node = node->upper;
				if(node->diff == 0) break;
			}
		}
	}

	void remove(const key<value_type> &key_value)
	{
		set_node<value_type> *node, *swap_node, *prev_node;
		while(true)
		{
			node = find(key_value);
			if(node == nullptr) break;
			size--;
			if(node->left != nullptr || node->right != nullptr)
			{
				swap_node = prev(node);
				if(swap_node == nullptr)
				{
					static_copy<sizeof(value_type)>(&node->right->value, &node->value);
					node = node->right;
				}
				else
				{
					static_copy<sizeof(value_type)>(&swap_node->value, &node->value);
					if(swap_node->left != nullptr)
					{
						static_copy<sizeof(value_type)>(&swap_node->left->value, &swap_node->value);
						node = swap_node->left;
					}
					else node = swap_node;
				}
			}
			if(node->upper == nullptr)
			{
				delete node;
				root = nullptr;
				break;
			}
			else
			{
				if(node->upper->right == node)
				{
					node->upper->right = nullptr;
					node->upper->diff++;
				}
				else
				{
					node->upper->left = nullptr;
					node->upper->diff--;
				}
				swap_node = node->upper;
				delete node;
				node = swap_node;
			}
			while(true)
			{
				prev_node = node;
				node = node->upper;
				if(node == nullptr) break;
				if(node->left == prev_node)
					node->diff--;
				else node->diff++;
				if(node->diff == 1 || node->diff == -1) break;
				else if(node->diff == 2 || node->diff == -2)
				{
					rebalance(node);
					node = node->upper;
					if(node->diff != 0) break;
				}
			}
		}
	}

	void clear()
	{
		if(root != nullptr)
			clear_node(root);
		root = nullptr;
		size = 0;
	}
};
