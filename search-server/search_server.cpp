#include "search_server.h"
#include <iostream>
#include <cmath>
using namespace std;

SearchServer::SearchServer(const string& stop_words_text)
    : SearchServer(SplitIntoWords(stop_words_text)) {}

set<int>::const_iterator SearchServer::begin() const {
    return document_ids_.begin();
}

set<int>::const_iterator SearchServer::end() const {
    return document_ids_.end();
}
const map<string, double>& SearchServer::GetWordFrequencies(int document_id) const {
    if (document_to_word_freqs_.count(document_id)) {
        return document_to_word_freqs_.at(document_id);
    }
    static map<string, double> mapfreqs;
    return mapfreqs;
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}

void SearchServer::RemoveDocument(int document_id) {
    if (documents_.count(document_id) == 0) {
        return;
    }
    auto& words_erase = document_to_word_freqs_.at(document_id);
    for (auto& [word, freq] : words_erase) {
        word_to_document_freqs_.at(word).erase(document_id);
    }
    documents_.erase(document_id);
    document_ids_.erase(document_id);
    document_to_word_freqs_.erase(document_id);
}

void SearchServer::AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
    if (document_id < 0) { // Проверка для отрицательного id  
        throw invalid_argument{ "Отрицательный индекс при добавлении документа: " + to_string(document_id) };
    }
    if (documents_.count(document_id) > 0) {
        throw invalid_argument{ "Попытка добавить документ c id ранее добавленного документа:  " + to_string(document_id) };
    }
    const vector<string> words = SplitIntoWordsNoStop(document);
    const double inv_word_count = 1.0 / words.size();

    for (string word : words) {
        auto it = words_.insert(static_cast<string>(word));
        word_to_document_freqs_[*it.first][document_id] += inv_word_count;
        document_to_word_freqs_[document_id][*it.first] += inv_word_count;
    }

    documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings),status });
    document_ids_.insert(document_id);
}
vector<Document> SearchServer::FindTopDocuments(const string& raw_query, DocumentStatus status) const {
    return FindTopDocuments(raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
        return document_status == status;
        });
}
vector<Document> SearchServer::FindTopDocuments(const string& raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

tuple<vector<string>, DocumentStatus> SearchServer::MatchDocument(const string& raw_query, int document_id) const {
    const auto query = ParseQuery(raw_query);
    vector<string> matched_words;
    for (const string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }
    for (const string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }
    return { matched_words, documents_.at(document_id).status };
}

bool SearchServer::IsStopWord(const string& word) const {
    return stop_words_.count(word) > 0;
}

bool SearchServer::IsValidWord(const string& word) {
    // A valid word must not contain special characters
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
        });
}

vector<string> SearchServer::SplitIntoWordsNoStop(const string& text) const {
    vector<string> words;
    for (const string& word : SplitIntoWords(text)) {
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
        if (!IsValidWord(word)) { // Проверка наличия спецсимволов  
            throw invalid_argument{ "Некорректные символы в слове: " + word };
        }
    }
    return words;
}

int SearchServer::ComputeAverageRating(const vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }
    int rating_sum = accumulate(ratings.begin(), ratings.end(), 0);
    return rating_sum / static_cast<int>(ratings.size());
}


SearchServer::QueryWord SearchServer::ParseQueryWord(const string& text) const {
    if (!IsValidWord(text)) { // Проверка наличия спецсимволов   
        throw invalid_argument{ "Некорректные символы в слове: " + text };
    }
    string word = text;
    bool is_minus = false;
    if (word[0] == '-') {
        is_minus = true;
        word = word.substr(1);
    }
    return { word, is_minus, IsStopWord(word) };
}

SearchServer::Query SearchServer::ParseQuery(const string& text) const {
    Query query;
    for (const string& word : SplitIntoWords(text)) {
        QueryWord query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                query.minus_words.insert(query_word.data);
            }
            else {
                query.plus_words.insert(query_word.data);
            }
        }
    }
    for (const auto& query_minus_word : query.minus_words) { //Проверка для минус-слова  
        if (query_minus_word == "") {
            throw invalid_argument{ "Неправильное минус-слово: Отсутствие текста после символа «минус» " + query_minus_word };
        }
        if (query_minus_word[0] == '-') {
            throw invalid_argument{ "Неправильное минус-слово: Наличие более чем одного минуса  " + query_minus_word };
        }
    }
    return query;
}
double SearchServer::ComputeWordInverseDocumentFreq(const string& word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}