#include <bits/stdc++.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;

long long convertToDecimal(const string &num, int base) {
    long long result = 0;
    for (char c : num) {
        int val;
        if (isdigit(c)) val = c - '0';
        else val = toupper(c) - 'A' + 10;
        result = result * base + val;
    }
    return result;
}

// Lagrange interpolation for f(x_eval) given shares
long long lagrangeEvaluate(const vector<pair<int, long long>>& shares, long long x_eval) {
    long double result = 0;
    int n = shares.size();

    for (int i = 0; i < n; i++) {
        long double term = shares[i].second;
        for (int j = 0; j < n; j++) {
            if (i != j) {
                term *= (x_eval - shares[j].first) / (long double)(shares[i].first - shares[j].first);
            }
        }
        result += term;
    }
    return llround(result);
}

int main() {
    ifstream inFile("2.json");
    if (!inFile) {
        cerr << "Error: could not open JSON file.\n";
        return 1;
    }

    json j;
    inFile >> j;

    vector<pair<int, long long>> shares;
    int n = j["keys"]["n"];
    int k = j["keys"]["k"];

    for (auto it = j.begin(); it != j.end(); ++it) {
        if (it.key() == "keys") continue;

        int x = stoi(it.key());
        int base = stoi(it.value()["base"].get<string>());
        string value = it.value()["value"].get<string>();

        long long y = convertToDecimal(value, base);
        shares.push_back({x, y});
    }

    // Use first k shares to reconstruct the polynomial
    vector<pair<int, long long>> baseShares(shares.begin(), shares.begin() + k);

    long long secret = lagrangeEvaluate(baseShares, 0);

    // Check all shares
    vector<pair<int, long long>> wrongShares;
    for (auto& sh : shares) {
        long long expected = lagrangeEvaluate(baseShares, sh.first);
        if (expected != sh.second) {
            wrongShares.push_back(sh);
        }
    }

    cout << "Secret: " << secret << "\n";
    if (wrongShares.empty()) {
        cout << "0 wrong\n";
    } else {
        cout << "Wrong shares:\n";
        for (auto& ws : wrongShares) {
            cout << "x=" << ws.first << " y=" << ws.second << "\n";
        }
    }

    return 0;
}
