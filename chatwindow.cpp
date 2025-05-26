#include "ChatWindow.h"
#include "AddContactDialog.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QDateTime>
#include <QScrollBar>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>
#include <QTimer>
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QMenu>
#include <QInputDialog>
#include <QClipboard>

// MessageWidget Implementation
MessageWidget::MessageWidget(const Message &msg, QWidget *parent)
    : QFrame(parent), message(msg), isHighlighted(false) {
    setMaximumWidth(600);

    QString timeString = msg.timestamp.toString("hh:mm");

    if (msg.isCurrentUser) {
        setStyleSheet(
            "QFrame {"
            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
            "        stop:0 #667eea, stop:1 #764ba2);"
            "    border-radius: 18px;"
            "    border-bottom-right-radius: 6px;"
            "    padding: 12px 16px;"
            "    margin: 4px 0px 4px 80px;"
            "}"
            );
    } else {
        setStyleSheet(
            "QFrame {"
            "    background: #e9ecef;"
            "    border-radius: 18px;"
            "    border-bottom-left-radius: 6px;"
            "    padding: 12px 16px;"
            "    margin: 4px 80px 4px 0px;"
            "}"
            );
    }

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    messageLabel = new QLabel(msg.content);
    messageLabel->setWordWrap(true);
    messageLabel->setStyleSheet(
        QString("QLabel { color: %1; font-size: 14px; background: transparent; }")
            .arg(msg.isCurrentUser ? "white" : "#495057")
        );

    timeLabel = new QLabel(timeString);
    timeLabel->setAlignment(msg.isCurrentUser ? Qt::AlignRight : Qt::AlignLeft);
    timeLabel->setStyleSheet(
        QString("QLabel { color: %1; font-size: 11px; background: transparent; }")
            .arg(msg.isCurrentUser ? "rgba(255,255,255,0.8)" : "#6c757d")
        );

    // Add menu button only for current user messages
    if (msg.isCurrentUser) {
        menuButton = new QPushButton("⋮");
        menuButton->setFixedSize(25, 25);
        menuButton->setStyleSheet(
            "QPushButton {"
            "    background: rgba(255, 255, 255, 0.3);"
            "    border: none;"
            "    border-radius: 12px;"
            "    color: white;"
            "    font-size: 14px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background: rgba(255, 255, 255, 0.5);"
            "}"
            );

        // Create context menu
        contextMenu = new QMenu(this);
        contextMenu->setStyleSheet(
            "QMenu {"
            "    background-color: white;"
            "    border: 1px solid #e9ecef;"
            "    border-radius: 8px;"
            "    padding: 5px 0px;"
            "}"
            "QMenu::item {"
            "    padding: 8px 20px;"
            "    color: #495057;"
            "}"
            "QMenu::item:selected {"
            "    background-color: #f8f9fa;"
            "}"
            );

        // Add menu actions
        QAction *editAction = contextMenu->addAction("Edit");
        QAction *deleteAction = contextMenu->addAction("Delete");
        QAction *copyAction = contextMenu->addAction("Copy");

        // Connect menu actions to slots
        connect(editAction, &QAction::triggered, [this]() {
            emit editRequested(message.id);
        });

        connect(deleteAction, &QAction::triggered, [this]() {
            emit deleteRequested(message.id);
        });

        connect(copyAction, &QAction::triggered, [this]() {
            QApplication::clipboard()->setText(message.content);
        });

        // Connect menu button to show context menu
        connect(menuButton, &QPushButton::clicked, [this]() {
            QPoint globalPos = menuButton->mapToGlobal(QPoint(0, menuButton->height()));
            contextMenu->exec(globalPos);
        });

        // Create a horizontal layout for time and menu button
        QHBoxLayout *bottomLayout = new QHBoxLayout();
        bottomLayout->setContentsMargins(0, 0, 0, 0);
        bottomLayout->addWidget(timeLabel);
        bottomLayout->addWidget(menuButton);

        layout->addWidget(messageLabel);
        layout->addLayout(bottomLayout);
    } else {
        menuButton = nullptr;
        contextMenu = nullptr;
        layout->addWidget(messageLabel);
        layout->addWidget(timeLabel);
    }
}

void MessageWidget::setHighlighted(bool highlighted) {
    isHighlighted = highlighted;

    if (highlighted) {
        if (message.isCurrentUser) {
            setStyleSheet(
                "QFrame {"
                "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
                "        stop:0 #ffd700, stop:1 #ff8c00);"
                "    border: 2px solid #ff6347;"
                "    border-radius: 18px;"
                "    border-bottom-right-radius: 6px;"
                "    padding: 12px 16px;"
                "    margin: 4px 0px 4px 80px;"
                "}"
                );

            if (menuButton) {
                menuButton->setStyleSheet(
                    "QPushButton {"
                    "    background: rgba(0, 0, 0, 0.3);"
                    "    border: none;"
                    "    border-radius: 12px;"  // Updated radius
                    "    color: black;"
                    "    font-size: 14px;"      // Updated font size
                    "    font-weight: bold;"
                    "}"
                    "QPushButton:hover {"
                    "    background: rgba(0, 0, 0, 0.5);"
                    "}"
                    );
            }
        } else {
            setStyleSheet(
                "QFrame {"
                "    background: #ffff99;"
                "    border: 2px solid #ff6347;"
                "    border-radius: 18px;"
                "    border-bottom-left-radius: 6px;"
                "    padding: 12px 16px;"
                "    margin: 4px 80px 4px 0px;"
                "}"
                );
        }
    } else {
        // Restore original styling
        if (message.isCurrentUser) {
            setStyleSheet(
                "QFrame {"
                "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
                "        stop:0 #667eea, stop:1 #764ba2);"
                "    border-radius: 18px;"
                "    border-bottom-right-radius: 6px;"
                "    padding: 12px 16px;"
                "    margin: 4px 0px 4px 80px;"
                "}"
                );

            if (menuButton) {
                menuButton->setStyleSheet(
                    "QPushButton {"
                    "    background: rgba(255, 255, 255, 0.3);"
                    "    border: none;"
                    "    border-radius: 12px;"  // Updated radius
                    "    color: white;"
                    "    font-size: 14px;"      // Updated font size
                    "    font-weight: bold;"
                    "}"
                    "QPushButton:hover {"
                    "    background: rgba(255, 255, 255, 0.5);"
                    "}"
                    );
            }
        } else {
            setStyleSheet(
                "QFrame {"
                "    background: #e9ecef;"
                "    border-radius: 18px;"
                "    border-bottom-left-radius: 6px;"
                "    padding: 12px 16px;"
                "    margin: 4px 80px 4px 0px;"
                "}"
                );
        }
    }
}

