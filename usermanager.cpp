#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QStandardPaths>
#include <QDir>

class UserManager
{
public:
    static UserManager& instance();

    // User registration and authentication
    bool registerUser(const QString& username, const QString& email, const QString& password);
    bool authenticateUser(const QString& username, const QString& password);
    bool userExists(const QString& username);

    // User data management
    QString getUserEmail(const QString& username);
    void clearAllUsers(); // For testing purposes

private:
    UserManager();
    ~UserManager();

    // File operations
    void loadUsers();
    void saveUsers();
    QString getDataFilePath();

    // Password hashing (simple hash for demo - use proper encryption in production)
    QString hashPassword(const QString& password);

    QMap<QString, QJsonObject> users; // username -> user data
    QString dataFilePath;
};

#endif // USERMANAGER_H
