#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Forensics tool for analyzing intelligence reports
// Reads from message_log.txt, analyzes text, and saves report to analysis_report.txt

// Clean and normalize text input.
string cleanText(string rawLine) {
    for (char &ch : rawLine) {
        unsigned char uch = static_cast<unsigned char>(ch);
        if (isalnum(uch) || isspace(uch)) {
            ch = static_cast<char>(tolower(uch));
        } else {
            ch = ' ';
        }
    }

    stringstream ss(rawLine);
    string token;
    string cleaned;
    while (ss >> token) {
        if (!cleaned.empty()) {
            cleaned += ' ';
        }
        cleaned += token;
    }
    return cleaned;
}

// Extract numeric value from token, return 0 if invalid
int extractNumericValue(string token) {
    if (token.empty()) {
        return 0;
    }

    size_t start = 0;
    if (token[0] == '-' || token[0] == '+') {
        if (token.size() == 1) {
            return 0;
        }
        start = 1;
    }

    for (size_t i = start; i < token.size(); ++i) {
        if (!isdigit(static_cast<unsigned char>(token[i]))) {
            return 0;
        }
    }

    try {
        return stoi(token);
    } catch (...) {
        return 0;
    }
}

// Check if token is a valid number
bool isNumericToken(const string &token) {
    if (token.empty()) {
        return false;
    }

    size_t start = 0;
    if (token[0] == '-' || token[0] == '+') {
        if (token.size() == 1) {
            return false;
        }
        start = 1;
    }

    for (size_t i = start; i < token.size(); ++i) {
        if (!isdigit(static_cast<unsigned char>(token[i]))) {
            return false;
        }
    }
    return true;
}

// Check if word is a common stop word
bool isStopWord(const string &token) {
    static const vector<string> stopWords = {
        "a", "an", "and", "are", "at", "be", "for", "from", "in", "is", "it", "of", "on", "or", "the", "to", "with"
    };

    for (const string &stopWord : stopWords) {
        if (token == stopWord) {
            return true;
        }
    }
    return false;
}

// Update word frequency in analysis
void updateWordFrequency(
    const string &word,
    vector<string> &uniqueWords,
    vector<int> &wordFrequencies,
    int &totalWords,
    bool ignoreStopWords
) {
    if (ignoreStopWords && isStopWord(word)) {
        return;
    }

    ++totalWords;

    for (size_t i = 0; i < uniqueWords.size(); ++i) {
        if (uniqueWords[i] == word) {
            ++wordFrequencies[i];
            return;
        }
    }

    uniqueWords.push_back(word);
    wordFrequencies.push_back(1);
}

// Analyze tokens in a cleaned line
void analyzeTokens(
    string cleanedLine,
    vector<string> &uniqueWords,
    vector<int> &wordFrequencies,
    int &totalWords,
    vector<int> &numericCodes,
    long long &numericSum,
    int &numericCount,
    vector<string> &cleanedLines,
    bool ignoreStopWords
) {
    if (cleanedLine.empty()) {
        return;
    }

    cleanedLines.push_back(cleanedLine);

    stringstream ss(cleanedLine);
    string token;

    while (ss >> token) {
        if (isNumericToken(token)) {
            int value = extractNumericValue(token);
            numericCodes.push_back(value);
            numericSum += value;
            ++numericCount;
        } else {
            updateWordFrequency(token, uniqueWords, wordFrequencies, totalWords, ignoreStopWords);
        }
    }
}

// Get indices sorted by frequency
vector<size_t> sortedFrequencyIndices(const vector<string> &uniqueWords, const vector<int> &wordFrequencies) {
    vector<size_t> order(uniqueWords.size());
    for (size_t i = 0; i < order.size(); ++i) {
        order[i] = i;
    }

    sort(order.begin(), order.end(), [&](size_t a, size_t b) {
        if (wordFrequencies[a] != wordFrequencies[b]) {
            return wordFrequencies[a] > wordFrequencies[b];
        }
        return uniqueWords[a] < uniqueWords[b];
    });

    return order;
}

// Get frequency of a specific word
int getWordFrequency(const string &word, const vector<string> &uniqueWords, const vector<int> &wordFrequencies) {
    for (size_t i = 0; i < uniqueWords.size(); ++i) {
        if (uniqueWords[i] == word) {
            return wordFrequencies[i];
        }
    }
    return 0;
}

