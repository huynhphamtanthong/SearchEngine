#include "trie.h"
#include "system.h"

#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <utility>
#include <algorithm>

using namespace std;

void Trie::build(string filename, unordered_map<string, int> data)
{
	if (data.size() == 0) return;
	//cerr << filename << " " << data.size() << "\n";
	TrieNode* root = this->root;
	for (auto it : data)
	{
		string key = it.first;
		if (root == nullptr)
			root = new TrieNode;
		TrieNode* tmproot = root;
		for (int i = 0; i < key.length(); ++i)
		{
			int tmp = get_index(key[i]);

			if (tmp == -1)
				continue;

			if (tmproot->child[tmp] == nullptr)
				tmproot->child[tmp] = new TrieNode;

			tmproot = tmproot->child[tmp];
		}
		tmproot->data[filename] += it.second;
	}

	if (!this->root) this->root = root;
}
// data of a file are keywords and their frequency

void Trie::save(string filename)
{
	ofstream out;
	out.open(get_link("cheatsheet", filename), ios::app);
	//out.open("output.txt", ios::app);

	queue<TrieNode*> que;
	que.push(root);

	int cnt = 0;
	while (!que.empty())
	{
		TrieNode* u = que.front();
		que.pop();

		if (u == nullptr)
		{
			out << "0\n";
			continue;
		}

		unordered_map <string, int> tmp;

		for (auto& it: u->data)
		{
			string tmpstr = "";
			for (int j = 0; j < it.first.length(); ++j)
				if (it.first[j] == ' ') tmpstr += '@';
				else tmpstr += it.first[j];
			tmp[tmpstr] = it.second;
			out << tmpstr << ' ' << it.second << ' ';
		}
		u->data.clear();
		u->data = tmp;
			
		out << "__END__ -1\n";

		for (int c = 0; c < 38; ++c)
			que.push(u->child[c]);
	}

	out.close();
}

void Trie::load(string filename)
{
	ifstream inp;
	inp.open(get_link("cheatsheet", filename));
	//inp.open("input.txt");
	if (root == nullptr)
		root = new TrieNode;

	//	std::cerr << "nom\n";

	string line;
	getline(inp, line);

	queue<TrieNode*> que;
	que.push(root);

	while (!inp.eof())
	{
		if (que.empty())
			break;
		TrieNode* u = que.front();
		que.pop();

		for (int c = 0; c < 38; ++c)
		{
			getline(inp, line);

			if (line.length() == 0)
				break;

			if (line == "0")
			{
				u->child[c] = nullptr;
				continue;
			}

			u->child[c] = new TrieNode;
			istringstream iss(line);

			do
			{
				string word, number;
				iss >> word >> number;

				if (word == "__END__")
					break;

				for (int j = 0; j < word.length(); ++j)
					if (word[j] == '@')
						word[j] = ' ';

				string file = word;
				int frequency = string_to_int(number);

				u->child[c]->data[string(file)] = frequency;
			} while (iss);

			que.push(u->child[c]);
		}
	}

	inp.close();
}

void Trie::__clear(TrieNode* node) {
	if (node == nullptr) return;
	for (int i = 0; i < 38; ++i)
		__clear(node->child[i]);
	delete node;
}

vector<pair<string, int>> Trie::search(string keyword) {
	if (!root)
	{
		return vector<pair<string, int>>();
	}

	TrieNode* tmp = root;
	for (int i = 0; i < keyword.length(); ++i)
	{
		if (!tmp) return vector<pair<string, int>>();
		tmp = tmp->child[get_index(keyword[i])];
	}

	return to_vector(tmp->data);
}

void Trie::clear() {
	__clear(root);
	root = nullptr;
}

bool Trie::isEmpty()
{
	return (!this->root);
}

void TrieNode::trieTraverse(TrieNode* head)
{
	if (!head) return;
	if (head->data.size() != 0)
	{
		for (auto it = head->data.begin(); it != head->data.end(); it++)
			cout << it->first << " " << it->second << endl;
	}
	for (int i = 0; i < 38; i++) this->trieTraverse(head->child[i]);
}

void Trie::trieTraverse()
{
	root->trieTraverse(root);
}
