#include "search_server.h"
#include <iostream> 
#include <cmath> 
using namespace std;

SearchServer::SearchServer(const string& stop_words_text) :SearchServer(SplitIntoWords(stop_words_text)) {}

const set<int>::const_iterator SearchServer::begin() const {
    return document_ids_.begin();
}

const set<int>::const_iterator SearchServer::end() const {
    return document_ids_.end();
}

const map<string_view, double>& SearchServer::GetWordFrequencies(int document_id) const {
    if (document_to_word_freqs_.count(document_id)) {
        return document_to_word_freqs_.at(document_id);
    }
    static map<string_view, double> mapfreqs;
    return mapfreqs;
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}

void SearchServer::RemoveDocument(int document_id) {
    RemoveDocument(std::execution::seq, document_id);
}
void SearchServer::AddDocument(int document_id, string_view document, DocumentStatus status, const vector<int>& ratings) {
    if (document_id < 0) { // Проверка для отрицательного id   
        throw invalid_argument("Отрицательный индекс при добавлении документа");
    }
    if (documents_.count(document_id) > 0) {
        throw invalid_argument("Попытка добавить документ c id ранее добавленного документа");
    }
    if (!IsValidWord(document)) {
        throw invalid_argument("Слово содержит специальный символ");
    }
    const auto [it, inserted_data] = documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status, string(document) });
    const auto words = SplitIntoWordsNoStop(it->second.data);
    const double inv_word_count = 1.0 / words.size();
    for (string_view word : words) {
        word_to_document_freqs_[word][document_id] += inv_word_count;
        document_to_word_freqs_[document_id][word] += inv_word_count;
    }
    document_ids_.insert(document_id);
}

vector<Document> SearchServer::FindTopDocuments(string_view raw_query, DocumentStatus status) const {
    return FindTopDocuments(execution::seq, raw_query, status);
}

vector<Document> SearchServer::FindTopDocuments(string_view raw_query) const {
    return FindTopDocuments(execution::seq, raw_query, DocumentStatus::ACTUAL);
}

tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(string_view raw_query, int document_id) const {
    return MatchDocument(execution::seq, raw_query, document_id);
}

tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(const execution::parallel_policy& policy, string_view raw_query, int document_id) const {
    const auto query = ParseQueryNoUnique(raw_query);
    vector<string_view> matched_words(query.plus_words.size());

    if (any_of(policy, query.minus_words.begin(), query.minus_words.end(),
        [this, &document_id](string_view word) {
            return word_to_document_freqs_.at(word).count(document_id);
        })) {
        return { {}, documents_.at(document_id).status };
    }
    auto it = std::copy_if(policy, query.plus_words.begin(), query.plus_words.end(), matched_words.begin(),
        [this, &document_id](string_view word) {
            return 	word_to_document_freqs_.at(word).count(document_id);
        });

    sort(policy, matched_words.begin(), it);
    auto pos = unique(policy, matched_words.begin(), it);
    size_t new_size = pos - matched_words.begin();
    matched_words.resize(new_size);
    return { matched_words, documents_.at(document_id).status };
}

tuple<vector<string_view>, DocumentStatus> SearchServer::MatchDocument(const execution::sequenced_policy& policy, string_view raw_query, int document_id) const {
    const auto query = ParseQuery(raw_query);
    vector<string_view> matched_words;
    for (string_view word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }
    for (string_view word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            return { {}, documents_.at(document_id).status };
        }
    }
    return { matched_words, documents_.at(document_id).status };
}

bool SearchServer::IsStopWord(string_view word) const {
    return stop_words_.count(word) > 0;
}

bool SearchServer::IsValidWord(string_view word) {
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
        });
}
vector<string_view> SearchServer::SplitIntoWordsNoStop(string_view text) const {
    vector<string_view> words;
    for (auto word : SplitIntoWords(text)) {
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
        if (!IsValidWord(word)) { // Проверка наличия спецсимволов   
            throw invalid_argument{ "Некорректные символы в слове: " + string(word) };
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

SearchServer::QueryWord SearchServer::ParseQueryWord(string_view text) const {
    if (text.empty()) {
        throw invalid_argument{ "Отсутствие минус-слова" + string(text) };
    }
    bool is_minus = false;
    if (text[0] == '-') {
        is_minus = true;
        text = text.substr(1);
    }
    if (!IsValidWord(text)) {
        throw invalid_argument{ "Некорректные символы в слове: " + string(text) };
    }
    if (text[0] == '-') {
        throw invalid_argument{ "Двойной знак - в минус-слове" + string(text) };
    }
    if (text.empty()) {
        throw invalid_argument{ "Пробел или нет текста после знака -" + string(text) };
    }
    return { text, is_minus, IsStopWord(text) };
}

SearchServer::Query SearchServer::ParseQuery(string_view text) const {
    Query result;

    for (auto word : SplitIntoWords(text)) {
        const auto query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                result.minus_words.push_back(query_word.data);
            }
            else {
                result.plus_words.push_back(query_word.data);
            }
        }
    }

    sort(result.minus_words.begin(), result.minus_words.end());
    sort(result.plus_words.begin(), result.plus_words.end());
    auto it_minus = unique(result.minus_words.begin(), result.minus_words.end());
    auto it_plus = unique(result.plus_words.begin(), result.plus_words.end());
    size_t new_size = it_minus - result.minus_words.begin();
    result.minus_words.resize(new_size);
    new_size = it_plus - result.plus_words.begin();
    result.plus_words.resize(new_size);
    return result;
}

SearchServer::Query SearchServer::ParseQueryNoUnique(string_view text) const {
    Query result;
    for (auto word : SplitIntoWords(text)) {
        const auto query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                result.minus_words.push_back(query_word.data);
            }
            else {
                result.plus_words.push_back(query_word.data);
            }
        }
    }
    for (const auto& query_minus_word : result.minus_words) { //Проверка для минус-слова  
        if (query_minus_word == "") {
            throw invalid_argument{ "Неправильное минус-слово: Отсутствие текста после символа «минус» " + string(query_minus_word) };
        }
        if (query_minus_word[0] == '-') {
            throw invalid_argument{ "Неправильное минус-слово: Наличие более чем одного минуса  " + string(query_minus_word) };
        }
    }
    return result;
}

double SearchServer::ComputeWordInverseDocumentFreq(string_view word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}