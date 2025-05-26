#include "RegisterWindow.h"
#include "UserManager.h"
#include <QApplication>
#include <QScreen>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QSpacerItem>
#include <QTimer>

RegisterWindow::RegisterWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Register - Create Account");
    setFixedSize(480, 900); // Increased height to accommodate content

    // Modern, frameless, translucent
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // Center window
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    setupUI();
}

void RegisterWindow::setupUI()
{
    // === Outer Main Frame ===
    QFrame *mainFrame = new QFrame(this);
    mainFrame->setObjectName("mainFrame");
    mainFrame->setStyleSheet(
        "#mainFrame {"
        " background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #667eea, stop:1 #764ba2);"
        " border-radius: 32px;"
        " border: 1px solid rgba(255,255,255,0.2);"
        "}"
        );
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(40);
    shadow->setYOffset(20);
    shadow->setColor(QColor(0, 0, 0, 80));
    mainFrame->setGraphicsEffect(shadow);

    // === Content Frame ===
    QFrame *contentFrame = new QFrame(mainFrame);
    contentFrame->setStyleSheet(
        "QFrame {"
        " background: rgba(255,255,255,0.98);"
        " border-radius: 28px;"
        " margin: 12px;" // Reduced from 16px
        "}"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(mainFrame);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QVBoxLayout *frameLayout = new QVBoxLayout(mainFrame);
    frameLayout->addWidget(contentFrame);
    frameLayout->setContentsMargins(20, 20, 20, 20); // Reduced from 24px

    QVBoxLayout *contentLayout = new QVBoxLayout(contentFrame);
    contentLayout->setSpacing(18); // Reduced from 28px for tighter but balanced spacing
    contentLayout->setContentsMargins(36, 36, 36, 50); // Adjusted: more bottom margin for buttons

    // === Header: Icon and Title ===
    QWidget *headerWidget = new QWidget();
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(0);

    QLabel *iconLabel = new QLabel("🔔");
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 60px; margin-bottom: 0px;"); // Slightly smaller icon

    QLabel *titleLabel = new QLabel("Create Account");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "font-size: 34px; font-weight: bold; color: #273043; margin-top: 0px; margin-bottom: 6px;" // Reduced font and margin
        );

    headerLayout->addSpacing(6); // Reduced from 8px
    headerLayout->addWidget(iconLabel);
    headerLayout->addWidget(titleLabel);
    headerLayout->addSpacing(6); // Reduced from 8px

    contentLayout->addSpacing(10); // Reduced from 12px
    contentLayout->addWidget(headerWidget);

    // === Subtitle ===
    QLabel *subtitleLabel = new QLabel("Join us today! Fill in your details below");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet(
        "font-size: 15px; color: #7f8c8d; margin-bottom: 20px; line-height: 1.4;" // Reduced font and margin
        );
    contentLayout->addWidget(subtitleLabel);

    // === Input Fields Helper ===
    auto createInputFrame = [](const QString &iconText, QLineEdit *&lineEdit, const QString &placeholder, bool isPassword = false) {
        QFrame *frame = new QFrame();
        frame->setFixedHeight(58); // Reduced from 64px
        frame->setStyleSheet(
            "QFrame {"
            " background: #f8f9fa;"
            " border: 2px solid #e9ecef;"
            " border-radius: 16px;" // Slightly smaller radius
            " margin: 3px 0;" // Reduced from 4px
            "}"
            "QFrame:focus-within {"
            " border-color: #667eea;"
            " background: #fff;"
            " box-shadow: 0 0 0 4px rgba(102,126,234,0.12);"
            "}"
            );
        QHBoxLayout *layout = new QHBoxLayout(frame);
        layout->setContentsMargins(20, 6, 20, 6); // Reduced from 24,8,24,8
        layout->setSpacing(16); // Reduced from 18px

        QLabel *icon = new QLabel(iconText);
        icon->setFixedWidth(30); // Reduced from 32px
        icon->setAlignment(Qt::AlignCenter);
        icon->setStyleSheet("font-size: 20px; color: #6c757d;"); // Reduced font size

        lineEdit = new QLineEdit();
        lineEdit->setPlaceholderText(placeholder);
        if (isPassword) lineEdit->setEchoMode(QLineEdit::Password);
        lineEdit->setStyleSheet(
            "QLineEdit {"
            " border: none;"
            " background: transparent;"
            " font-size: 15px;" // Reduced from 16px
            " color: #2c3e50;"
            " padding: 8px 6px;" // Reduced from 10px,8px
            "}"
            "QLineEdit::placeholder { color: #adb5bd; }"
            );

        layout->addWidget(icon);
        layout->addWidget(lineEdit);

        return frame;
    };

    // === Input Fields ===
    QFrame *usernameFrame, *emailFrame, *passwordFrame, *confirmPasswordFrame;
    usernameFrame = createInputFrame("👤", usernameEdit, "Choose a username");
    emailFrame = createInputFrame("📧", emailEdit, "Enter your email address");
    passwordFrame = createInputFrame("🔐", passwordEdit, "Create a password", true);
    confirmPasswordFrame = createInputFrame("🔒", confirmPasswordEdit, "Confirm your password", true);

    contentLayout->addWidget(usernameFrame);
    contentLayout->addWidget(emailFrame);
    contentLayout->addWidget(passwordFrame);
    contentLayout->addWidget(confirmPasswordFrame);

    // === Password Info ===
    QLabel *passwordInfo = new QLabel("💡 Password must be at least 6 characters long");
    passwordInfo->setStyleSheet(
        "font-size: 13px; color: #6c757d;" // Reduced font size
        "background: rgba(102,126,234,0.08);"
        "padding: 12px 16px; border-radius: 12px;" // Reduced padding and radius
        "border-left: 4px solid #667eea; margin: 6px 0 16px 0;" // Reduced margins
        );
    contentLayout->addWidget(passwordInfo);

    // === Buttons ===
    registerButton = new QPushButton("Create My Account");
    registerButton->setFixedHeight(54); // Reduced from 58px
    registerButton->setStyleSheet(
        "QPushButton {"
        " background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2);"
        " border: none; border-radius: 18px; color: white;" // Reduced radius
        " font-size: 14px; font-weight: bold; padding: 14px;" // Reduced font and padding
        "}"
        "QPushButton:hover {"
        " background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #5a67d8, stop:1 #6b46c1);"
        "}"
        "QPushButton:pressed {"
        " background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4c51bf, stop:1 #553c9a);"
        "}"
        );

    cancelButton = new QPushButton("Cancel");
    cancelButton->setFixedHeight(50); // Reduced from 54px
    cancelButton->setStyleSheet(
        "QPushButton {"
        " background: transparent; border: 2px solid #dc3545; border-radius: 15px;" // Reduced radius
        " color: #dc3545; font-size: 15px; font-weight: bold; padding: 10px;" // Reduced padding
        "}"
        "QPushButton:hover {"
        " background: rgba(220,53,69,0.08); border-color: #c82333; color: #c82333;"
        "}"
        "QPushButton:pressed {"
        " background: rgba(220,53,69,0.15);"
        "}"
        );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(16); // Reduced from 20px
    buttonLayout->addWidget(cancelButton, 1);
    buttonLayout->addWidget(registerButton, 2);

    contentLayout->addLayout(buttonLayout);
    contentLayout->addSpacing(30); // Added spacing at the bottom to prevent cutoff

    // === Connect Signals ===
    connect(registerButton, &QPushButton::clicked, this, &RegisterWindow::onRegisterClicked);
    connect(cancelButton, &QPushButton::clicked, this, &RegisterWindow::onCancelClicked);
    connect(confirmPasswordEdit, &QLineEdit::returnPressed, this, &RegisterWindow::onRegisterClicked);
}

