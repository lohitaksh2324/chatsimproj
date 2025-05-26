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

    qDebug() << "ChatWindow constructor completed";
}

ChatWindow::~ChatWindow()
{
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

    // Header with user info and logout
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

    contactsLayout->addWidget(headerFrame);
    contactsLayout->addWidget(titleFrame);
    contactsLayout->addWidget(contactsList);

    connect(contactsList, &QListWidget::itemClicked, this, &ChatWindow::onContactSelected);
    connect(logoutButton, &QPushButton::clicked, this, &ChatWindow::onLogout);
    connect(addContactButton, &QPushButton::clicked, this, &ChatWindow::onAddContact);
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
}

void ChatWindow::onContactSelected()
{
    QListWidgetItem *item = contactsList->currentItem();
    if (!item) return;

    selectedContact = item->text().split('\n')[0];
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

    messageInput->setEnabled(true);
    sendButton->setEnabled(true);

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
            "That's interesting! Tell me more.",
            "I see what you mean.",
            "Thanks for sharing that!",
            "How do you feel about that?",
            "That sounds great!",
            "I understand your point.",
            "What do you think we should do next?",
            "That's a good question.",
        };
        QString response = responses[QRandomGenerator::global()->bounded(responses.size())];
        addMessage(selectedContact, response, false);
    });
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

    // Add to UI
    addMessageWidget(msg);

    // Save to file
    saveChats();
}

void ChatWindow::addMessageWidget(const Message &msg)
{
    MessageWidget *messageWidget = new MessageWidget(msg, messagesWidget);

    // Store reference for later updates
    messageWidgets[msg.id] = messageWidget;

    // Connect to edit/delete signals
    connect(messageWidget, &MessageWidget::editRequested, this, &ChatWindow::onEditMessage);
    connect(messageWidget, &MessageWidget::deleteRequested, this, &ChatWindow::onDeleteMessage);

    // Insert before the stretch
    messagesLayout->insertWidget(messagesLayout->count() - 1, messageWidget);

    // Auto-scroll to bottom
    QTimer::singleShot(10, [this]() {
        QScrollBar *scrollBar = messagesScrollArea->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    });
}

void ChatWindow::loadChatHistory(const QString &contact)
{
    clearMessagesDisplay();

    if (chatHistory.contains(contact)) {
        const QList<Message> &messages = chatHistory[contact];
        for (const Message &msg : messages) {
            addMessageWidget(msg);
        }
    }
}

void ChatWindow::addContactToList(const Contact &contact)
{
    QString displayText = QString("%1\n📞 %2").arg(contact.name, contact.phone);
    contactsList->addItem(displayText);
    contactPhones[contact.name] = contact.phone;
}

void ChatWindow::clearMessagesDisplay()
{
    // Clear all message widgets
    for (auto it = messageWidgets.begin(); it != messageWidgets.end(); ++it) {
        MessageWidget *widget = it.value();
        messagesLayout->removeWidget(widget);
        widget->deleteLater();
    }
    messageWidgets.clear();
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
    return QDir(dataDir).filePath("contacts.json");
}

QString ChatWindow::getChatsFilePath() const
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    return QDir(dataDir).filePath("chats.json");
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