// ChatWindow Implementation
ChatWindow::ChatWindow(const QString &currentUser, QWidget *parent)
    : QWidget(parent), currentUser(currentUser), selectedContact("")
{
    setWindowTitle(QString("Chat - %1").arg(currentUser));
    setMinimumSize(1200, 800);

    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint |
                   Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

    setAttribute(Qt::WA_ShowWithoutActivating, false);
    setAttribute(Qt::WA_DeleteOnClose, true);

    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    resize(screenGeometry.width() * 0.9, screenGeometry.height() * 0.9);

    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    setupUI();
    loadContacts();
    loadChats();
    setupAutoMessages();
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        trayIcon = new QSystemTrayIcon(this);
        trayIcon->setIcon(QIcon(":/icons/chat.png")); // You can use any icon
        trayIcon->show();
    } else {
        trayIcon = nullptr;
    }

    qDebug() << "ChatWindow constructor completed";
}

ChatWindow::~ChatWindow()
{
    if (autoMessageTimer) {
        autoMessageTimer->stop();
    }
    if (trayIcon) {
        trayIcon->hide();
    }
    saveContacts();
    saveChats();
}

void ChatWindow::setupUI()
{
    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setChildrenCollapsible(false);

    setupContactsList();
    setupChatArea();

    splitter->addWidget(contactsFrame);
    splitter->addWidget(chatFrame);
    splitter->setSizes({240, 960});
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter);

    setStyleSheet(
        "QWidget {"
        "    background-color: #f8f9fa;"
        "    font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
        "QSplitter::handle {"
        "    background-color: #dee2e6;"
        "    width: 2px;"
        "}"
        "QSplitter::handle:hover {"
        "    background-color: #667eea;"
        "}"
        );
}

void ChatWindow::setupContactsList()
{
    contactsFrame = new QFrame();
    contactsFrame->setFixedWidth(300);
    contactsFrame->setStyleSheet(
        "QFrame {"
        "    background-color: #ffffff;"
        "    border-right: 1px solid #e9ecef;"
        "}"
        );

    contactsLayout = new QVBoxLayout(contactsFrame);
    contactsLayout->setContentsMargins(0, 0, 0, 0);
    contactsLayout->setSpacing(0);

    // Header with user info, profile button, and logout
    QFrame *headerFrame = new QFrame();
    headerFrame->setFixedHeight(80);
    headerFrame->setStyleSheet(
        "QFrame {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "    border: none;"
        "}"
        );

    QVBoxLayout *headerLayout = new QVBoxLayout(headerFrame);
    headerLayout->setContentsMargins(20, 15, 20, 15);

    QLabel *userLabel = new QLabel(QString("👋 %1").arg(currentUser));
    userLabel->setStyleSheet(
        "QLabel {"
        "    color: white;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "}"
        );

    // Profile button
    profileButton = new QPushButton("👤");
    profileButton->setFixedSize(30, 30);
    profileButton->setToolTip("View Profile");
    profileButton->setStyleSheet(
        "QPushButton {"
        "    background: rgba(255, 255, 255, 0.2);"
        "    border: 1px solid rgba(255, 255, 255, 0.3);"
        "    border-radius: 15px;"
        "    color: white;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background: rgba(255, 255, 255, 0.3);"
        "}"
        "QPushButton:pressed {"
        "    background: rgba(255, 255, 255, 0.1);"
        "}"
        );

    logoutButton = new QPushButton("Logout");
    logoutButton->setFixedSize(60, 25);
    logoutButton->setStyleSheet(
        "QPushButton {"
        "    background: rgba(255, 255, 255, 0.2);"
        "    border: 1px solid rgba(255, 255, 255, 0.3);"
        "    border-radius: 12px;"
        "    color: white;"
        "    font-size: 12px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: rgba(255, 255, 255, 0.3);"
        "}"
        "QPushButton:pressed {"
        "    background: rgba(255, 255, 255, 0.1);"
        "}"
        );

    QHBoxLayout *userLayout = new QHBoxLayout();
    userLayout->addWidget(userLabel);
    userLayout->addStretch();
    userLayout->addWidget(profileButton);
    userLayout->addWidget(logoutButton);

    headerLayout->addLayout(userLayout);

    // Contacts title with add button
    QFrame *titleFrame = new QFrame();
    titleFrame->setStyleSheet("background: transparent;");
    QHBoxLayout *titleLayout = new QHBoxLayout(titleFrame);
    titleLayout->setContentsMargins(20, 15, 20, 10);
    titleLayout->setSpacing(10);

    contactsTitle = new QLabel("💬 Contacts");
    contactsTitle->setStyleSheet(
        "QLabel {"
        "    color: #495057;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    background: transparent;"
        "}"
        );

    addContactButton = new QPushButton("➕");
    addContactButton->setFixedSize(30, 30);
    addContactButton->setToolTip("Add New Contact");
    addContactButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "    border: none;"
        "    border-radius: 15px;"
        "    color: white;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #5a67d8, stop:1 #6b46c1);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #4c51bf, stop:1 #553c9a);"
        "}"
        );

    titleLayout->addWidget(contactsTitle);
    titleLayout->addStretch();
    titleLayout->addWidget(addContactButton);

    // Contacts list
    contactsList = new QListWidget();
    contactsList->setStyleSheet(
        "QListWidget {"
        "    background: transparent;"
        "    border: none;"
        "    outline: none;"
        "}"
        "QListWidget::item {"
        "    padding: 15px 20px;"
        "    border-bottom: 1px solid #f1f3f4;"
        "    color: #495057;"
        "    font-size: 14px;"
        "}"
        "QListWidget::item:hover {"
        "    background-color: rgba(102, 126, 234, 0.08);"
        "}"
        "QListWidget::item:selected {"
        "    background-color: rgba(102, 126, 234, 0.15);"
        "    color: #667eea;"
        "    font-weight: bold;"
        "}"
        );

    // ENABLE CONTEXT MENU FOR CONTACTS LIST
    contactsList->setContextMenuPolicy(Qt::CustomContextMenu);

    // CREATE CONTEXT MENU FOR EDIT/DELETE
    contactContextMenu = new QMenu(this);
    contactContextMenu->setStyleSheet(
        "QMenu {"
        "    background-color: white;"
        "    border: 1px solid #e9ecef;"
        "    border-radius: 8px;"
        "    padding: 5px 0px;"
        "}"
        "QMenu::item {"
        "    padding: 8px 20px;"
        "    color: #495057;"
        "}"
        "QMenu::item:selected {"
        "    background-color: #f8f9fa;"
        "}"
        );

    QAction *editAction = contactContextMenu->addAction("✏️ Edit Contact");
    QAction *deleteAction = contactContextMenu->addAction("🗑️ Delete Contact");

    contactsLayout->addWidget(headerFrame);
    contactsLayout->addWidget(titleFrame);
    contactsLayout->addWidget(contactsList);

    // CONNECT ALL SIGNALS
    connect(contactsList, &QListWidget::itemClicked, this, &ChatWindow::onContactSelected);
    connect(logoutButton, &QPushButton::clicked, this, &ChatWindow::onLogout);
    connect(addContactButton, &QPushButton::clicked, this, &ChatWindow::onAddContact);
    connect(profileButton, &QPushButton::clicked, this, &ChatWindow::onProfileClicked);

    // NEW CONNECTIONS FOR EDIT/DELETE FUNCTIONALITY
    connect(contactsList, &QListWidget::customContextMenuRequested,
            this, &ChatWindow::onContactRightClicked);
    connect(editAction, &QAction::triggered, this, &ChatWindow::onEditContact);
    connect(deleteAction, &QAction::triggered, this, &ChatWindow::onDeleteContact);
}

