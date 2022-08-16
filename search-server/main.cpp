#include "search_server.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

/* Подставьте вашу реализацию класса SearchServer сюда */
/*struct Document {
    int id;
    double relevance;
    int rating;
};

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};
class SearchServer {
public:
    void SetStopWords(const string& text) { //добавляет стоп-слова в множество
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id,DocumentData{ComputeAverageRating(ratings),status
            });
    }

    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate) const { // возвращает документы по убыванию рейтинга и релевантности
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query, document_predicate);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                if (abs(lhs.relevance - rhs.relevance) < 1e-6) {
                    return lhs.rating > rhs.rating;
                } else {
                    return lhs.relevance > rhs.relevance;
                }
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus status) const {
        return FindTopDocuments(raw_query, [status](int document_id, DocumentStatus STATUS, int rating) { return STATUS == status; });
    }
    vector<Document> FindTopDocuments(const string& raw_query) const {
        return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
    }

    int GetDocumentCount() const {
        return documents_.size();
    }

    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const {
        const Query query = ParseQuery(raw_query);
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
        return {matched_words, documents_.at(document_id).status};
    }

private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };

    set<string> stop_words_;
    map<string, map<int, double>> word_to_document_freqs_;
    map<int, DocumentData> documents_;

    bool IsStopWord(const string& word) const { //является ли слово стоп-словом
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const { //разбивает любую строку на слова без стоп-слов
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    static int ComputeAverageRating(const vector<int>& ratings) {//вычисление среднего рейтинга
        if (ratings.empty()) {
            return 0;
        }
        int rating_sum = 0;
        for (const int rating : ratings) {
            rating_sum += rating;
        }
        return rating_sum / static_cast<int>(ratings.size());
    }

    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(string text) const {
        bool is_minus = false;
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return {
            text,
            is_minus,
            IsStopWord(text)
        };
    }

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    Query ParseQuery(const string& text) const {
        Query query;
        for (const string& word : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                } else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
    }

    double ComputeWordInverseDocumentFreq(const string& word) const { //вычисление IDF
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }

    template <typename DocumentPredicate>
    vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const {
        map<int, double> document_to_relevance;
        for (const string& word : query.plus_words) {
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

        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        vector<Document> matched_documents;
        for (const auto [document_id, relevance] : document_to_relevance) {
            matched_documents.push_back({
                document_id,
                relevance,
                documents_.at(document_id).rating
            });
        }
        return matched_documents;
    }
};*/

/*template <typename T, typename U>
void AssertEqualImpl(const T& t, const U& u, const string& t_str, const string& u_str, const string& file,
                     const string& func, unsigned line, const string& hint) {
    if (t != u) {
        cout << boolalpha;
        cout << file << "("s << line << "): "s << func << ": "s;
        cout << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
        cout << t << " != "s << u << "."s;
        if (!hint.empty()) {
            cout << " Hint: "s << hint;
        }
        cout << endl;
        abort();
    }
}

#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s)
#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))

void AssertImpl(bool value, const string& expr_str, const string& file, const string& func, unsigned line,
                const string& hint) {
    if (!value) {
        cout << file << "("s << line << "): "s << func << ": "s;
        cout << "ASSERT("s << expr_str << ") failed."s;
        if (!hint.empty()) {
            cout << " Hint: "s << hint;
        }
        cout << endl;
        abort();
    }
}

#define ASSERT(expr) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, ""s)
#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))*/



// -------- Начало модульных тестов поисковой системы ----------

//Поддержка стоп-слов. Стоп-слова исключаются из текста документов.
void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = { 1, 2, 3 };
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(), "Stop words must be excluded from documents"s);
    }
}
//Добавление документов
void TestAddDocument() {
    // проверка на нахождение 1 документа по слову
    SearchServer server;
    {
        const int doc_id = 42;
        const string content = "scorpion in the desert"s;
        const vector<int> ratings = { 1, 2, 3 };
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }
    // проверка на нахождение 1 документа, содержащего слово поискового запроса, из всех
    {
        const int doc_id = 18;
        const string content = "cow on the field"s;
        const vector<int> ratings = { 3, 2, 1 };
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT(doc0.id == 42);
    }
}
//Поддержка минус-слов.
void TestExcludeMinusWordsFromFindTopDocuments() {
    const int doc_id = 42;
    const string content = "scorpion in the desert"s;
    const vector<int> ratings = { 1, 2, 3 };
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
        ASSERT(server.FindTopDocuments("in -scorpion"s).empty());
    }
}

