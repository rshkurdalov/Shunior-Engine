#include "structured_file.h"
#include "external_data.h" //!!!

snode::snode()
{
	type = snode_type::string;
}

void utility<snode>::copy(const snode &source, snode *target)
{
	utility<string>().copy(source.name, &target->name);
	target->type = source.type;
	utility<string>().copy(source.value, &target->value);
	utility<array<snode>>().copy(source.elements, &target->elements);
}

void structured_file::load(string &filename)
{
	file storage;
	storage.filename << filename;
	storage.open();
	storage.position = 0;
	char8 *str = new char8[storage.size + 1];
	storage.read(storage.size, str);
	str[storage.size] = '\0';
	storage.close();
	string content;
	content << str;
	delete[] str;
	string_stream ss(content.addr, content.size);
	char32 ch;
	snode container;
	array<snode *> nodes;
	snode *node = &container;
	while(!ss.ended())
	{
		ss >> ch;
		if(ch == U'{')
		{
			if(ss.position + 1 < ss.size
				&& ss.addr[ss.position] == U'\\'
				&& ss.addr[ss.position + 1] == U'}')
			{
				ss.position += 2;
				node = nodes.back();
				if(nodes.size == 0) break;
				nodes.pop();
			}
			else
			{
				node->elements.push_default();
				nodes.push(node);
				node = &node->elements.back();
				while(!ss.ended())
				{
					ss >> ch;
					if(ch == U'}') break;
					else node->name << ch;
				}
				node->type = snode_type::array;
			}
		}
		else if(ch == U'<')
		{
			node->elements.push_default();
			while(!ss.ended())
			{
				ss >> ch;
				if(ch == U'>')
				{
					if(ss.addr[ss.position - 1] == '\\') node->elements.back().name.pop();
					else break;
				}
				else node->elements.back().name << ch;
			}
			node->elements.back().type = snode_type::string;
			while(!ss.ended())
			{
				ss >> ch;
				if(ch == U'"') break;
			}
			while(!ss.ended())
			{
				ss >> ch;
				if(ch == U'"')
				{
					if(ss.addr[ss.position - 1] == '\\') node->elements.back().name.pop();
					else break;
				}
				else node->elements.back().value << ch;
			}
		}
	}
	if(container.elements.size != 0)
		swap(&root, &container.elements.front());
}

void structured_file::clear()
{
	root.name.reset();
	root.value.reset();
	root.elements.reset();
}

struct sf_node_jump
{
	snode *node;
	uint32 include_level;

	sf_node_jump() {}
	sf_node_jump(snode *node, uint32 include_level)
		: node(node), include_level(include_level) {}
};

void structured_file::save(string &filename)
{
	file storage;
	storage.filename << filename;
	storage.open();
	array<sf_node_jump> nodes;
	nodes.push(sf_node_jump(&root, 0));
	uint32 idx;
	sf_node_jump last_node;
	string data;
	while(nodes.size != 0)
	{
		idx = nodes.size - 1;
		for(uint64 i = 0; i < nodes[idx].node->name.size; i++)
			if(nodes[idx].node->name[i] == U'>')
			{
				nodes[idx].node->name.insert(i, U'\\');
				i++;
			}
		for(uint64 i = 0; i < nodes[idx].node->value.size; i++)
			if(nodes[idx].node->value[i] == U'"')
			{
				nodes[idx].node->value.insert(i, U'\\');
				i++;
			}
		for(uint32 i = 0; i < nodes.addr[idx].include_level; i++)
			data << U"	";
		if(nodes.addr[idx].node == nullptr)
		{
			data << U"{\\}\n";
			nodes.pop();
		}
		else if(nodes.addr[idx].node->type == snode_type::string)
		{
			data << U"<" << nodes.addr[idx].node->name << U"> = \"" << nodes.addr[idx].node->value << U"\"\n";
			nodes.pop();
		}
		else
		{
			last_node = nodes.addr[idx];
			nodes.pop();
			data << U"{" << last_node.node->name << U"}\n";
			nodes.push(sf_node_jump(nullptr, last_node.include_level));
			for(uint32 i = last_node.node->elements.size; i != 0; i--)
				nodes.push(sf_node_jump(&last_node.node->elements.addr[i - 1], last_node.include_level + 1));
		}
	}
	storage.position = 0;
	storage.write(create_u8sz(data), data.size);
	storage.close();
}