void ChatWindow::setupChatArea()
{
    chatFrame = new QFrame();
    chatFrame->setStyleSheet(
        "QFrame {"
        "    background-color: #ffffff;"
        "}"
        );

    chatLayout = new QVBoxLayout(chatFrame);
    chatLayout->setContentsMargins(0, 0, 0, 0);
    chatLayout->setSpacing(0);

    // Chat header
    chatHeader = new QLabel("Select a contact to start chatting");
    chatHeader->setFixedHeight(80);
    chatHeader->setAlignment(Qt::AlignCenter);
    chatHeader->setStyleSheet(
        "QLabel {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #f8f9fa, stop:1 #e9ecef);"
        "    color: #6c757d;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    border-bottom: 2px solid #e9ecef;"
        "    padding: 0 30px;"
        "}"
        );

    setupSearchBar();

    // Messages area
    messagesScrollArea = new QScrollArea();
    messagesScrollArea->setWidgetResizable(true);
    messagesScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    messagesScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    messagesScrollArea->setStyleSheet(
        "QScrollArea {"
        "    border: none;"
        "    background: #f8f9fa;"
        "}"
        "QScrollBar:vertical {"
        "    background: #f1f3f4;"
        "    width: 8px;"
        "    border-radius: 4px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: #ced4da;"
        "    border-radius: 4px;"
        "    min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background: #adb5bd;"
        "}"
        );

    messagesWidget = new QWidget();
    messagesWidget->setStyleSheet("background: transparent;");
    messagesLayout = new QVBoxLayout(messagesWidget);
    messagesLayout->setContentsMargins(20, 20, 20, 20);
    messagesLayout->setSpacing(15);
    messagesLayout->addStretch();

    messagesScrollArea->setWidget(messagesWidget);

    // Input area
    inputFrame = new QFrame();
    inputFrame->setFixedHeight(80);
    inputFrame->setStyleSheet(
        "QFrame {"
        "    background: #ffffff;"
        "    border-top: 2px solid #e9ecef;"
        "}"
        );

    inputLayout = new QHBoxLayout(inputFrame);
    inputLayout->setContentsMargins(20, 15, 20, 15);
    inputLayout->setSpacing(15);

    messageInput = new QLineEdit();
    messageInput->setPlaceholderText("Type your message here...");
    messageInput->setFixedHeight(50);
    messageInput->setStyleSheet(
        "QLineEdit {"
        "    border: 2px solid #e9ecef;"
        "    border-radius: 25px;"
        "    padding: 12px 20px;"
        "    font-size: 14px;"
        "    background: #f8f9fa;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #667eea;"
        "    background: #ffffff;"
        "}"
        );

    sendButton = new QPushButton("Send");
    sendButton->setFixedSize(80, 50);
    sendButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "    border: none;"
        "    border-radius: 25px;"
        "    color: white;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #5a67d8, stop:1 #6b46c1);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #4c51bf, stop:1 #553c9a);"
        "}"
        "QPushButton:disabled {"
        "    background: #ced4da;"
        "    color: #6c757d;"
        "}"
        );

    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);

    messageInput->setEnabled(false);
    sendButton->setEnabled(false);

    chatLayout->addWidget(chatHeader);
    chatLayout->addWidget(searchFrame);
    chatLayout->addWidget(messagesScrollArea);
    chatLayout->addWidget(inputFrame);

    connect(messageInput, &QLineEdit::returnPressed, this, &ChatWindow::onSendMessage);
    connect(sendButton, &QPushButton::clicked, this, &ChatWindow::onSendMessage);
}

