#include "search.h"

using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>

#include "trie.h"
#include "system.h"
#include "query_handling.h"
#include "file_handling.h"

void firstSearch::exit()
{
	//INTERFACE WHEN NULL INTERSECTION
	cout << "There are no match results !!" << endl;
	return;
}

void firstSearch::intersection(vector<pair<string, int>> f2)
{
	if (f2.size() == 0)
	{
		delete this->container;
		this->container = nullptr;
		return;
	}
	//IF CONTAINER IS EMPTY -> INITILIZE IT FIRST
	if (!(this->container != nullptr && this->container->size() != 0))
	{
		if (!this->container) this->container = new unordered_map<string, int>;
		for (auto it = f2.begin(); it != f2.end(); it++)
		{
			(*this->container)[it->first] = it->second;
		}
		return;
	}
	//HAVE NOT OPTIMIZED YET
	unordered_map<string, int>* newContainer = new unordered_map<string, int>;
	for (auto it = f2.begin(); it != f2.end(); it++)
	{
		auto temp = this->container->find(it->first);
		if (temp == this->container->end()) continue;
		else (*newContainer)[it->first] = min(temp->second, it->second);
	}

	delete this->container;
	this->container = newContainer;
	return;
}

vector<pair<string, int>> firstSearch::intersection(vector<pair<string, int>> f1, vector<pair<string, int>> f2)
{
	sort(f2.begin(), f2.end());
	vector<pair<string, int>> result;
	for (int i = 0; i < f1.size(); ++i)
	{
		int j = lower_bound(f2.begin(), f2.end(), make_pair(f1[i].first, 0)) - f2.begin();
		if (j == f2.size() || f1[i].first != f2[j].first)
			continue;
		result.push_back(make_pair(f1[i].first, min(f1[i].second, f2[j].second)));
	}
	return result;
}

void firstSearch::unionSet(vector<pair<string, int>> f1)
{
	for (auto it : f1)
	{
		auto tmp = this->container->find(it.first);
		if (tmp == this->container->end()) (*this->container)[it.first] = it.second + tmp->second;
	}
	return;
}


vector<pair<string, int>> firstSearch::unionSet(vector<pair<string, int>> f1, vector<pair<string, int >> f2) 
{
	sort(f1.begin(), f1.end());
	sort(f2.begin(), f2.end());
	vector <pair <string, int>> ret;
	for (int i = 0, j = 0; i < f1.size() || j < f2.size();) {
		if (i < f1.size() && (j == f2.size() || f1[i].first < f2[j].first))
			ret.push_back(f1[i++]);
		else if (i == f1.size() || f1[i].first > f2[j].first)
			ret.push_back(f2[j++]);
		else ret.push_back({ f1[i].first, f1[i].second + f2[j].second }), ++i, ++j;
	}
	return ret;
}


//----------------------------------------------------SEARCH--------------------------------------------------//
vector<pair<string, int>> firstSearch::search(string ss, Trie T)
{
	if (!T.isEmpty()) return T.search(ss);
	else vector<pair<string, int>>();
}

vector<pair<string, int>> firstSearch::search(string ss)
{
	if (!T.isEmpty()) return this->search(ss, this->T);
	else return vector<pair<string, int>>();
}

//SS STAND FOR SEARCH_STRING
//OPERATORS
//------------------------------------------------QUOTES----------------------------------------------------//

vector<pair<string, int>> firstSearch::quote(string quotes) {
	const int lim = 5;

	string tmp = quotes;
	QueryHandling qh;
	qh.filter(tmp);

	vector <pair <string, int>> cur;
	int cnt = 0;
	string word = "";
	for (int i = 0; i < tmp.length(); ++i)
		if (tmp[i] == ' ' || tmp[i] == '*') {
			if (word == "") continue;
			if (!cnt) cur = this->search(word);
			else cur = intersection(cur, this->search(word));
			++cnt; word = "";
			if (cnt == lim) break;
		}

	vector <pair <string, int>> ret;

	ifstream fi;
	for (auto file : cur) {
		fi.open(file.first);
		int checkpoint = 0;
		int score = 0;
		int cur = 0;
		for (char c; !fi.eof();) {
			c = fi.get();
			if (c == quotes[cur]) {
				++cur;
				if (cur < quotes.length() && quotes[cur] == '*')
					checkpoint = ++cur;
				if (cur == quotes.length())
					++score, checkpoint = cur = 0;
			}
			else cur = checkpoint;
		}
		if (score) ret.push_back({ file.first, score });
		fi.close();
	}
	return ret;
}

//------------------------------------------------ORIGIN----------------------------------------------------//
vector<pair<string, int>> firstSearch::origin(vector<string> origins)
{
	if (!origins.size()) return vector<pair<string, int>>();
	vector<pair<string, int>> result;
	for (auto it = origins.begin(); it != origins.end(); it++)
	{
		if (result.empty())
		{
			result = this->search((*it), this->T);
			continue;
		}
		vector<pair<string, int>> temp;
		temp = this->search((*it), this->T);
		result = this->intersection(result, temp);
	}
	return result;
}

//------------------------------------------------OR----------------------------------------------------//
vector<pair<string, int>> firstSearch::findOr(vector<string> OR)
{
	if (!OR.size()) return vector<pair<string, int>>();
	vector<pair<string, int>> result;
	for (auto it : OR)
	{
		vector<pair<string, int>> tmp = this->search(it);
		tmp = this->unionSet(result, tmp);
	}
	return result;
}

