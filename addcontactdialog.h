#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include "chatwindow.h"

class AddContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddContactDialog(QWidget *parent = nullptr);
    ~AddContactDialog();
    void setContactData(const QString &name, const QString &phone);

    Contact getContact() const;

private slots:
    void validateInput();
    void onAddClicked();
    void onCancelClicked();

private:
    void setupUI();
    bool validatePhoneNumber(const QString &phone) const;

    // UI Components
    QVBoxLayout *mainLayout;
    QFrame *contentFrame;
    QFormLayout *formLayout;
    QLineEdit *nameEdit;
    QLineEdit *phoneEdit;
    QPushButton *addButton;
    QPushButton *cancelButton;

    // Data
    Contact contact;
};

#endif // ADDCONTACTDIALOG_H