void ChatWindow::setupSearchBar()
{
    searchFrame = new QFrame();
    searchFrame->setFixedHeight(60);
    searchFrame->setStyleSheet(
        "QFrame {"
        "    background: #ffffff;"
        "    border-bottom: 1px solid #e9ecef;"
        "}"
        );
    searchFrame->hide(); // Hidden by default

    searchLayout = new QHBoxLayout(searchFrame);
    searchLayout->setContentsMargins(20, 10, 20, 10);
    searchLayout->setSpacing(10);

    QLabel *searchLabel = new QLabel("🔍");
    searchLabel->setStyleSheet("font-size: 16px;");

    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Search messages...");
    searchInput->setStyleSheet(
        "QLineEdit {"
        "    border: 2px solid #e9ecef;"
        "    border-radius: 20px;"
        "    padding: 8px 15px;"
        "    font-size: 14px;"
        "    background: #f8f9fa;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #667eea;"
        "    background: #ffffff;"
        "}"
        );

    clearSearchButton = new QPushButton("✖");
    clearSearchButton->setFixedSize(30, 30);
    clearSearchButton->setToolTip("Clear Search");
    clearSearchButton->setStyleSheet(
        "QPushButton {"
        "    background: #dc3545;"
        "    border: none;"
        "    border-radius: 15px;"
        "    color: white;"
        "    font-size: 12px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: #c82333;"
        "}"
        "QPushButton:pressed {"
        "    background: #bd2130;"
        "}"
        );

    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(clearSearchButton);

    connect(searchInput, &QLineEdit::textChanged, this, &ChatWindow::onSearchTextChanged);
    connect(clearSearchButton, &QPushButton::clicked, this, &ChatWindow::onClearSearch);
    connect(searchInput, &QLineEdit::returnPressed, this, &ChatWindow::onSearchEnterPressed);
}

void ChatWindow::onContactSelected()
{
    QListWidgetItem *item = contactsList->currentItem();
    if (!item) return;

    // Extract contact name from display text (remove unread count if present)
    QString contactName = item->text().split('\n')[0];
    if (contactName.contains(" (") && contactName.endsWith(")")) {
        contactName = contactName.split(" (")[0];
    }

    selectedContact = contactName;

    qDebug() << "=== onContactSelected() called for:" << selectedContact << "===";
    qDebug() << "Chat history exists:" << chatHistory.contains(selectedContact);
    qDebug() << "Chat history size:" << chatHistory[selectedContact].size();

    // Clear unread count when selecting contact
    if (unreadCounts.value(selectedContact, 0) > 0) {
        unreadCounts[selectedContact] = 0;
        updateContactUnreadCount(selectedContact, 0);
    }

    // Update chat header
    chatHeader->setText(QString("💬 Chat with %1").arg(selectedContact));
    chatHeader->setStyleSheet(
        "QLabel {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "    color: white;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    border-bottom: 2px solid #e9ecef;"
        "    padding: 0 30px;"
        "}"
        );

    // Show search bar when a contact is selected
    searchFrame->show();

    // Enable input controls
    messageInput->setEnabled(true);
    sendButton->setEnabled(true);

    // Load and display chat history
    loadChatHistory(selectedContact);
}


void ChatWindow::onSendMessage()
{
    QString message = messageInput->text().trimmed();
    if (message.isEmpty() || selectedContact.isEmpty()) return;

    addMessage(currentUser, message, true);
    messageInput->clear();

    // Simulate response after a short delay
    QTimer::singleShot(1000 + QRandomGenerator::global()->bounded(2000), [this]() {
        QStringList responses = {
            "That's interesting! Tell me more. 🤔",
            "I see what you mean. 👀",
            "Thanks for sharing that! 🙏",
            "How do you feel about that? 🧠",
            "That sounds great! 😄",
            "I understand your point. 👍",
            "What do you think we should do next? 🤷",
            "That's a good question. 🤨",
            "Wow, really? 😲",
            "Haha, that's funny! 😂",
            "Can you explain that a bit more? 🧐",
            "Hmm, let me think about that. 🤔",
            "I'm here for you. 💬",
            "Interesting perspective. 🧩",
            "That makes sense. ✅",
            "I'm not sure I follow. Could you elaborate? 🤯",
            "Exactly! I was thinking the same. 💡",
            "That's one way to look at it. 👓",
            "Good point! 📌",
            "I hadn’t thought of it that way. 🔄",
            "You might be onto something. 🕵️",
            "I'm curious—what made you think of that? 🧐",
            "Could you give me an example? 📘",
            "I appreciate your insight. 🌟",
            "Let’s look at it from another angle. 🔍",
            "Fascinating idea. Tell me more. 🧠",
            "You’re making me think! 🧠💭",
            "Let's explore that further. 🚀",
            "That caught my attention. 👂",
            "I like where this is going. 😎",
            "You're absolutely right! ✅",
            "This is getting interesting. 👀",
            "Hmm, that's debatable. 🤨",
            "Now that's something to consider. 🧐",
            "Intriguing thought. 🧠✨",
            "Can you clarify that a little? 🤓",
            "Let me make sure I got that right. 📝",
            "Totally! Couldn't agree more. 🙌",
            "That's a bold statement! 🔥",
            "Tell me why you think that. 🗣️",
            "I hear you loud and clear. 🔊",
            "That's deep. 💭",
            "You're raising some great points. 👏",
            "Let's dive deeper into that. 🌊",
            "Well said! 🎯",
            "That perspective is refreshing. 🌿",
            "You’ve clearly thought about this. 🤓",
            "I like how you put that. ✍️",
            "That reminds me of something... 💭",
            "Go on, I'm listening. 🎧"
        };
        QString response = responses[QRandomGenerator::global()->bounded(responses.size())];
        addMessage(selectedContact, response, false);
    });
}

