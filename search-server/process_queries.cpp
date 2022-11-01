#include "process_queries.h"
using namespace std;

vector<vector<Document>> ProcessQueries(const SearchServer& search_server, const vector<string>& queries) {
    vector<vector<Document>> result(queries.size());
    transform(execution::par, queries.begin(), queries.end(), result.begin(),
        [&search_server](const string& query) {
            return search_server.FindTopDocuments(query);
        });
    return result;
}

vector<Document> ProcessQueriesJoined(const SearchServer& search_server, const vector<string>& queries) {
    vector<Document> result;
    for (auto& docs : ProcessQueries(search_server, queries)) {
        for (auto& document : docs) {
            result.push_back(move(document));
        }
    }
    return result;
}