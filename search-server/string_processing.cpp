#include "string_processing.h"

using namespace std;

vector<string_view> SplitIntoWords(string_view str) {
    vector<string_view> result;
    const int64_t pos = str.npos;
    while (true) {
        int64_t space = str.find(' ');
        result.push_back(space == pos ? str.substr(0) : str.substr(0, space));
        str.remove_prefix(std::min(str.find_first_of(" ") + 1, str.size()));
        if (space == pos) {
            break;
        }
    }
    return result;
}