//Сортировка найденных документов по релевантности.
void TestRelevanceSortingOrder() {
    SearchServer server;
    const int doc_id1 = 0;
    const string content1 = "scorpion in the ground"s;
    const vector<int> ratings1 = { 1, 2, 3 };

    const int doc_id2 = 1;
    const string content2 = "scorpion in the desert"s;
    const vector<int> ratings2 = { 1, 2, 6 };

    server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
    server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
    const auto found_docs = server.FindTopDocuments("in desert"s);
    const Document& doc0 = found_docs[0];
    const Document& doc1 = found_docs[1];

    ASSERT_EQUAL(found_docs.size(), 2);
    ASSERT(doc0.relevance >= doc1.relevance);

}
//Вычисление рейтинга документов.
void TestRatingCalc() {
    SearchServer server;
    const int doc_id = 42;
    const string content = "scorpion in the ground"s;
    const vector<int> ratings = { 5, 7, 9 };
    int Rating = (5 + 7 + 9) / static_cast<int>(ratings.size());

    server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
    const auto found_docs = server.FindTopDocuments("the"s);
    ASSERT_EQUAL(found_docs.size(), 1u);
    const Document& doc0 = found_docs[0];
    ASSERT_EQUAL(doc0.rating, Rating);

}
// Фильтрация результатов поиска с использованием предиката, задаваемого пользователем.
void TestPredicat() {
    SearchServer server;
    server.SetStopWords("on in and"s);
    server.AddDocument(0, "straw hat and red head"s, DocumentStatus::ACTUAL, { -2, 9 });
    server.AddDocument(1, "white hat white car"s, DocumentStatus::ACTUAL, { 8, 8, 1 });
    server.AddDocument(2, "tall giraffe blue sky"s, DocumentStatus::ACTUAL, { -18, 6, 3, 1 });
    server.AddDocument(3, "tall rabbit desert"s, DocumentStatus::BANNED, { 17,3 });
    server.AddDocument(4, "white rabbit desert"s, DocumentStatus::BANNED, { 8, 5 });

    vector<int> doc = { 4, 0, 2 };
    int i = 0;
    for (const Document& document : server.FindTopDocuments("white tall hat"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; })) {
        ASSERT_EQUAL(document.id, doc[i]);
        ++i;
    }

}
//Поиск документов, имеющих заданный статус.
void TestFindTopDocumentsWithStatus() {
    SearchServer server;
    server.SetStopWords("on in and"s);

    server.AddDocument(0, "straw hat and red head"s, DocumentStatus::ACTUAL, { -2, 9 });
    server.AddDocument(1, "white hat white car"s, DocumentStatus::ACTUAL, { 8, 8, 1 });
    server.AddDocument(2, "tall giraffe blue sky"s, DocumentStatus::ACTUAL, { -18, 6, 3, 1 });
    server.AddDocument(3, "tall rabbit desert"s, DocumentStatus::BANNED, { 17,3 });
    server.AddDocument(4, "white rabbit desert"s, DocumentStatus::BANNED, { 8, 5 });

    {
        vector<int> id = { 1, 0, 2 };
        int i = 0;
        for (const Document& document : server.FindTopDocuments("white tall hat"s, DocumentStatus::ACTUAL)) {
            ASSERT_EQUAL(document.id, id[i]);
            ++i;
        }
    }
    {
        vector<int> id = { 3,4 };
        int i = 0;
        for (const Document& document : server.FindTopDocuments("white tall hat"s, DocumentStatus::BANNED)) {
            ASSERT_EQUAL(document.id, id[i]);
            ++i;
        }
    }
    {
        vector<Document> Document1 = server.FindTopDocuments("white tall hat"s, DocumentStatus::IRRELEVANT);
        ASSERT(Document1.empty());
    }
    {
        vector<Document> Document2 = server.FindTopDocuments("white tall hat"s, DocumentStatus::REMOVED);
        ASSERT(Document2.empty());
    }


}
//Корректное вычисление релевантности найденных документов.
void TestRelevanceCalc() {
    const int doc_id1 = 0;
    const string content1 = "scorpion in the ground"s;
    const vector<int> ratings1 = { 1, 2, 3 };

    const int doc_id2 = 1;
    const string content2 = "scorpion in the desert"s;
    const vector<int> ratings2 = { 1, 5, 9 };

    SearchServer server;
    server.AddDocument(doc_id1, content1, DocumentStatus::ACTUAL, ratings1);
    server.AddDocument(doc_id2, content2, DocumentStatus::ACTUAL, ratings2);
    const auto found_docs = server.FindTopDocuments("in garden"s);
    ASSERT_EQUAL(found_docs.size(), 2);

    const Document& doc0 = found_docs[0];
    const Document& doc1 = found_docs[1];
    ASSERT(doc0.relevance <= 1e-6);
    ASSERT(doc1.relevance <= 1e-6);
}

// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
    TestAddDocument();
    TestExcludeStopWordsFromAddedDocumentContent();
    TestExcludeMinusWordsFromFindTopDocuments();
    TestRelevanceSortingOrder();
    TestRatingCalc();
    TestRelevanceCalc();
    TestFindTopDocumentsWithStatus();
    TestPredicat();
}

// --------- Окончание модульных тестов поисковой системы -----------

int main() {
    TestSearchServer();
    cout << "Search server testing finished"s << endl;
}