void ChatWindow::onProfileClicked()
{
    UserManager &userMgr = UserManager::instance();
    QString email = userMgr.getUserEmail(currentUser);

    QDialog *profileDialog = new QDialog(this);
    profileDialog->setWindowTitle("User Profile");
    profileDialog->setFixedSize(350, 200);
    profileDialog->setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(profileDialog);
    layout->setSpacing(15);
    layout->setContentsMargins(30, 30, 30, 30);

    // Profile icon and title
    QLabel *titleLabel = new QLabel("👤 User Profile");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    color: #667eea;"
        "    margin-bottom: 10px;"
        "}"
        );

    // User info
    QLabel *nameLabel = new QLabel(QString("Name: %1").arg(currentUser));
    nameLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 14px;"
        "    color: #495057;"
        "    padding: 5px;"
        "}"
        );

    QLabel *emailLabel = new QLabel(QString("Email: %1").arg(email));
    emailLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 14px;"
        "    color: #495057;"
        "    padding: 5px;"
        "}"
        );

    // Close button
    QPushButton *closeButton = new QPushButton("Close");
    closeButton->setFixedHeight(35);
    closeButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "    border: none;"
        "    border-radius: 17px;"
        "    color: white;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #5a67d8, stop:1 #6b46c1);"
        "}"
        );

    layout->addWidget(titleLabel);
    layout->addWidget(nameLabel);
    layout->addWidget(emailLabel);
    layout->addStretch();
    layout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, profileDialog, &QDialog::accept);

    profileDialog->exec();
    profileDialog->deleteLater();
}

void ChatWindow::onLogout()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Logout", "Are you sure you want to logout?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        emit qApp->quit();
    }
}

void ChatWindow::onAddContact()
{
    AddContactDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Contact newContact = dialog.getContact();

        // Check if contact already exists
        for (const Contact &contact : contactsList_data) {
            if (contact.name.toLower() == newContact.name.toLower()) {
                QMessageBox::warning(this, "Duplicate Contact",
                                     "A contact with this name already exists!");
                return;
            }
        }

        contactsList_data.append(newContact);
        addContactToList(newContact);
        saveContacts();
    }
}

void ChatWindow::onSearchTextChanged()
{
    QString searchText = searchInput->text().trimmed();
    if (searchText.isEmpty()) {
        clearHighlights();
    } else {
        searchMessages(searchText);
    }
}

void ChatWindow::onClearSearch()
{
    searchInput->clear();
    clearHighlights();
}

void ChatWindow::addMessage(const QString &sender, const QString &content, bool isCurrentUser)
{
    if (selectedContact.isEmpty()) return;

    Message msg(sender, content, QDateTime::currentDateTime(), isCurrentUser);

    // Add to chat history
    chatHistory[selectedContact].append(msg);

    // Add to UI immediately since this is for the selected contact
    addMessageWidget(msg);

    // Save to file
    saveChats();

    qDebug() << "Message added and displayed for selected contact:" << selectedContact;
}


void ChatWindow::addMessageWidget(const Message &msg)
{
    qDebug() << "=== addMessageWidget() called ===";
    qDebug() << "Creating widget for message:" << msg.content.left(30) << "...";
    qDebug() << "Message ID:" << msg.id;
    qDebug() << "Current widget count:" << messageWidgets.size();

    MessageWidget *messageWidget = new MessageWidget(msg, messagesWidget);

    // Store reference for later updates
    messageWidgets[msg.id] = messageWidget;

    // Connect to edit/delete signals
    connect(messageWidget, &MessageWidget::editRequested, this, &ChatWindow::onEditMessage);
    connect(messageWidget, &MessageWidget::deleteRequested, this, &ChatWindow::onDeleteMessage);

    // Insert before the stretch (should be last item)
    int insertIndex = messagesLayout->count() - 1;
    if (insertIndex < 0) insertIndex = 0;

    messagesLayout->insertWidget(insertIndex, messageWidget);

    qDebug() << "Widget inserted at index:" << insertIndex;
    qDebug() << "New widget count:" << messageWidgets.size();
    qDebug() << "Layout count:" << messagesLayout->count();

    // Force widget to show
    messageWidget->show();
    messageWidget->update();

    // Force layout update
    messagesWidget->updateGeometry();
    messagesScrollArea->updateGeometry();

    // Auto-scroll to bottom with delay
    QTimer::singleShot(50, [this]() {
        QScrollBar *scrollBar = messagesScrollArea->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
        qDebug() << "Auto-scrolled to bottom";
    });
}

void ChatWindow::loadChatHistory(const QString &contact)
{
    qDebug() << "=== loadChatHistory() called for:" << contact << "===";

    // Clear existing message widgets first
    clearMessagesDisplay();

    qDebug() << "Cleared existing widgets. Widget count now:" << messageWidgets.size();

    // Check if we have chat history for this contact
    if (chatHistory.contains(contact)) {
        const QList<Message> &messages = chatHistory[contact];
        qDebug() << "Found" << messages.size() << "messages in history";

        // Add each message widget
        for (int i = 0; i < messages.size(); ++i) {
            const Message &msg = messages[i];
            qDebug() << "Adding message" << i << ":" << msg.content.left(20) << "...";

            MessageWidget *messageWidget = new MessageWidget(msg, messagesWidget);

            // Store reference for later updates
            messageWidgets[msg.id] = messageWidget;

            // Connect to edit/delete signals
            connect(messageWidget, &MessageWidget::editRequested, this, &ChatWindow::onEditMessage);
            connect(messageWidget, &MessageWidget::deleteRequested, this, &ChatWindow::onDeleteMessage);

            // Insert before the stretch (which should be the last item)
            messagesLayout->insertWidget(messagesLayout->count() - 1, messageWidget);

            qDebug() << "Widget added. Layout count now:" << messagesLayout->count();
        }

        qDebug() << "Total message widgets created:" << messageWidgets.size();
    } else {
        qDebug() << "No chat history found for contact:" << contact;
    }

    // Force update and scroll to bottom
    messagesWidget->updateGeometry();
    messagesScrollArea->updateGeometry();

    // Scroll to bottom with delay to ensure widgets are rendered
    QTimer::singleShot(100, [this]() {
        QScrollBar *scrollBar = messagesScrollArea->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
        qDebug() << "Scrolled to bottom. Max:" << scrollBar->maximum() << "Current:" << scrollBar->value();
    });
}



