#ifndef MESSAGEACTIONS_H
#define MESSAGEACTIONS_H

#include "chatwindow.h" // Include ChatWindow header
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QDateTime>
#include <QString>
#include <QObject>
#include <QWidget>

// Forward declarations (no need to redeclare ChatWindow or MessageWidget)
// They are already defined in chatwindow.h

// Dialog for editing messages
class EditMessageDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EditMessageDialog(const Message &message, QWidget *parent = nullptr);
    QString getEditedContent() const;

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onTextChanged();

private:
    void setupUI();

    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QLabel *infoLabel;
    QTextEdit *contentEdit;
    QHBoxLayout *buttonLayout;
    QPushButton *saveButton;
    QPushButton *cancelButton;
    Message originalMessage;
    QString editedContent;
};

// Enhanced MessageWidget with context menu support
// Inherits from the MessageWidget defined in chatwindow.h
class EnhancedMessageWidget : public MessageWidget
{
    Q_OBJECT
public:
    explicit EnhancedMessageWidget(const Message &msg, ChatWindow *chatWindow, QWidget *parent = nullptr);

signals:
    void editRequested(const QString &messageId);
    void deleteRequested(const QString &messageId);
    void copyRequested(const QString &content);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onEditMessage();
    void onDeleteMessage();
    void onCopyMessage();

private:
    ChatWindow *parentChatWindow;
    QMenu *contextMenu;
    QAction *editAction;
    QAction *deleteAction;
    QAction *copyAction;
    QAction *separatorAction;
};

// Message management class
class MessageManager : public QObject
{
    Q_OBJECT
public:
    explicit MessageManager(ChatWindow *chatWindow, QObject *parent = nullptr);

    bool editMessage(const QString &messageId, const QString &newContent);
    bool deleteMessage(const QString &messageId);
    void copyMessageToClipboard(const QString &content);

signals:
    void messageEdited(const QString &messageId, const QString &newContent);
    void messageDeleted(const QString &messageId);

private:
    ChatWindow *chatWindow;
};

#endif // MESSAGEACTIONS_H
