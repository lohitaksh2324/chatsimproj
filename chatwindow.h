#ifndef CHATWINDOW_H
#define CHATWINDOW_H
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QFrame>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>
#include <QStringList>
#include <QMap>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QUuid>
#include <QAction>


// Message struct for storing individual messages
struct Message {
    QString id;           // Unique identifier for each message
    QString sender;
    QString content;
    QDateTime timestamp;
    bool isCurrentUser;

    Message() {
        id = QUuid::createUuid().toString();
    }

    Message(const QString &s, const QString &c, const QDateTime &t, bool isCurrent)
        : sender(s), content(c), timestamp(t), isCurrentUser(isCurrent) {
        id = QUuid::createUuid().toString();
    }

    // Methods for JSON serialization
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["id"] = id;
        obj["sender"] = sender;
        obj["content"] = content;
        obj["timestamp"] = timestamp.toString(Qt::ISODate);
        obj["isCurrentUser"] = isCurrentUser;
        return obj;
    }

    static Message fromJson(const QJsonObject &obj) {
        Message msg(
            obj["sender"].toString(),
            obj["content"].toString(),
            QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate),
            obj["isCurrentUser"].toBool()
            );
        msg.id = obj["id"].toString();
        // Generate ID if not present (for backward compatibility)
        if (msg.id.isEmpty()) {
            msg.id = QUuid::createUuid().toString();
        }
        return msg;
    }
};

// Forward declaration for Contact struct
struct Contact {
    QString name;
    QString phone;
    Contact() = default;
    Contact(const QString &n, const QString &p) : name(n), phone(p) {}

    // Methods for JSON serialization
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["phone"] = phone;
        return obj;
    }

    static Contact fromJson(const QJsonObject &obj) {
        return Contact(obj["name"].toString(), obj["phone"].toString());
    }
};

// Custom widget to hold message data
class MessageWidget : public QFrame {
    Q_OBJECT
signals:
    void editRequested(const QString &messageId);
    void deleteRequested(const QString &messageId);
public:
    MessageWidget(const Message &msg, QWidget *parent = nullptr);
    const Message& getMessage() const { return message; }
    void setHighlighted(bool highlighted);
    QLabel *messageLabel;

private:
    Message message;

    QLabel *timeLabel;
    QPushButton *menuButton;
    bool isHighlighted;
    QMenu *contextMenu;
};

class ChatWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ChatWindow(const QString &currentUser, QWidget *parent = nullptr);

    ~ChatWindow();

private slots:
    void onContactSelected();
    void onSendMessage();
    void onLogout();
    void onAddContact();
    void onSearchTextChanged();
    void onClearSearch();
    void onEditMessage(const QString &messageId);
    void onDeleteMessage(const QString &messageId);

private:
    void setupUI();
    void setupContactsList();
    void setupChatArea();
    void setupSearchBar();
    void addMessage(const QString &sender, const QString &message, bool isCurrentUser);
    void addMessageWidget(const Message &msg);
    void loadSampleContacts();
    void loadChatHistory(const QString &contact);
    void addContactToList(const Contact &contact);
    void clearMessagesDisplay();
    void searchMessages(const QString &searchText);
    void highlightSearchResults(const QString &searchText);
    void clearHighlights();

    // Persistence methods
    void saveContacts();
    void loadContacts();
    void saveChats();
    void loadChats();
    QString getContactsFilePath() const;
    QString getChatsFilePath() const;

    // UI Components
    QHBoxLayout *mainLayout;
    QSplitter *splitter;

    // Contacts panel
    QFrame *contactsFrame;
    QVBoxLayout *contactsLayout;
    QLabel *contactsTitle;
    QListWidget *contactsList;
    QPushButton *logoutButton;
    QPushButton *addContactButton;

    // Chat panel
    QFrame *chatFrame;
    QVBoxLayout *chatLayout;
    QLabel *chatHeader;

    // Search bar
    QFrame *searchFrame;
    QHBoxLayout *searchLayout;
    QLineEdit *searchInput;
    QPushButton *clearSearchButton;

    QScrollArea *messagesScrollArea;
    QWidget *messagesWidget;
    QVBoxLayout *messagesLayout;
    QFrame *inputFrame;
    QHBoxLayout *inputLayout;
    QLineEdit *messageInput;
    QPushButton *sendButton;

    // Data
    QString currentUser;
    QString selectedContact;
    QList<Contact> contactsList_data;
    QMap<QString, QString> contactPhones;
    QMap<QString, QList<Message>> chatHistory; // Store chat history for each contact
    QMap<QString, MessageWidget*> messageWidgets; // Map message ID to widget
};

#endif // CHATWINDOW_H
