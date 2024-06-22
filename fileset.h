#pragma once
#include "file.h"

template<typename value_type> struct fileset_node
{
	value_type value;
	int64 upper;
	int64 left;
	int64 right;
	int32 diff;
};

template<typename value_type> struct fileset_iterator
{
	int64 idx;
	fileset_node<value_type> node;
};

template<typename value_type> struct fileset
{
	file storage;
	int64 root_idx;
	int64 free_idx;
	uint64 size;

	fileset()
	{
		storage.read_access = true;
		storage.write_access = true;
	}

	void create(string &filename)
	{
		storage.filename.reset();
		storage.filename << filename;
		storage.open();
		clear();
	}

	void open(string &filename)
	{
		storage.filename.reset();
		storage.filename << filename;
		storage.open();
		storage.read(8, &root_idx);
		storage.read(8, &free_idx);
		storage.read(8, &size);
	}

	void close()
	{
		storage.close();
	}

	fileset_iterator<value_type> iterator(int64 idx)
	{
		fileset_iterator<value_type> iter;
		iter.idx = idx;
		if(iter.idx == -1) return iter;
		storage.position = uint64(24 + idx * sizeof(fileset_node<value_type>));
		storage.read(sizeof(fileset_node<value_type>), &iter.node);
		return iter;
	}

	void save_node(fileset_iterator<value_type> iter)
	{
		if(iter.idx == -1) return;
		storage.position = uint64(24 + iter.idx * sizeof(fileset_node<value_type>));
		storage.write(&iter.node, sizeof(fileset_node<value_type>));
	}

	fileset_iterator<value_type> begin()
	{
		fileset_iterator<value_type> iter;
		if(root_idx == -1)
		{
			iter.idx = -1;
			return iter;
		}
		iter = iterator(root_idx);
		while(iter.node.left != -1)
			iter = iterator(iter.node.left);
		return iter;
	}

	fileset_iterator<value_type> end()
	{
		fileset_iterator<value_type> iter;
		if(root_idx == -1)
		{
			iter.idx = -1;
			return iter;
		}
		iter = iterator(root_idx);
		while(iter.node.right != -1)
			iter = iterator(iter.node.right);
		return iter;
	}

	fileset_iterator<value_type> next(fileset_iterator<value_type> iter)
	{
		if(iter.node.right != -1)
		{
			iter = iterator(iter.node.right);
			while(iter.node.left != -1)
				iter = iterator(iter.node.left);
			return iter;
		}
		else
		{
			while(iter.node.upper != -1 && iterator(iter.node.upper).node.right == iter.idx)
				iter = iterator(iter.node.upper);
			return iterator(iter.node.upper);
		}
	}

	fileset_iterator<value_type> prev(fileset_iterator<value_type> iter)
	{
		if(iter.node.left != -1)
		{
			iter = iterator(iter.node.left);
			while(iter.node.right != -1)
				iter = iterator(iter.node.right);
			return iter;
		}
		else
		{
			while(iter.node.upper != -1 && iterator(iter.node.upper).node.left == iter.idx)
				iter = iterator(iter.node.upper);
			return iterator(iter.node.upper);
		}
	}

	fileset_iterator<value_type> lower_bound(const key<value_type> &key_value)
	{
		fileset_iterator<value_type> iter;
		if(root_idx == -1)
		{
			iter.idx = -1;
			return iter;
		}
		iter = iterator(root_idx);
		while(true)
		{
			if(!(key<value_type>(iter.node.value) < key_value))
			{
				if(iter.node.left != -1)
					iter = iterator(iter.node.left);
				else break;
			}
			else
			{
				if(iter.node.right != -1)
					iter = iterator(iter.node.right);
				else break;
			}
		}
		while(iter.idx != -1 && key<value_type>(iter.node.value) < key_value)
			iter = next(iter);
		return iter;
	}

	fileset_iterator<value_type> upper_bound(const key<value_type> &key_value)
	{
		fileset_iterator<value_type> iter;
		if(root_idx == -1)
		{
			iter.idx = -1;
			return iter;
		}
		iter = iterator(root_idx);
		while(true)
		{
			if(!(key_value < key<value_type>(iter.node.value)))
			{
				if(iter.node.right != -1)
					iter = iterator(iter.node.right);
				else break;
			}
			else
			{
				if(iter.node.left != -1)
					iter = iterator(iter.node.left);
				else break;
			}
		}
		while(iter.idx != -1 && key_value < key<value_type>(iter.node.value))
			iter = prev(iter);
		return iter;
	}

	fileset_iterator<value_type> find(const key<value_type> &key_value)
	{
		fileset_iterator<value_type> iter = lower_bound(key_value);
		if(iter.idx == -1 || key_value < key<value_type>(iter.node.value))
			iter.idx = -1;
		return iter;
	}

	void rotate_left(fileset_iterator<value_type> &iter)
	{
		fileset_iterator<value_type> t = iterator(iter.node.right);
		iter.node.right = t.node.left;
		fileset_iterator<value_type> r = iterator(iter.node.right);
		if(r.idx != -1)
			r.node.upper = iter.idx;
		t.node.left = iter.idx;
		t.node.upper = iter.node.upper;
		iter.node.upper = t.idx;
		if(t.node.upper == -1)
		{
			root_idx = t.idx;
			storage.position = 0;
			storage.write(&root_idx, 8);
		}
		else
		{
			fileset_iterator<value_type> u = iterator(t.node.upper);
			if(u.node.left == iter.idx)
				u.node.left = t.idx;
			else u.node.right = t.idx;
			save_node(u);
		}
		save_node(t);
		save_node(iter);
		save_node(r);
	}

	void rotate_right(fileset_iterator<value_type> &iter)
	{
		fileset_iterator<value_type> t = iterator(iter.node.left);
		iter.node.left = t.node.right;
		fileset_iterator<value_type> l = iterator(iter.node.left);
		if(l.idx != -1)
			l.node.upper = iter.idx;
		t.node.right = iter.idx;
		t.node.upper = iter.node.upper;
		iter.node.upper = t.idx;
		if(t.node.upper == -1)
		{
			root_idx = t.idx;
			storage.position = 0;
			storage.write(&root_idx, 8);
		}
		else
		{
			fileset_iterator<value_type> u = iterator(t.node.upper);
			if(u.node.left == iter.idx)
				u.node.left = t.idx;
			else u.node.right = t.idx;
			save_node(u);
		}
		save_node(t);
		save_node(iter);
		save_node(l);
	}

	void rebalance(fileset_iterator<value_type> &iter)
	{
		if(iter.node.diff == -2)
		{
			fileset_iterator<value_type> r = iterator(iter.node.right);
			if(r.node.diff != 1)
			{
				if(r.node.diff == -1)
				{
					iter.node.diff = 0;
					r.node.diff = 0;
				}
				else
				{
					iter.node.diff = -1;
					r.node.diff = 1;
				}
				save_node(iter);
				save_node(r);
				rotate_left(iter);
			}
			else
			{
				fileset_iterator<value_type> rl = iterator(r.node.left);
				if(rl.node.diff == -1)
				{
					iter.node.diff = 1;
					r.node.diff = 0;
				}
				else if(rl.node.diff == 0)
				{
					iter.node.diff = 0;
					r.node.diff = 0;
				}
				else
				{
					iter.node.diff = 0;
					r.node.diff = -1;
				}
				rl.node.diff = 0;
				save_node(iter);
				save_node(r);
				save_node(rl);
				rotate_right(r);
				iter = iterator(iter.idx);
				rotate_left(iter);
			}
		}
		else
		{
			fileset_iterator<value_type> l = iterator(iter.node.left);
			if(l.node.diff != -1)
			{
				if(l.node.diff == 1)
				{
					iter.node.diff = 0;
					l.node.diff = 0;
				}
				else
				{
					iter.node.diff = 1;
					l.node.diff = -1;
				}
				save_node(iter);
				save_node(l);
				rotate_right(iter);
			}
			else
			{
				fileset_iterator<value_type> lr = iterator(l.node.right);
				if(lr.node.diff == 1)
				{
					iter.node.diff = -1;
					l.node.diff = 0;
				}
				else if(lr.node.diff == 0)
				{
					iter.node.diff = 0;
					l.node.diff = 0;
				}
				else
				{
					iter.node.diff = 0;
					l.node.diff = 1;
				}
				lr.node.diff = 0;
				save_node(iter);
				save_node(l);
				save_node(lr);
				rotate_left(l);
				iter = iterator(iter.idx);
				rotate_right(iter);
			}
		}
	}

	void insert(const value_type &value)
	{
		fileset_iterator<value_type> node, prev_node, new_node;
		if(free_idx == -1)
		{
			new_node.idx = (storage.size - 24) / sizeof(fileset_node<value_type>);
			storage.resize(storage.size + sizeof(fileset_node<value_type>));
		}
		else
		{
			new_node.idx = free_idx;
			node = iterator(free_idx);
			free_idx = node.node.left;
			storage.position = 8;
			storage.write(&free_idx, 8);
		}
		new_node.node.value = value;
		new_node.node.left = -1;
		new_node.node.right = -1;
		new_node.node.diff = 0;
		size++;
		storage.position = 16;
		storage.write(&size, 8);
		if(root_idx == -1)
		{
			root_idx = new_node.idx;
			storage.position = 0;
			storage.write(&root_idx, 8);
			new_node.node.upper = -1;
		}
		else
		{
			key<value_type> key_value(value);
			node = iterator(root_idx);
			while(true)
			{
				if(key_value < key<value_type>(node.node.value))
				{
					if(node.node.left != -1)
						node = iterator(node.node.left);
					else
					{
						node.node.left = new_node.idx;
						save_node(node);
						break;
					}
				}
				else
				{
					if(node.node.right != -1)
						node = iterator(node.node.right);
					else
					{
						node.node.right = new_node.idx;
						save_node(node);
						break;
					}
				}
			}
			new_node.node.upper = node.idx;
		}
		save_node(new_node);
		node = new_node;
		while(true)
		{
			prev_node = node;
			node = iterator(node.node.upper);
			if(node.idx == -1) break;
			if(node.node.left == prev_node.idx)
				node.node.diff++;
			else node.node.diff--;
			save_node(node);
			if(node.node.diff == 0) break;
			else if(node.node.diff == 2 || node.node.diff == -2)
			{
				rebalance(node);
				node = iterator(node.node.upper);
				if(node.node.diff == 0) break;
			}
		}
	}

	void remove(const key<value_type> &key_value)
	{
		fileset_iterator<value_type> node, swap_node, prev_node;
		while(true)
		{
			node = find(key_value);
			if(node.idx == -1) break;
			size--;
			storage.position = 16;
			storage.write(&size, 8);
			if(node.node.left != -1 || node.node.right != -1)
			{
				swap_node = prev(node);
				if(swap_node.idx == -1)
				{
					fileset_iterator<value_type> r = iterator(node.node.right);
					static_copy<sizeof(value_type)>(&r.node.value, &node.node.value);
					save_node(node);
					node = r;
				}
				else
				{
					static_copy<sizeof(value_type)>(&swap_node.node.value, &node.node.value);
					save_node(node);
					if(swap_node.node.left != -1)
					{
						fileset_iterator<value_type> l = iterator(swap_node.node.left);
						static_copy<sizeof(value_type)>(&l.node.value, &swap_node.node.value);
						save_node(swap_node);
						node = l;
					}
					else node = swap_node;
				}
			}
			if(node.node.upper == -1)
			{
				clear();
				break;
			}
			else
			{
				fileset_iterator<value_type> u = iterator(node.node.upper);
				if(u.node.right == node.idx)
				{
					u.node.right = -1;
					u.node.diff++;
				}
				else
				{
					u.node.left = -1;
					u.node.diff--;
				}
				save_node(u);
				swap_node = u;
				node.node.left = free_idx;
				save_node(node);
				free_idx = node.idx;
				storage.position = 8;
				storage.write(&free_idx, 8);
				node = swap_node;
			}
			while(true)
			{
				prev_node = node;
				node = iterator(node.node.upper);
				if(node.idx == -1) break;
				if(node.node.left == prev_node.idx)
					node.node.diff--;
				else node.node.diff++;
				save_node(node);
				if(node.node.diff == 1 || node.node.diff == -1) break;
				else if(node.node.diff == 2 || node.node.diff == -2)
				{
					rebalance(node);
					node = iterator(node.node.upper);
					if(node.node.diff != 0) break;
				}
			}
		}
	}

	void clear()
	{
		storage.resize(24);
		storage.position = 0;
		root_idx = -1;
		storage.write(&root_idx, 8);
		free_idx = -1;
		storage.write(&free_idx, 8);
		size = 0;
		storage.write(&size, 8);
	}
};
