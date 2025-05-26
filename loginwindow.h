#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class RegisterWindow;
class ChatWindow;  // Add forward declaration

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onBackToLogin();
    void onRegisterWindowClosed();

private:
    void setupUI();
    bool validateLogin(const QString &username, const QString &password);

    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *registerButton;
    RegisterWindow *registerWindow;
    ChatWindow *chatWindow;  // Add ChatWindow pointer
};

#endif // LOGINWINDOW_H

