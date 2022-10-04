#include "search_server.h"
#include "remove_duplicates.h"

#include <set>

using namespace std;
void RemoveDuplicates(SearchServer& search_server) {
	set<int> remove_docs_ids;
	set<set<string>> del_doc;
	for (const int document_id : search_server) {
		map<string, double> word_freq = search_server.GetWordFrequencies(document_id);
		set<string> words;
		for (auto [word, freq] : word_freq) {
			words.insert(word);
		}
		if (del_doc.count(words) == 0) {
			del_doc.insert(words);
		}
		else {
			remove_docs_ids.insert(document_id);
		}
	}
	for (const int remove_doc_id : remove_docs_ids) {
		search_server.RemoveDocument(remove_doc_id);
		cout << "Found duplicate document id " << remove_doc_id << endl;
	}
}