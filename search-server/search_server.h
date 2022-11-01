#pragma once
#include <string>
#include <set>
#include <map>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <execution>
#include <stdexcept>

#include "document.h"
#include "string_processing.h"
#include "concurrent_map.h"
const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double ACCURACY = 1e-6;

class SearchServer {
public:
    const std::set<int>::const_iterator begin() const;
    const std::set<int>::const_iterator end() const;

    template <typename StringContainer>
    SearchServer(const StringContainer& stop_words);
    SearchServer(const std::string& stop_words_text);
    const std::map<std::string_view, double>& GetWordFrequencies(int document_id) const;

    void RemoveDocument(int document_id);
    template<class ExecutionPolicy>
    void RemoveDocument(ExecutionPolicy&& policy, int document_id) {
        if (documents_.count(document_id) == 0) {
            return;
        }
        if (document_to_word_freqs_.count(document_id)) {
            const auto& words_erase = document_to_word_freqs_.at(document_id);
            std::for_each(policy, words_erase.begin(), words_erase.end(), [this, document_id](const auto x) {
                word_to_document_freqs_.at(x.first).erase(document_id);
                });
            documents_.erase(document_id);
            document_ids_.erase(document_id);
            document_to_word_freqs_.erase(document_id);
        }
    }

    void AddDocument(int document_id, std::string_view document, DocumentStatus status, const std::vector<int>& ratings);

    template <typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(std::string_view raw_query, DocumentPredicate document_predicate) const;

    template <typename DocumentPredicate, typename ExecutionPolicy>
    std::vector<Document> FindTopDocuments(ExecutionPolicy&& policy, std::string_view raw_query, DocumentPredicate document_predicate) const;

    std::vector<Document> FindTopDocuments(std::string_view raw_query, DocumentStatus status) const;

    template <typename ExecutionPolicy>
    std::vector<Document> FindTopDocuments(ExecutionPolicy&& policy, std::string_view raw_query, DocumentStatus status) const;

    std::vector<Document> FindTopDocuments(std::string_view raw_query) const;

    template <typename ExecutionPolicy>
    std::vector<Document> FindTopDocuments(ExecutionPolicy&& policy, std::string_view raw_query) const;

    int GetDocumentCount() const;

    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(std::string_view raw_query, int document_id) const;

    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(const std::execution::parallel_policy& policy, std::string_view raw_query, int document_id) const;

    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(const std::execution::sequenced_policy& policy, std::string_view raw_query, int document_id) const;

private:
    struct DocumentData {
        int rating = {};
        DocumentStatus status = {};
        std::string data = {};
    };

    const std::set<std::string_view, std::less<>> stop_words_;
    std::map<std::string_view, std::map<int, double>> word_to_document_freqs_;
    std::map<int, std::map<std::string_view, double>> document_to_word_freqs_;
    std::map<int, DocumentData> documents_;
    std::set<int> document_ids_;

    bool IsStopWord(std::string_view word) const;
    static bool IsValidWord(std::string_view word);
    std::vector<std::string_view> SplitIntoWordsNoStop(std::string_view text) const;
    static int ComputeAverageRating(const std::vector<int>& ratings);
    struct QueryWord {
        std::string_view data = {};
        bool is_minus = {};
        bool is_stop = {};
    };

    QueryWord ParseQueryWord(std::string_view text) const;
    struct Query {
        std::vector<std::string_view> plus_words;
        std::vector<std::string_view> minus_words;
    };
    Query ParseQuery(std::string_view text) const;
    Query ParseQueryNoUnique(std::string_view text) const;
    double ComputeWordInverseDocumentFreq(std::string_view word) const;
    template <typename DocumentPredicate>
    std::vector<Document> FindAllDocuments(std::string_view raw_query, DocumentPredicate document_predicate) const;

    template <typename DocumentPredicate>
    std::vector<Document> FindAllDocuments(const std::execution::sequenced_policy& policy, std::string_view raw_query, DocumentPredicate document_predicate) const;

    template <typename DocumentPredicate>
    std::vector<Document> FindAllDocuments(const std::execution::parallel_policy& policy, std::string_view raw_query, DocumentPredicate document_predicate) const;


};

