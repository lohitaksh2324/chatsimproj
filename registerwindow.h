#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QWidget>

class QLineEdit;
class QPushButton;

class RegisterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);

signals:
    void userRegistered(const QString &username);
    void backToLogin(); // New signal to navigate back to login

private slots:
    void onRegisterClicked();
    void onCancelClicked();

private:
    void setupUI();
    bool validateInput();
    bool isValidEmail(const QString &email);

    QLineEdit *usernameEdit;
    QLineEdit *emailEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QPushButton *registerButton;
    QPushButton *cancelButton;
};

#endif // REGISTERWINDOW_H