void ChatWindow::addContactToList(const Contact &contact)
{
    QString displayText = QString("%1\n📞 %2").arg(contact.name, contact.phone);
    contactsList->addItem(displayText);
    contactPhones[contact.name] = contact.phone;
}

void ChatWindow::clearMessagesDisplay()
{
    qDebug() << "=== clearMessagesDisplay() called ===";
    qDebug() << "Current widget count:" << messageWidgets.size();

    // Remove all message widgets from layout and delete them
    QList<QString> keysToRemove;
    for (auto it = messageWidgets.begin(); it != messageWidgets.end(); ++it) {
        MessageWidget *widget = it.value();
        messagesLayout->removeWidget(widget);
        widget->deleteLater();
        keysToRemove.append(it.key());
    }

    // Clear the map
    for (const QString &key : keysToRemove) {
        messageWidgets.remove(key);
    }

    qDebug() << "Cleared all widgets. New count:" << messageWidgets.size();
    qDebug() << "Layout count after clear:" << messagesLayout->count();
}

void ChatWindow::searchMessages(const QString &searchText)
{
    clearHighlights();

    if (searchText.isEmpty() || selectedContact.isEmpty()) return;

    highlightSearchResults(searchText);
}

void ChatWindow::highlightSearchResults(const QString &searchText)
{
    if (searchText.isEmpty()) return;

    for (auto it = messageWidgets.begin(); it != messageWidgets.end(); ++it) {
        MessageWidget *widget = it.value();
        const Message &msg = widget->getMessage();

        if (msg.content.contains(searchText, Qt::CaseInsensitive)) {
            widget->setHighlighted(true);
        }
    }
}

void ChatWindow::clearHighlights()
{
    for (auto it = messageWidgets.begin(); it != messageWidgets.end(); ++it) {
        MessageWidget *widget = it.value();
        widget->setHighlighted(false);
    }
}

void ChatWindow::saveContacts()
{
    QString filePath = getContactsFilePath();
    QJsonArray contactsArray;

    for (const Contact &contact : contactsList_data) {
        contactsArray.append(contact.toJson());
    }

    QJsonDocument doc(contactsArray);

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
    }
}

void ChatWindow::onEditMessage(const QString &messageId)
{
    if (selectedContact.isEmpty()) return;

    // Find the message in chat history
    QList<Message> &messages = chatHistory[selectedContact];
    for (int i = 0; i < messages.size(); ++i) {
        if (messages[i].id == messageId) {
            // Get new text from user
            bool ok;
            QString newText = QInputDialog::getText(this, "Edit Message",
                                                    "Edit your message:",
                                                    QLineEdit::Normal,
                                                    messages[i].content, &ok);
            if (ok && !newText.trimmed().isEmpty()) {
                // Update message
                messages[i].content = newText.trimmed();

                // Update widget
                MessageWidget *widget = messageWidgets[messageId];
                widget->messageLabel->setText(newText.trimmed());

                // Save changes
                saveChats();
            }
            break;
        }
    }
}
void ChatWindow::onContactRightClicked(const QPoint &position)
{
    QListWidgetItem *item = contactsList->itemAt(position);
    if (!item) return;

    // Extract contact name from the display text
    rightClickedContact = item->text().split('\n')[0];

    // Show context menu at cursor position
    contactContextMenu->exec(contactsList->mapToGlobal(position));
}

void ChatWindow::onEditContact()
{
    if (rightClickedContact.isEmpty()) return;

    // Find the contact in our data
    Contact *contactToEdit = nullptr;
    int contactIndex = -1;

    for (int i = 0; i < contactsList_data.size(); ++i) {
        if (contactsList_data[i].name == rightClickedContact) {
            contactToEdit = &contactsList_data[i];
            contactIndex = i;
            break;
        }
    }

    if (!contactToEdit) return;

    // Create edit dialog
    AddContactDialog dialog(this);
    dialog.setWindowTitle("Edit Contact");
    dialog.setContactData(contactToEdit->name, contactToEdit->phone);

    if (dialog.exec() == QDialog::Accepted) {
        Contact updatedContact = dialog.getContact();

        // Check if new name conflicts with existing contacts (except current one)
        for (int i = 0; i < contactsList_data.size(); ++i) {
            if (i != contactIndex &&
                contactsList_data[i].name.toLower() == updatedContact.name.toLower()) {
                QMessageBox::warning(this, "Duplicate Contact",
                                     "A contact with this name already exists!");
                return;
            }
        }

        QString oldName = contactToEdit->name;

        // Update contact data
        *contactToEdit = updatedContact;

        // Update chat history if contact name changed
        if (oldName != updatedContact.name && chatHistory.contains(oldName)) {
            chatHistory[updatedContact.name] = chatHistory[oldName];
            chatHistory.remove(oldName);
        }

        // Update selected contact if it's the one being edited
        if (selectedContact == oldName) {
            selectedContact = updatedContact.name;
            chatHeader->setText(QString("💬 Chat with %1").arg(selectedContact));
        }

        // Refresh contacts list
        refreshContactsList();
        saveContacts();
        saveChats();

        QMessageBox::information(this, "Success", "Contact updated successfully!");
    }
}

