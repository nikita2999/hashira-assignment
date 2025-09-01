#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "json.hpp" 

using json = nlohmann::json;
using namespace std;

// Function to convert a string from given base to decimal
long long convertToDecimal(const string& value, int base) {
    long long result = 0;
    long long power = 1;

    for (int i = value.length() - 1; i >= 0; i--) {
        char c = value[i];
        int digit;

        if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else if (c >= 'a' && c <= 'z') {
            digit = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'Z') {
            digit = c - 'A' + 10;
        } else {
            throw invalid_argument("Invalid character in number");
        }

        if (digit >= base) {
            throw invalid_argument("Digit exceeds base value");
        }

        result += digit * power;
        power *= base;
    }

    return result;
}

// Function to parse JSON and extract roots
vector<long long> parseRoots(const json& data) {
    vector<long long> roots;
    int n = data["keys"]["n"];

    for (int i = 1; i <= n; i++) {
        string key = to_string(i);
        if (data.contains(key)) {
            string base_str = data[key]["base"];
            string value_str = data[key]["value"];

            int base = stoi(base_str);
            long long decimal_value = convertToDecimal(value_str, base);
            roots.push_back(decimal_value);
        }
    }

    return roots;
}

// Function to find quadratic coefficients using Vieta's formulas
long long findConstantC(const vector<long long>& roots) {
    if (roots.size() < 2) {
        throw runtime_error("Need at least 2 roots for quadratic equation");
    }

    // For quadratic equation ax² + bx + c = 0 with roots r1, r2
    // r1 * r2 = c/a, assuming a=1, so c = r1 * r2
    return roots[0] * roots[1];
}

void processFile(const string& filename) {
    try {
        cout << "=== Processing " << filename << " ===" << endl;

        // Read JSON input from file
        ifstream input_file(filename);
        if (!input_file.is_open()) {
            cerr << "Error: Could not open file " << filename << endl;
            return;
        }

        json data;
        input_file >> data;
        input_file.close();

        // Step 1: Parse keys
        int n = data["keys"]["n"];
        int k = data["keys"]["k"];
        cout << "n: " << n << ", k: " << k << endl;

        // Step 2: Decode roots from different bases to decimal
        vector<long long> roots = parseRoots(data);

        cout << "Decoded roots:" << endl;
        for (size_t i = 0; i < roots.size(); i++) {
            cout << "Root " << (i+1) << ": " << roots[i] << endl;
        }

        // Step 3: Find constant c using Vieta's formulas
        long long c = findConstantC(roots);
        cout << "Constant c: " << c << endl;
        cout << "=================================" << endl << endl;

    } catch (const exception& e) {
        cerr << "Error processing " << filename << ": " << e.what() << endl;
    }
}

int main() {
    cout << "Polynomial Equation Solver" << endl;
    cout << "==========================" << endl << endl;

    // Process both input files
    processFile("input1.json");
    processFile("input2.json");

    return 0;
}