void RegisterWindow::onRegisterClicked()
{
    qDebug() << "=== REGISTER BUTTON CLICKED ===";

    if (!validateInput()) {
        return;
    }

    QString username = usernameEdit->text().trimmed();
    QString email = emailEdit->text().trimmed();
    QString password = passwordEdit->text();

    UserManager& userManager = UserManager::instance();
    if (userManager.userExists(username)) {
        QMessageBox::warning(this, "Error", "Username already exists. Please choose a different username.");
        usernameEdit->setFocus();
        usernameEdit->selectAll();
        return;
    }

    if (userManager.registerUser(username, email, password)) {
        QMessageBox::information(this, "Success",
                                 QString("Account created successfully!\nUsername: %1\nEmail: %2\n\nYou can now login with your credentials.").arg(username, email));
        emit userRegistered(username);

        // Emit signal to show login window and close this window
        emit backToLogin();
        close();
    } else {
        QMessageBox::critical(this, "Error", "Failed to create account. Please try again.");
    }
}

void RegisterWindow::onCancelClicked()
{
    // Emit signal to show login window and close this window
    emit backToLogin();
    close();
}

bool RegisterWindow::validateInput()
{
    QString username = usernameEdit->text().trimmed();
    QString email = emailEdit->text().trimmed();
    QString password = passwordEdit->text();
    QString confirmPassword = confirmPasswordEdit->text();

    if (username.isEmpty()) {
        QMessageBox::warning(this, "Error", "Username is required.");
        usernameEdit->setFocus();
        return false;
    }

    if (username.length() < 3) {
        QMessageBox::warning(this, "Error", "Username must be at least 3 characters long.");
        usernameEdit->setFocus();
        return false;
    }

    QRegularExpression usernameRegex("^[A-Za-z0-9_]+$");
    if (!usernameRegex.match(username).hasMatch()) {
        QMessageBox::warning(this, "Error", "Username can only contain letters, numbers, and underscores.");
        usernameEdit->setFocus();
        return false;
    }

    if (email.isEmpty()) {
        QMessageBox::warning(this, "Error", "Email is required.");
        emailEdit->setFocus();
        return false;
    }

    if (!isValidEmail(email)) {
        QMessageBox::warning(this, "Error", "Please enter a valid email address.");
        emailEdit->setFocus();
        return false;
    }

    if (password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Password is required.");
        passwordEdit->setFocus();
        return false;
    }

    if (password.length() < 6) {
        QMessageBox::warning(this, "Error", "Password must be at least 6 characters long.");
        passwordEdit->setFocus();
        return false;
    }

    if (password != confirmPassword) {
        QMessageBox::warning(this, "Error", "Passwords do not match.");
        confirmPasswordEdit->clear();
        confirmPasswordEdit->setFocus();
        return false;
    }

    return true;
}

bool RegisterWindow::isValidEmail(const QString &email)
{
    QRegularExpression emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    return emailRegex.match(email).hasMatch();
}
