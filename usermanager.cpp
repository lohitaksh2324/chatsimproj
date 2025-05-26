#include "UserManager.h"
#include <QCryptographicHash>
#include <QJsonArray>
#include <QDebug>

UserManager& UserManager::instance()
{
    static UserManager instance;
    return instance;
}

UserManager::UserManager()
{
    dataFilePath = getDataFilePath();
    loadUsers();
}

UserManager::~UserManager()
{
    saveUsers();
}

QString UserManager::getDataFilePath()
{
    QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataDir);
    if (!dir.exists()) {
        dir.mkpath(appDataDir);
    }
    return appDataDir + "/users.json";
}

bool UserManager::registerUser(const QString& username, const QString& email, const QString& password)
{
    if (userExists(username)) {
        qDebug() << "User already exists:" << username;
        return false;
    }

    QJsonObject userData;
    userData["username"] = username;
    userData["email"] = email;
    userData["password"] = hashPassword(password);
    userData["created_date"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    users[username] = userData;
    saveUsers();

    qDebug() << "User registered successfully:" << username;
    return true;
}

bool UserManager::authenticateUser(const QString& username, const QString& password)
{
    if (!userExists(username)) {
        qDebug() << "User does not exist:" << username;
        return false;
    }

    QString hashedPassword = hashPassword(password);
    QString storedPassword = users[username]["password"].toString();

    bool authenticated = (hashedPassword == storedPassword);
    qDebug() << "Authentication attempt for" << username << ":" << (authenticated ? "SUCCESS" : "FAILED");

    return authenticated;
}

bool UserManager::userExists(const QString& username)
{
    return users.contains(username);
}

QString UserManager::getUserEmail(const QString& username)
{
    if (userExists(username)) {
        return users[username]["email"].toString();
    }
    return QString();
}

void UserManager::clearAllUsers()
{
    users.clear();
    saveUsers();
    qDebug() << "All users cleared";
}

void UserManager::loadUsers()
{
    QFile file(dataFilePath);
    if (!file.exists()) {
        qDebug() << "User data file does not exist, starting fresh:" << dataFilePath;
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open user data file for reading:" << dataFilePath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Failed to parse user data JSON:" << error.errorString();
        return;
    }

    QJsonObject rootObj = doc.object();
    for (auto it = rootObj.begin(); it != rootObj.end(); ++it) {
        users[it.key()] = it.value().toObject();
    }

    qDebug() << "Loaded" << users.size() << "users from" << dataFilePath;
}

void UserManager::saveUsers()
{
    QJsonObject rootObj;
    for (auto it = users.begin(); it != users.end(); ++it) {
        rootObj[it.key()] = it.value();
    }

    QJsonDocument doc(rootObj);

    QFile file(dataFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open user data file for writing:" << dataFilePath;
        return;
    }

    file.write(doc.toJson());
    file.close();

    qDebug() << "Saved" << users.size() << "users to" << dataFilePath;
}

QString UserManager::hashPassword(const QString& password)
{
    // Simple SHA-256 hash for demo purposes
    // In production, use proper password hashing with salt (bcrypt, scrypt, etc.)
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(password.toUtf8());
    return hash.result().toHex();
}
