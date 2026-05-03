/*
 * Auth.cpp - Authentication System Implementation
 *
 * Custom hash uses a combination of character shifting, XOR, and
 * multiplication to produce a non-reversible hex string.
 * NOT cryptographically secure but demonstrates the concept.
 */

#include "Auth.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif

// ──────────────────────────────────────────────
// UserSession
// ──────────────────────────────────────────────
UserSession::UserSession() : username(""), isLoggedIn(false), loginTime(0) {}

// ──────────────────────────────────────────────
// Auth Constructor
// ──────────────────────────────────────────────
Auth::Auth(const std::string& filePath) : dataFilePath(filePath) {
    // Ensure data directory exists
    MKDIR("data");
    loadUsers();
}

// ──────────────────────────────────────────────
// Custom Password Hashing
// Uses polynomial rolling hash + XOR scrambling + salt
// Produces a 16-character hex string from any password
// ──────────────────────────────────────────────
std::string Auth::hashPassword(const std::string& password) {
    const unsigned long long PRIME = 31;
    const unsigned long long MOD = 1e9 + 7;
    const unsigned long long SALT = 0xDEADBEEF;

    unsigned long long hash1 = 0;
    unsigned long long hash2 = SALT;
    unsigned long long power = 1;

    for (size_t i = 0; i < password.size(); i++) {
        // Polynomial hash
        hash1 = (hash1 + (password[i] - 'a' + 1) * power) % MOD;
        power = (power * PRIME) % MOD;

        // XOR scramble hash
        hash2 ^= ((unsigned long long)password[i] << ((i % 8) * 8));
        hash2 = (hash2 * 6364136223846793005ULL + 1442695040888963407ULL);
    }

    // Combine both hashes
    unsigned long long combined = hash1 ^ (hash2 >> 16);
    combined = combined * 2654435761ULL; // Knuth's multiplicative hash

    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << combined;
    return ss.str();
}

// ──────────────────────────────────────────────
// File I/O
// Format per line: username:hashedPassword
// ──────────────────────────────────────────────
bool Auth::loadUsers() {
    userDatabase.clear();
    std::ifstream file(dataFilePath);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        size_t delimiter = line.find(':');
        if (delimiter != std::string::npos) {
            std::string username = line.substr(0, delimiter);
            std::string hashedPw = line.substr(delimiter + 1);
            userDatabase[username] = hashedPw;
        }
    }
    file.close();
    return true;
}

bool Auth::saveUsers() {
    std::ofstream file(dataFilePath, std::ios::trunc);
    if (!file.is_open()) return false;

    for (auto& pair : userDatabase) {
        file << pair.first << ":" << pair.second << "\n";
    }
    file.close();
    return true;
}

// ──────────────────────────────────────────────
// Signup
// ──────────────────────────────────────────────
bool Auth::signup(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) {
        std::cout << "[ERROR] Username and password cannot be empty.\n";
        return false;
    }

    if (username.find(':') != std::string::npos) {
        std::cout << "[ERROR] Username cannot contain ':' character.\n";
        return false;
    }

    if (password.size() < 4) {
        std::cout << "[ERROR] Password must be at least 4 characters.\n";
        return false;
    }

    if (userExists(username)) {
        std::cout << "[ERROR] Username '" << username << "' already exists.\n";
        return false;
    }

    std::string hashedPw = hashPassword(password);
    userDatabase[username] = hashedPw;
    saveUsers();

    std::cout << "[SUCCESS] Account created for '" << username << "'!\n";
    return true;
}

// ──────────────────────────────────────────────
// Login
// ──────────────────────────────────────────────
bool Auth::login(const std::string& username, const std::string& password) {
    if (!userExists(username)) {
        std::cout << "[ERROR] User '" << username << "' not found.\n";
        return false;
    }

    std::string hashedPw = hashPassword(password);
    if (userDatabase[username] != hashedPw) {
        std::cout << "[ERROR] Incorrect password.\n";
        return false;
    }

    currentSession.username = username;
    currentSession.isLoggedIn = true;
    currentSession.loginTime = (long long)time(nullptr);

    std::cout << "[SUCCESS] Welcome back, " << username << "!\n";
    return true;
}

void Auth::logout() {
    std::cout << "[INFO] Logging out " << currentSession.username << "...\n";
    currentSession.username = "";
    currentSession.isLoggedIn = false;
    currentSession.loginTime = 0;
}

bool Auth::isLoggedIn() const {
    return currentSession.isLoggedIn;
}

std::string Auth::getCurrentUser() const {
    return currentSession.username;
}

UserSession Auth::getSession() const {
    return currentSession;
}

bool Auth::userExists(const std::string& username) const {
    return userDatabase.find(username) != userDatabase.end();
}

std::vector<std::string> Auth::getAllUsers() const {
    std::vector<std::string> users;
    for (auto& pair : userDatabase)
        users.push_back(pair.first);
    return users;
}

// ──────────────────────────────────────────────
// Interactive Terminal Interface
// ──────────────────────────────────────────────
bool Auth::interactiveLogin() {
    std::string username, password;
    std::cout << "\n===================================\n";
    std::cout << "          USER LOGIN\n";
    std::cout << "===================================\n";
    std::cout << "  Username: ";
    std::getline(std::cin, username);
    std::cout << "  Password: ";
    std::getline(std::cin, password);
    return login(username, password);
}

bool Auth::interactiveSignup() {
    std::string username, password, confirmPw;
    std::cout << "\n===================================\n";
    std::cout << "        CREATE ACCOUNT\n";
    std::cout << "===================================\n";
    std::cout << "  Username: ";
    std::getline(std::cin, username);
    std::cout << "  Password (min 4 chars): ";
    std::getline(std::cin, password);
    std::cout << "  Confirm Password: ";
    std::getline(std::cin, confirmPw);

    if (password != confirmPw) {
        std::cout << "[ERROR] Passwords do not match.\n";
        return false;
    }

    return signup(username, password);
}

int Auth::authMenu() {
    std::cout << "\n==============================================\n";
    std::cout << "   ADAPTIVE MULTI-GAME LEADERBOARD SYSTEM\n";
    std::cout << "          Advanced DSA Edition\n";
    std::cout << "==============================================\n";
    std::cout << "  1. Login\n";
    std::cout << "  2. Sign Up\n";
    std::cout << "  0. Exit\n";
    std::cout << "==============================================\n";
    std::cout << "  Choice: ";

    std::string input;
    std::getline(std::cin, input);
    if (input == "1") return 1;
    if (input == "2") return 2;
    return 0;
}
