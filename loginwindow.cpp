#include "LoginWindow.h"
#include "RegisterWindow.h"
#include "ChatWindow.h"
#include "usermanager.h"
#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QFrame>
#include <QGraphicsDropShadowEffect>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent), registerWindow(nullptr), chatWindow(nullptr)
{
    setupUI();
    setWindowTitle("Login - Welcome Back");
    setFixedSize(440, 700);

    // Set window properties for modern look
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // Center the window
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

LoginWindow::~LoginWindow()
{
    if (registerWindow) {
        delete registerWindow;
    }
    if (chatWindow) {
        delete chatWindow;
    }
}

void LoginWindow::setupUI()
{
    // Main container with rounded corners and shadow
    QFrame *mainFrame = new QFrame(this);
    mainFrame->setObjectName("mainFrame");
    mainFrame->setStyleSheet(
        "#mainFrame {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "    border-radius: 24px;"
        "    border: 1px solid rgba(255, 255, 255, 0.2);"
        "}"
        );

    // Add shadow effect
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(30);
    shadow->setXOffset(0);
    shadow->setYOffset(12);
    shadow->setColor(QColor(0, 0, 0, 60));
    mainFrame->setGraphicsEffect(shadow);

    // Content frame
    QFrame *contentFrame = new QFrame(mainFrame);
    contentFrame->setStyleSheet(
        "QFrame {"
        "    background: rgba(255, 255, 255, 0.96);"
        "    border-radius: 20px;"
        "    margin: 12px;"
        "}"
        );

    // Create layouts with better spacing
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(mainFrame);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *frameLayout = new QVBoxLayout(mainFrame);
    frameLayout->addWidget(contentFrame);
    frameLayout->setContentsMargins(20, 20, 20, 20);

    QVBoxLayout *contentLayout = new QVBoxLayout(contentFrame);
    contentLayout->setSpacing(20);
    contentLayout->setContentsMargins(30, 30, 30, 30);

    // Close button
    QPushButton *closeButton = new QPushButton("✕", this);
    closeButton->setFixedSize(30, 30);
    closeButton->setStyleSheet(
        "QPushButton {"
        "  background: transparent;"
        "  font-size: 20px;"
        "  color: #888;"
        "  border: none;"
        "}"
        "QPushButton:hover { color: #f00; }"
        );
    // Place it at the top-right
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addStretch();
    topLayout->addWidget(closeButton);
    topLayout->setContentsMargins(0, 35, 0, 0);
    mainLayout->insertLayout(0, topLayout);

    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);

    // Title section with better spacing
    QLabel *titleLabel = new QLabel("🔐 Welcome Back");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 28px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "    margin-bottom: 8px;"
        "}"
        );

    QLabel *subtitleLabel = new QLabel("Sign in to your account");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 16px;"
        "    color: #7f8c8d;"
        "    margin-bottom: 30px;"
        "}"
        );

    // Username input with improved design
    QFrame *usernameFrame = new QFrame();
    usernameFrame->setFixedHeight(60);
    usernameFrame->setStyleSheet(
        "QFrame {"
        "    background: #f8f9fa;"
        "    border: 2px solid #e9ecef;"
        "    border-radius: 16px;"
        "}"
        "QFrame:focus-within {"
        "    border-color: #667eea;"
        "    background: #ffffff;"
        "}"
        );

    QHBoxLayout *usernameLayout = new QHBoxLayout(usernameFrame);
    usernameLayout->setContentsMargins(20, 0, 20, 0);
    usernameLayout->setSpacing(12);

    QLabel *userIcon = new QLabel("👤");
    userIcon->setStyleSheet(
        "font-size: 18px; color: #6c757d; min-width: 30px; min-height: 30px; margin-left: 7px; margin-top: 0px;"
        );

    usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("Enter your username");
    usernameEdit->setStyleSheet(
        "QLineEdit {"
        "    border: none;"
        "    background: transparent;"
        "    font-size: 16px;"
        "    color: #2c3e50;"
        "    padding: 8px 4px;"
        "    min-height: 20px;"
        "}"
        "QLineEdit::placeholder {"
        "    color: #adb5bd;"
        "    font-style: normal;"
        "}"
        );

    usernameLayout->addWidget(userIcon);
    usernameLayout->addWidget(usernameEdit);

    // Password input with improved design
    QFrame *passwordFrame = new QFrame();
    passwordFrame->setFixedHeight(60);
    passwordFrame->setStyleSheet(
        "QFrame {"
        "    background: #f8f9fa;"
        "    border: 2px solid #e9ecef;"
        "    border-radius: 16px;"
        "}"
        "QFrame:focus-within {"
        "    border-color: #667eea;"
        "    background: #ffffff;"
        "}"
        );

    QHBoxLayout *passwordLayout = new QHBoxLayout(passwordFrame);
    passwordLayout->setContentsMargins(20, 0, 20, 0);
    passwordLayout->setSpacing(12);

    QLabel *lockIcon = new QLabel("🔒");
    lockIcon->setStyleSheet(
        "font-size: 18px; color: #6c757d; min-width: 20px; min-height: 30px; margin-left: 7px; margin-top: 0px;"
        );

    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("Enter your password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setStyleSheet(
        "QLineEdit {"
        "    border: none;"
        "    background: transparent;"
        "    font-size: 16px;"
        "    color: #2c3e50;"
        "    padding: 8px 4px;"
        "    min-height: 20px;"
        "}"
        "QLineEdit::placeholder {"
        "    color: #adb5bd;"
        "    font-style: normal;"
        "}"
        );

    passwordLayout->addWidget(lockIcon);
    passwordLayout->addWidget(passwordEdit);

    // Login button with better proportions
    loginButton = new QPushButton("Sign In");
    loginButton->setFixedHeight(50);
    loginButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "    border: none;"
        "    border-radius: 16px;"
        "    color: white;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    padding: 16px;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #5a67d8, stop:1 #6b46c1);"
        "    transform: translateY(-2px);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #4c51bf, stop:1 #553c9a);"
        "    transform: translateY(0px);"
        "}"
        );

    // Spacer for better visual separation
    contentLayout->addStretch(1);

    // Separator with better styling
    QFrame *separator = new QFrame();
    separator->setFrameStyle(QFrame::HLine);
    separator->setFixedHeight(1);
    separator->setStyleSheet(
        "QFrame {"
        "    background-color: #e9ecef;"
        "    border: none;"
        "    margin: 20px 40px;"
        "}"
        );

    // Register section
    QLabel *registerLabel = new QLabel("Don't have an account?");
    registerLabel->setAlignment(Qt::AlignCenter);
    registerLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 14px;"
        "    color: #6c757d;"
        "    margin-bottom: 12px;"
        "}"
        );

    registerButton = new QPushButton("Create Account");
    registerButton->setFixedHeight(45);
    registerButton->setStyleSheet(
        "QPushButton {"
        "    background: transparent;"
        "    border: 2px solid #667eea;"
        "    border-radius: 14px;"
        "    color: #667eea;"
        "    font-size: 15px;"
        "    font-weight: bold;"
        "    padding: 12px;"
        "}"
        "QPushButton:hover {"
        "    background: rgba(102, 126, 234, 0.08);"
        "    border-color: #5a67d8;"
        "    color: #5a67d8;"
        "}"
        "QPushButton:pressed {"
        "    background: rgba(102, 126, 234, 0.15);"
        "}"
        );

    // Add widgets to content layout with proper spacing
    contentLayout->addWidget(titleLabel);
    contentLayout->addWidget(subtitleLabel);
    contentLayout->addWidget(usernameFrame);
    contentLayout->addWidget(passwordFrame);
    contentLayout->addWidget(loginButton);
    contentLayout->addStretch(1);
    contentLayout->addWidget(separator);
    contentLayout->addWidget(registerLabel);
    contentLayout->addWidget(registerButton);
    contentLayout->addStretch(1);

    // Connect signals
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginWindow::onLoginClicked);
}