// Display analysis report to console
void displayReport(
    const vector<string> &cleanedLines,
    const vector<string> &uniqueWords,
    const vector<int> &wordFrequencies,
    int totalWords,
    const vector<int> &numericCodes,
    long long numericSum,
    int numericCount,
    const string &searchedWord,
    int searchedFrequency
) {
    cout << "\n===== CLEANED INTELLIGENCE REPORT =====\n";
    for (const string &line : cleanedLines) {
        cout << line << '\n';
    }

    cout << "\n===== ANALYSIS SUMMARY =====\n";
    cout << "Total words: " << totalWords << '\n';
    cout << "Unique words: " << uniqueWords.size() << '\n';

    cout << "\nNumeric code count: " << numericCount << '\n';
    cout << "Numeric code sum: " << numericSum << '\n';
    double avg = (numericCount > 0) ? static_cast<double>(numericSum) / numericCount : 0.0;
    cout << fixed << setprecision(2);
    cout << "Numeric code average: " << avg << '\n';

    cout << "\nSearch result: '" << searchedWord << "' appears " << searchedFrequency << " time(s).\n";

    cout << "\n===== WORD FREQUENCIES (DESC) =====\n";
    cout << left << setw(18) << "Word" << "Frequency\n";
    cout << string(28, '-') << '\n';

    vector<size_t> order = sortedFrequencyIndices(uniqueWords, wordFrequencies);
    for (size_t index : order) {
        cout << left << setw(18) << uniqueWords[index] << wordFrequencies[index] << '\n';
    }

    cout << "\n===== NUMERIC CODES =====\n";
    if (numericCodes.empty()) {
        cout << "(none)\n";
    } else {
        for (size_t i = 0; i < numericCodes.size(); ++i) {
            cout << numericCodes[i];
            if (i + 1 < numericCodes.size()) {
                cout << ", ";
            }
        }
        cout << '\n';
    }
}

// Save analysis report to file
void saveReport(
    const string &outputPath,
    const vector<string> &cleanedLines,
    const vector<string> &uniqueWords,
    const vector<int> &wordFrequencies,
    int totalWords,
    const vector<int> &numericCodes,
    long long numericSum,
    int numericCount,
    const string &searchedWord,
    int searchedFrequency
) {
    ofstream outFile(outputPath);
    if (!outFile) {
        cerr << "Error: could not write report to " << outputPath << '\n';
        return;
    }

    outFile << "===== CLEANED INTELLIGENCE REPORT =====\n";
    for (const string &line : cleanedLines) {
        outFile << line << '\n';
    }

    outFile << "\n===== ANALYSIS SUMMARY =====\n";
    outFile << "Total words: " << totalWords << '\n';
    outFile << "Unique words: " << uniqueWords.size() << '\n';
    outFile << "Numeric code count: " << numericCount << '\n';
    outFile << "Numeric code sum: " << numericSum << '\n';

    double avg = (numericCount > 0) ? static_cast<double>(numericSum) / numericCount : 0.0;
    outFile << fixed << setprecision(2);
    outFile << "Numeric code average: " << avg << '\n';

    outFile << "Search result: '" << searchedWord << "' appears " << searchedFrequency << " time(s).\n";

    outFile << "\n===== WORD FREQUENCIES (DESC) =====\n";
    outFile << left << setw(18) << "Word" << "Frequency\n";
    outFile << string(28, '-') << '\n';

    vector<size_t> order = sortedFrequencyIndices(uniqueWords, wordFrequencies);
    for (size_t index : order) {
        outFile << left << setw(18) << uniqueWords[index] << wordFrequencies[index] << '\n';
    }

    outFile << "\n===== NUMERIC CODES =====\n";
    if (numericCodes.empty()) {
        outFile << "(none)\n";
    } else {
        for (size_t i = 0; i < numericCodes.size(); ++i) {
            outFile << numericCodes[i];
            if (i + 1 < numericCodes.size()) {
                outFile << ", ";
            }
        }
        outFile << '\n';
    }
}

// Main program entry point
int main() {
    const string inputPath = "message_log.txt";
    const string outputPath = "analysis_report.txt";
    const bool ignoreCommonStopWords = true;

    ifstream inFile(inputPath);
    if (!inFile) {
        cerr << "Error: could not open " << inputPath << " for reading.\n";
        return 1;
    }

    vector<string> uniqueWords;
    vector<int> wordFrequencies;
    vector<int> numericCodes;
    vector<string> cleanedLines;

    int totalWords = 0;
    long long numericSum = 0;
    int numericCount = 0;

    string rawLine;
    // Process each line from input file
    while (getline(inFile, rawLine)) {
        string cleanedLine = cleanText(rawLine);
        analyzeTokens(
            cleanedLine,
            uniqueWords,
            wordFrequencies,
            totalWords,
            numericCodes,
            numericSum,
            numericCount,
            cleanedLines,
            ignoreCommonStopWords
        );
    }

    string query;
    // Get user search query
    cout << "Enter a word to search frequency: ";
    getline(cin, query);

    query = cleanText(query);
    stringstream queryStream(query);
    string searchedWord;
    queryStream >> searchedWord;

    // Get frequency of searched word
    int searchedFrequency = 0;
    if (!searchedWord.empty()) {
        searchedFrequency = getWordFrequency(searchedWord, uniqueWords, wordFrequencies);
    }

    // Display and save report
    displayReport(
        cleanedLines,
        uniqueWords,
        wordFrequencies,
        totalWords,
        numericCodes,
        numericSum,
        numericCount,
        searchedWord.empty() ? "(empty input)" : searchedWord,
        searchedFrequency
    );

    saveReport(
        outputPath,
        cleanedLines,
        uniqueWords,
        wordFrequencies,
        totalWords,
        numericCodes,
        numericSum,
        numericCount,
        searchedWord.empty() ? "(empty input)" : searchedWord,
        searchedFrequency
    );

    cout << "\nAnalysis saved to " << outputPath << '\n';
    return 0;
}
