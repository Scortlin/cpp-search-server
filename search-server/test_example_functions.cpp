#include "test_example_functions.h"

using namespace std;

void PrintDocument(const Document& document) {
    cout << "{ "
        << "document_id = " << document.id << ", "
        << "relevance = " << document.relevance << ", "
        << "rating = " << document.rating << " }" << endl;
}

void PrintMatchDocumentResult(int document_id, const vector<string_view>& words, DocumentStatus status) {
    cout << "{ "s
        << "document_id = "s << document_id << ", "s
        << "status = "s << static_cast<int>(status) << ", "s
        << "words ="s;
    for (auto word : words) {
        cout << ' ' << word;
    }
    cout << "}"s << endl;
}

void AddDocument(SearchServer& search_server, int document_id, const string_view& document, DocumentStatus status,
    const vector<int>& ratings) {
    try {
        search_server.AddDocument(document_id, document, status, ratings);
    }
    catch (const invalid_argument& e) {
        cout << "������ ���������� ��������� "s << document_id << ": "s << e.what() << endl;
    }
}

void FindTopDocuments(const SearchServer& search_server, const string& raw_query) {
    cout << "���������� ������ �� �������: "s << raw_query << endl;
    try {
        for (const Document& document : search_server.FindTopDocuments(raw_query)) {
            PrintDocument(document);
        }
    }
    catch (const invalid_argument& e) {
        cout << "������ ������: "s << e.what() << endl;
    }
}

void MatchDocuments(const SearchServer& search_server, string_view query) {
    try {
        cout << "������� ���������� �� �������: "s << query << endl;
        const int document_count = search_server.GetDocumentCount();
        for (int index = 0; index < document_count; ++index) {
            for (const int document_id : search_server) {
                const auto [words, status] = search_server.MatchDocument(query, document_id);
                PrintMatchDocumentResult(document_id, words, status);
            }
        }
    }
    catch (const invalid_argument& e) {
        cout << "������ �������� ���������� �� ������ "s << query << ": "s << e.what() << endl;
    }
}