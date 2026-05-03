/*
 * Auth.h - Authentication System
 *
 * Provides terminal-based login/signup with:
 * - File-based user storage (users.txt)
 * - Custom hash-based password storage (NOT plain text)
 * - Duplicate username detection
 * - Session management
 */

#ifndef AUTH_H
#define AUTH_H

#include <string>
#include <vector>
#include <unordered_map>

struct UserSession {
    std::string username;
    bool isLoggedIn;
    long long loginTime;

    UserSession();
};

class Auth {
private:
    std::string dataFilePath;
    std::unordered_map<std::string, std::string> userDatabase; // username -> hashed password
    UserSession currentSession;

    // Custom hash function (simple but not plain text)
    std::string hashPassword(const std::string& password);
    bool loadUsers();
    bool saveUsers();

public:
    Auth(const std::string& filePath = "data/users.txt");

    bool signup(const std::string& username, const std::string& password);
    bool login(const std::string& username, const std::string& password);
    void logout();
    bool isLoggedIn() const;
    std::string getCurrentUser() const;
    UserSession getSession() const;
    bool userExists(const std::string& username) const;
    std::vector<std::string> getAllUsers() const;

    // Interactive terminal methods
    bool interactiveLogin();
    bool interactiveSignup();
    int authMenu(); // Returns: 1=login, 2=signup, 0=exit
};

#endif // AUTH_H