template <typename StringContainer>
SearchServer::SearchServer(const StringContainer& stop_words)
    : stop_words_(MakeUniqueNonEmptyStrings(stop_words)) {
    if (!all_of(stop_words_.begin(), stop_words_.end(), IsValidWord)) {
        throw std::invalid_argument("Некоретные символы в стоп-слове");
    }
}

template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindTopDocuments(std::string_view raw_query, DocumentPredicate document_predicate) const {
    return FindTopDocuments(std::execution::seq, raw_query, document_predicate);
}

template <typename DocumentPredicate, typename ExecutionPolicy>
std::vector<Document> SearchServer::FindTopDocuments(ExecutionPolicy&& policy, std::string_view raw_query, DocumentPredicate document_predicate) const {
    auto matched_documents = FindAllDocuments(policy, raw_query, document_predicate);
    std::sort(policy, matched_documents.begin(), matched_documents.end(),
        [](const Document& lhs, const Document& rhs) {
            if (std::abs(lhs.relevance - rhs.relevance) < ACCURACY) {
                return lhs.rating > rhs.rating;
            }
            else {
                return lhs.relevance > rhs.relevance;
            }
        });
    if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
        matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
    }
    return matched_documents;
}

template <typename ExecutionPolicy>
std::vector<Document> SearchServer::FindTopDocuments(ExecutionPolicy&& policy, std::string_view raw_query, DocumentStatus status) const {
    return FindTopDocuments(policy, raw_query, [status](int document_id, DocumentStatus document_status, int rating) {return document_status == status; });
}

template <typename ExecutionPolicy>
std::vector<Document> SearchServer::FindTopDocuments(ExecutionPolicy&& policy, std::string_view raw_query) const {
    return FindTopDocuments(policy, raw_query, DocumentStatus::ACTUAL);
}

//-----------
template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindAllDocuments(std::string_view raw_query, DocumentPredicate document_predicate) const {
    return FindAllDocuments(std::execution::seq, raw_query, document_predicate);
}

template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindAllDocuments(const std::execution::sequenced_policy& policy, std::string_view raw_query, DocumentPredicate document_predicate) const {
    std::map<int, double> document_to_relevance;
    const auto query = ParseQuery(raw_query);
    for (std::string_view word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
        for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
            const auto& document_data = documents_.at(document_id);
            if (document_predicate(document_id, document_data.status, document_data.rating)) {
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }
    }
    for (std::string_view word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
            document_to_relevance.erase(document_id);
        }
    }
    std::vector<Document> matched_documents;
    for (const auto [document_id, relevance] : document_to_relevance) {
        matched_documents.push_back({ document_id, relevance, documents_.at(document_id).rating });
    }
    return matched_documents;
}

template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindAllDocuments(const std::execution::parallel_policy& policy, std::string_view raw_query, DocumentPredicate document_predicate) const {
    ConcurrentMap<int, double> document_to_relevance(10);
    const auto query = ParseQuery(raw_query);

    std::for_each(policy, query.minus_words.begin(), query.minus_words.end(),
        [this, &document_to_relevance](std::string_view word) {
            if (word_to_document_freqs_.count(word)) {
                for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                    document_to_relevance.BuildOrdinaryMap().erase(document_id);
                }
            }
        });
    std::for_each(policy, query.plus_words.begin(), query.plus_words.end(),
        [this, &document_predicate, &document_to_relevance](std::string_view word) {
            if (word_to_document_freqs_.count(word)) {
                const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
                for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                    const auto& document_data = documents_.at(document_id);
                    if (document_predicate(document_id, document_data.status, document_data.rating)) {
                        document_to_relevance[document_id].ref_to_value += term_freq * inverse_document_freq;
                    }
                }
            }
        }
    );

    std::vector<Document> matched_documents;
    for (const auto [document_id, relevance] : document_to_relevance.BuildOrdinaryMap()) {
        matched_documents.push_back({ document_id, relevance, documents_.at(document_id).rating });
    }
    return matched_documents;
}
