#ifndef __SEARCH__
#define __SEARCH__

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>

#include "trie.h"
#include "query_handling.h"

using namespace std;

class firstSearch {
private:
	//VARIABLES
	string query;
	unordered_map<string, int>* container = nullptr;
	Trie T;
	QueryHandling QH;

	void exit();

	void intersection(vector<pair<string, int>> f2);

	vector<pair<string, int>> intersection(vector<pair<string, int>> f1, vector<pair<string, int>> f2);

	void unionSet(vector<pair<string, int>> f1);

	vector<pair<string, int>> unionSet(vector<pair<string, int>> f1, vector<pair<string, int>> f2);

	vector<pair<string, int>> search(string ss, Trie T);

	vector<pair<string, int>> search(string ss);

	vector<pair<string, int>> map_to_vector(unordered_map<string, int> *con);


	//SS STAND FOR SEARCH_STRING
	//OPERATOR COUNT: 6 + AND = 7 + 1 HASHTAG = 8 + 2 = 10
	vector<pair<string, int>> quote(string quotes); //TASK QUOTE + WILDCARD -> 2OPE

	vector<pair<string, int>> origin(vector<string> origins);

	vector<pair<string, int>> findOr(vector<string> OR);//1 OPE

	void exclude(string ss);//1 OPE

	vector <pair <string, int>> intitle(string word);//1OPE

	vector <pair <string, int>> filetype(string type); //1 OPE

	vector<pair<string, int>> number_searching(int lower, int upper, bool is_price); //RANGE

	vector<pair<string, int>> price_searching(string object, int price); //PRICE

	vector<pair<string, int>> price_searching(int price); //PRICE WITHOUT OBJECT

public:

	vector<pair<string, int>> search();

	firstSearch(string query, Trie T);
};
#endif