void ChatWindow::onDeleteContact()
{
    if (rightClickedContact.isEmpty()) return;

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Delete Contact",
        QString("Are you sure you want to delete '%1'?\n\nThis will also delete all chat history with this contact.")
            .arg(rightClickedContact),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Remove from contacts list data
        for (int i = 0; i < contactsList_data.size(); ++i) {
            if (contactsList_data[i].name == rightClickedContact) {
                contactsList_data.removeAt(i);
                break;
            }
        }

        // Remove from phone mapping
        contactPhones.remove(rightClickedContact);

        // Remove chat history
        chatHistory.remove(rightClickedContact);

        // Clear chat if this contact was selected
        if (selectedContact == rightClickedContact) {
            selectedContact.clear();
            chatHeader->setText("Select a contact to start chatting");
            chatHeader->setStyleSheet(
                "QLabel {"
                "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
                "        stop:0 #f8f9fa, stop:1 #e9ecef);"
                "    color: #6c757d;"
                "    font-size: 18px;"
                "    font-weight: bold;"
                "    border-bottom: 2px solid #e9ecef;"
                "    padding: 0 30px;"
                "}"
                );
            searchFrame->hide();
            messageInput->setEnabled(false);
            sendButton->setEnabled(false);
            clearMessagesDisplay();
        }

        // Refresh contacts list
        refreshContactsList();
        saveContacts();
        saveChats();

        QMessageBox::information(this, "Success", "Contact deleted successfully!");
    }
}

// 3. ADD THIS NEW HELPER METHOD:
void ChatWindow::refreshContactsList()
{
    contactsList->clear();
    contactPhones.clear();

    for (const Contact &contact : contactsList_data) {
        addContactToList(contact);
    }
}

void ChatWindow::showNotificationPopup(const QString &contactName, const QString &message)
{
    QString title = QString("%1 sent a message").arg(contactName);
    QString body = message.length() > 50 ? message.left(47) + "..." : message;

    // System tray notification
    if (trayIcon && trayIcon->isVisible()) {
        trayIcon->showMessage(title, body, QSystemTrayIcon::Information, 3000);
    }

    // Alternative: Custom popup dialog
    QDialog *popup = new QDialog(this);
    popup->setWindowTitle("New Message");
    popup->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    popup->setAttribute(Qt::WA_DeleteOnClose);
    popup->setFixedSize(300, 100);

    // Position at bottom-right corner
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    popup->move(screenGeometry.width() - 320, screenGeometry.height() - 120);

    QVBoxLayout *layout = new QVBoxLayout(popup);
    layout->setContentsMargins(15, 10, 15, 10);

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("font-weight: bold; color: #667eea; font-size: 14px;");

    QLabel *messageLabel = new QLabel(body);
    messageLabel->setWordWrap(true);
    messageLabel->setStyleSheet("color: #495057; font-size: 12px;");

    layout->addWidget(titleLabel);
    layout->addWidget(messageLabel);

    popup->setStyleSheet(
        "QDialog {"
        "    background: white;"
        "    border: 2px solid #667eea;"
        "    border-radius: 10px;"
        "}"
        );

    popup->show();

    // Auto-close after 3 seconds
    QTimer::singleShot(3000, popup, &QDialog::close);
}

void ChatWindow::updateContactUnreadCount(const QString &contactName, int count)
{
    unreadCounts[contactName] = count;

    // Update contact list display
    for (int i = 0; i < contactsList->count(); ++i) {
        QListWidgetItem *item = contactsList->item(i);
        QString itemText = item->text();
        QString itemContactName = itemText.split('\n')[0];
        // Remove existing unread count from display name if present
        if (itemContactName.contains(" (") && itemContactName.endsWith(")")) {
            itemContactName = itemContactName.split(" (")[0];
        }

        if (itemContactName == contactName) {
            QString phone = contactPhones[contactName];
            QString displayText;

            if (count > 0) {
                displayText = QString("%1 (%2)\n📞 %3").arg(contactName).arg(count).arg(phone);
                item->setBackground(QBrush(QColor(255, 240, 240))); // Light red background
            } else {
                displayText = QString("%1\n📞 %2").arg(contactName, phone);
                item->setBackground(QBrush()); // Clear background
            }

            item->setText(displayText);
            break;
        }
    }
}

void ChatWindow::onSearchEnterPressed()
{
    QString searchText = searchInput->text().trimmed();
    if (searchText.isEmpty() || selectedContact.isEmpty()) return;

    // Find the first highlighted message widget
    for (auto it = messageWidgets.begin(); it != messageWidgets.end(); ++it) {
        MessageWidget *widget = it.value();
        if (widget->getHighlighted()) {
            // Scroll to make this widget visible
            messagesScrollArea->ensureWidgetVisible(widget);
            break;
        }
    }
}

void ChatWindow::onDeleteMessage(const QString &messageId)
{
    if (selectedContact.isEmpty()) return;

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Delete Message", "Are you sure you want to delete this message?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Remove from chat history
        QList<Message> &messages = chatHistory[selectedContact];
        for (int i = 0; i < messages.size(); ++i) {
            if (messages[i].id == messageId) {
                messages.removeAt(i);
                break;
            }
        }

        // Remove widget from UI
        MessageWidget *widget = messageWidgets[messageId];
        messagesLayout->removeWidget(widget);
        messageWidgets.remove(messageId);
        widget->deleteLater();

        // Save changes
        saveChats();
    }
}

void ChatWindow::setupAutoMessages()
{
    autoMessageTimer = new QTimer(this);
    autoMessageTimer->setSingleShot(false);

    // Set interval between 15-30 seconds (15000-30000 milliseconds)
    int interval = 15000 + QRandomGenerator::global()->bounded(15000);
    autoMessageTimer->setInterval(interval);

    connect(autoMessageTimer, &QTimer::timeout, this, &ChatWindow::sendAutoMessage);

    // Start the timer after a short delay
    QTimer::singleShot(3000, [this]() {
        if (!contactsList_data.isEmpty()) {
            autoMessageTimer->start();
        }
    });
}