//------------------------------------------------EXCLUDE----------------------------------------------------//
void firstSearch::exclude(string ss)
{
	if (!ss.size()) return;
	if (!container) return;
	else if (!container->size()) return;
	vector<pair<string, int>> tmp = this->search(ss);
	for (auto it : tmp)
	{
		auto cur = container->find(it.first);
		if (cur != container->end()) container->erase(cur);
	}
	return;
}

//------------------------------------------------TITLE----------------------------------------------------//
vector<pair<string, int>> firstSearch::intitle(string word) {
	if (!word.size()) return vector<pair<string, int>>();
	vector <pair <string, int>> files = this->search(word);
	vector <pair <string, int>> ret;
	ifstream fi;
	for (auto file : files) {
		fi.open(file.first);
		string t = "";
		int freq = 0;
		do {
			char c = fi.get();
			if (c == '.') break;
			if (c == ' ') {
				if (t == word) ++freq;
				t = "";
			}
			else t += c;
		} while (true);
		if (t == word) ++freq;
		if (freq) ret.push_back({ file.first, freq });
		fi.close();
	}
	return ret;
}

//------------------------------------------------RANGE----------------------------------------------------//
vector<pair<string, int>> firstSearch::number_searching(int lower, int upper, bool is_price)
{
	vector<pair<string, int>> ans;

	for (int i = lower; i <= upper; ++i)
		if (is_price)
			ans = unionSet(ans, search("$" + to_string(i), T));
		else
			ans = unionSet(ans, search(to_string(i), T));

	return ans;
}

//------------------------------------------------PRICE----------------------------------------------------//
vector<pair<string, int>> firstSearch::price_searching(string object, int price)
{
	vector<pair<string, int>> file_contain_object = search(object, T);
	//object must be put into "". E.g: "black rose"

	vector<pair<string, int>> file_contain_price = number_searching(price - 30, price + 30, true);

	return intersection(file_contain_object, file_contain_price);
}

vector<pair<string, int>> firstSearch::price_searching(int price)
{
	return number_searching(price - 30, price + 30, true);
}

//------------------------------------------------FILETYPE----------------------------------------------------//
vector<pair<string, int>> firstSearch::filetype(string type) {
	if (type == "txt" || type.size() == 0) return vector<pair<string, int>>();
	File_Handling fh;
	vector <string> files = fh.load_file_names("__index.txt");
	vector <pair <string, int>> ret;
	for (string file : files) {
		bool flag = true;
		for (int i = 0; i < type.length(); ++i)
			if (file[file.length() - i - 1] != type[type.length() - i - 1]) {
				flag = false; break;
			}
		if (flag) ret.push_back({ file, 0 });
	}
	return ret;
}

//----------------------------------------------CONSTRUCTOR AND MISC--------------------------------------------//

vector<pair<string, int>> firstSearch::map_to_vector(unordered_map<string, int> *con)
{
	if (!con) return vector<pair<string, int>>();
	else if (!con->size()) return vector<pair<string, int>>();
	vector<pair<string, int>> result;
	for (auto it = con->begin(); it!= con->end(); it++)
	{
		result.push_back(make_pair(it->first, it->second));
	}
	delete con;
	return result;
}

void show(unordered_map<string, int>* container, string header)
{
	cout << "THIS IS DEBUG RESULT FOR " << header << ":";
	if(!container)
	{
		cout << "EMPTY" << endl;
		return;
	}
	else if (!container->size())
	{
		cout << "EMPTY" << endl;
		return;
	}
	cout << " - SIZE: " << container->size() << endl;
	for (auto it : (*container))
	{
		cout << it.first << " " << it.second << endl;
	}
}

vector<pair<string, int>> firstSearch::search()
{
	//FIRST 8 AND(by default) + FILETYPE + TITLE + QUOTE/WILD + OR + EXCLUDE + HASHTAG(DEFUALT IN ORIGIN)
	if (QH.filetypeRe.size() != 0 && QH.filetypeRe != "txt")
		this->intersection(this->filetype(QH.filetypeRe));

	if (QH.intitleRe.size())
		this->intersection(this->intitle(QH.intitleRe)); //show(this->container, "INTITLE");

	if (QH.quotesRe.size())
		this->intersection(this->quote(QH.quotesRe)); //show(this->container, "AFTER QUOTES");

	if (QH.originRe.size())
		this->intersection(this->origin(QH.originRe)); //show(this->container, "AFTER ORIGIN");

	for (auto it : QH.orRe)
	{
		this->unionSet(this->findOr(it));
	}
	if (QH.excludeRe.size())
		this->exclude(QH.excludeRe); //show(this->container, "AFTER EXCLUDE");

	//PRICE AND RANGE -> 10 OPE
	if (QH.rangeRe.first.size() && QH.rangeRe.first != "-1")
	{
		int lower = string_to_int(QH.rangeRe.first);
		int upper = string_to_int(QH.rangeRe.second);
		this->intersection(this->number_searching(lower, upper, false));
		//show(this->container, "AFTER RANGE");
	}

	if (QH.priceRe.size()) this->intersection(this->price_searching(string_to_int(QH.priceRe)));
	vector<pair<string, int>> result = this->map_to_vector(this->container);
	this->container = nullptr;
	return result;
}

firstSearch::firstSearch(string query, Trie T)
{
	if (this->T.isEmpty()) this->T = T;
	this->query = query;
	this->QH = QueryHandling(query);
}