void LoginWindow::onLoginClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();

    qDebug() << "Login attempt - Username:" << username;

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter both username and password.");
        return;
    }

    UserManager& userManager = UserManager::instance();

    if (userManager.authenticateUser(username, password)) {
        qDebug() << "Authentication successful, creating chat window";

        // Clean up existing chat window if it exists
        if (chatWindow) {
            chatWindow->deleteLater();
            chatWindow = nullptr;
        }

        // Create and show chat window
        chatWindow = new ChatWindow(username, this);

        // Set window flags to ensure it appears properly
        chatWindow->setWindowFlags(Qt::Window);
        chatWindow->setAttribute(Qt::WA_ShowWithoutActivating, false);

        // Connect chat window close event to show login again
        connect(chatWindow, &QWidget::destroyed, this, [this]() {
            qDebug() << "Chat window destroyed, showing login again";
            chatWindow = nullptr;
            this->show();
            this->raise();
            this->activateWindow();
            // Clear login form
            usernameEdit->clear();
            passwordEdit->clear();
            usernameEdit->setFocus();
        });

        // Hide login window first
        this->hide();

        // Show chat window with proper activation
        chatWindow->show();
        chatWindow->raise();
        chatWindow->activateWindow();

        // Force the window to be on top temporarily
        chatWindow->setWindowState(Qt::WindowActive);

        // Additional debugging
        qDebug() << "Chat window created with geometry:" << chatWindow->geometry();
        qDebug() << "Chat window visible:" << chatWindow->isVisible();
        qDebug() << "Chat window window state:" << chatWindow->windowState();

        // Force a repaint
        chatWindow->repaint();

    } else {
        qDebug() << "Authentication failed";
        QMessageBox::warning(this, "Login Failed",
                             "Invalid username or password. Please try again.");
        passwordEdit->clear();
        passwordEdit->setFocus();
    }
}

void LoginWindow::onRegisterClicked()
{
    qDebug() << "Register button clicked in LoginWindow";

    // Create new register window
    registerWindow = new RegisterWindow();

    // Connect the backToLogin signal to show this window again
    connect(registerWindow, &RegisterWindow::backToLogin, this, &LoginWindow::onBackToLogin);

    // Show register window and hide login window
    registerWindow->show();
    registerWindow->raise();
    registerWindow->activateWindow();
    this->hide();

    qDebug() << "RegisterWindow created and shown, LoginWindow hidden";
}

void LoginWindow::onBackToLogin()
{
    qDebug() << "Back to login signal received";

    // Clean up register window
    if (registerWindow) {
        registerWindow->deleteLater();
        registerWindow = nullptr;
    }

    // Show login window again
    this->show();
    this->raise();
    this->activateWindow();

    // Clear the form fields for fresh start
    usernameEdit->clear();
    passwordEdit->clear();
    usernameEdit->setFocus();
}

void LoginWindow::onRegisterWindowClosed()
{
    registerWindow = nullptr;
}

bool LoginWindow::validateLogin(const QString &username, const QString &password)
{
    return (username == "admin" && password == "password") ||
           (username == "user" && password == "123456");
}
