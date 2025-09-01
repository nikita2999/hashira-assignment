#include <bits/stdc++.h>
#include <nlohmann/json.hpp>  // or: #include "json.hpp"

using namespace std;
using json = nlohmann::json;

// base-B string -> decimal (unsigned long long can overflow for huge y;
// but we'll use long double for solving and round at the end, same idea
// as the Lagrange version).
long long toDecimal(const string &value, int base) {
    long double acc = 0.0L;
    for (char c : value) {
        int d;
        if (isdigit(c)) d = c - '0';
        else if (isalpha(c)) d = tolower(c) - 'a' + 10;
        else continue;
        acc = acc * base + d;
    }
    // we only need the final integer value; round safely
    return (long long) llround(acc);
}

// Solve A * a = y using Gaussian elimination with partial pivoting
// A is k x k (Vandermonde), y is size k. Returns vector a (coefficients).
vector<long double> gaussianSolve(vector<vector<long double>> A, vector<long double> y) {
    int n = (int)A.size();

    // Augment A | y
    for (int i = 0; i < n; ++i) A[i].push_back(y[i]);

    for (int col = 0, row = 0; col < n && row < n; ++col, ++row) {
    
        int piv = row;
        for (int r = row + 1; r < n; ++r)
            if (fabsl(A[r][col]) > fabsl(A[piv][col])) piv = r;
        if (fabsl(A[piv][col]) < 1e-18L) continue;

        // swap rows
        if (piv != row) swap(A[piv], A[row]);
        long double div = A[row][col];
        for (int c = col; c <= n; ++c) A[row][c] /= div;
        for (int r = row + 1; r < n; ++r) {
            long double factor = A[r][col];
            if (fabsl(factor) < 1e-18L) continue;
            for (int c = col; c <= n; ++c) A[r][c] -= factor * A[row][c];
        }
    }

    // Back substitution
    vector<long double> a(n, 0.0L);
    for (int i = n - 1; i >= 0; --i) {
        // find leading 1
        int lead = -1;
        for (int c = 0; c < n; ++c) {
            if (fabsl(A[i][c]) > 1e-12L) { lead = c; break; }
        }
        if (lead == -1) continue; // degenerate (shouldn't happen)
        long double rhs = A[i][n];
        for (int c = lead + 1; c < n; ++c) rhs -= A[i][c] * a[c];
        a[lead] = rhs / A[i][lead]; // A[i][lead] should be 1 after normalization
    }
    return a;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Read complete JSON from stdin
    string input, line;
    while (getline(cin, line)) input += line;
    json data = json::parse(input);

    int n = data["keys"]["n"];
    int k = data["keys"]["k"];

    // (x, y) points
    vector<pair<long double, long double>> pts;
    pts.reserve(n);

    for (auto &el : data.items()) {
        if (el.key() == "keys") continue;
        int x = stoi(el.key());
        int base = stoi(el.value()["base"].get<string>());
        string v = el.value()["value"].get<string>();
        long long y = toDecimal(v, base);
        pts.push_back({(long double)x, (long double)y});
    }

    // sort by x and keep only k points
    sort(pts.begin(), pts.end(),
         [](auto &a, auto &b){ return a.first < b.first; });
    if ((int)pts.size() > k) pts.resize(k);

    // Build Vandermonde matrix A (row i: [1, x_i, x_i^2, ...])
    vector<vector<long double>> A(k, vector<long double>(k, 0.0L));
    vector<long double> Y(k, 0.0L);

    for (int i = 0; i < k; ++i) {
        long double x = pts[i].first;
        long double p = 1.0L;
        for (int j = 0; j < k; ++j) {
            A[i][j] = p;
            p *= x;
        }
        Y[i] = pts[i].second;
    }

    // Solve for coefficients a0..a_{k-1}
    vector<long double> a = gaussianSolve(A, Y);

    // constant term is a0
    long long secret = llround(a[0]);
    cout << "Secret (c) = " << secret << "\n";

    return 0;
}