void ChatWindow::sendAutoMessage()
{
    if (contactsList_data.isEmpty()) {
        return;
    }

    // Pick a random contact
    int randomIndex = QRandomGenerator::global()->bounded(contactsList_data.size());
    QString contactName = contactsList_data[randomIndex].name;

    // Array of automatic messages
    QStringList autoMessages = {
        "Hey! How's your day going? 😊",
        "Just wanted to say hi! 👋",
        "Hope you're doing well! 💙",
        "What's up? Haven't heard from you in a while 🤔",
        "Good morning! ☀️",
        "Good evening! 🌙",
        "How are things on your end? 🤗",
        "Just checking in! 📱",
        "Hope you're having a great day! ✨",
        "Missing our chats! 💭",
        "Any exciting plans today? 🎉",
        "Hope work is going well! 💼",
        "Thinking about you! 💝",
        "How's the weather there? 🌤️",
        "Just wanted to catch up! ☕",
        "Hope you're staying safe! 🛡️",
        "Any good news to share? 📰",
        "What's keeping you busy these days? ⏰",
        "Hope you're getting enough rest! 😴",
        "Sending good vibes your way! ✨"
    };

    QString randomMessage = autoMessages[QRandomGenerator::global()->bounded(autoMessages.size())];

    // Create the auto message
    Message autoMsg(contactName, randomMessage, QDateTime::currentDateTime(), false);

    qDebug() << "=== sendAutoMessage() ===";
    qDebug() << "Auto message from:" << contactName;
    qDebug() << "Message:" << randomMessage;
    qDebug() << "Selected contact:" << selectedContact;

    // Add to chat history
    chatHistory[contactName].append(autoMsg);
    qDebug() << "Added to chat history. New size:" << chatHistory[contactName].size();

    // Check if this contact is currently selected
    if (selectedContact == contactName) {
        // Contact is currently selected - add message widget immediately
        qDebug() << "Contact is selected - adding widget immediately";
        addMessageWidget(autoMsg);
    } else {
        // Contact is not selected - increment unread count and show notification
        qDebug() << "Contact not selected - updating unread count";
        unreadCounts[contactName] = unreadCounts.value(contactName, 0) + 1;
        updateContactUnreadCount(contactName, unreadCounts[contactName]);
        showNotificationPopup(contactName, randomMessage);
    }

    // Save the chat
    saveChats();

    // Set next random interval
    int nextInterval = 60000 + QRandomGenerator::global()->bounded(7000);
    autoMessageTimer->setInterval(nextInterval);

    qDebug() << "Auto message processing completed";
}

void ChatWindow::refreshCurrentChat()
{
    if (!selectedContact.isEmpty()) {
        loadChatHistory(selectedContact);
    }
}

void ChatWindow::loadContacts()
{
    QString filePath = getContactsFilePath();
    QFile file(filePath);

    if (!file.exists()) {
        loadSampleContacts();
        return;
    }

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray contactsArray = doc.array();

        contactsList_data.clear();
        contactsList->clear();
        contactPhones.clear();

        for (const QJsonValue &value : contactsArray) {
            Contact contact = Contact::fromJson(value.toObject());
            contactsList_data.append(contact);
            addContactToList(contact);
        }
    } else {
        loadSampleContacts();
    }
}

void ChatWindow::saveChats()
{
    QString filePath = getChatsFilePath();
    QJsonObject chatsObject;

    for (auto it = chatHistory.begin(); it != chatHistory.end(); ++it) {
        const QString &contact = it.key();
        const QList<Message> &messages = it.value();

        QJsonArray messagesArray;
        for (const Message &msg : messages) {
            messagesArray.append(msg.toJson());
        }

        chatsObject[contact] = messagesArray;
    }

    QJsonDocument doc(chatsObject);

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
    }
}

void ChatWindow::loadChats()
{
    QString filePath = getChatsFilePath();
    QFile file(filePath);

    if (!file.exists()) {
        return; // No chat history exists yet
    }

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject chatsObject = doc.object();

        chatHistory.clear();

        for (auto it = chatsObject.begin(); it != chatsObject.end(); ++it) {
            const QString &contact = it.key();
            QJsonArray messagesArray = it.value().toArray();

            QList<Message> messages;
            for (const QJsonValue &value : messagesArray) {
                messages.append(Message::fromJson(value.toObject()));
            }

            chatHistory[contact] = messages;
        }
    }
}

QString ChatWindow::getContactsFilePath() const
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    // Make contacts file user-specific
    return QDir(dataDir).filePath(QString("contacts_%1.json").arg(currentUser));
}

QString ChatWindow::getChatsFilePath() const
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    // Make chat file user-specific
    return QDir(dataDir).filePath(QString("chats_%1.json").arg(currentUser));
}

void ChatWindow::loadSampleContacts() {
    qDebug() << "=== loadSampleContacts() called at" << QDateTime::currentDateTime().toString() << "===";

    // Clear existing data
    contactsList_data.clear();
    contactsList->clear();
    contactPhones.clear();
    qDebug() << "Cleared existing contacts.";

    // Define sample contacts
    QList<Contact> sampleContacts = {
        Contact("Krishna Singla", "945975477"),
        Contact("Khushi Gupta", "9826264826"),
        Contact("Madhavan Dixit", "98436735347"),
        Contact("Manya", "9876543210")
    };
    qDebug() << "Sample contacts created:" << sampleContacts.size();

    // Add sample contacts to both data list and UI
    for (const Contact &contact : sampleContacts) {
        contactsList_data.append(contact);
        addContactToList(contact);
        qDebug() << "Added contact:" << contact.name << contact.phone;
    }

    // Ensure the list widget updates
    contactsList->update();
    contactsList->setVisible(true);
    contactsList->repaint();
    qDebug() << "Forced UI update for contactsList.";

    // Log final state
    qDebug() << "Final contactsList_data size:" << contactsList_data.size();
    qDebug() << "Final contactsList widget count:" << contactsList->count();
    qDebug() << "ContactsList geometry:" << contactsList->geometry();
    qDebug() << "ContactsFrame geometry:" << contactsFrame->geometry();

    // Save contacts to file
    saveContacts();
    qDebug() << "Saved contacts to file.";
}
