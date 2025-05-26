#include "MessageActions.h"
#include "ChatWindow.h"
#include <QApplication>
#include <QClipboard>
#include <QMouseEvent>
#include <QTimer>
#include <QGraphicsDropShadowEffect>

// EditMessageDialog Implementation
EditMessageDialog::EditMessageDialog(const Message &message, QWidget *parent)
    : QDialog(parent), originalMessage(message)
{
    setWindowTitle("Edit Message");
    setModal(true);
    setFixedSize(500, 300);

    // Center the dialog
    if (parent) {
        move(parent->geometry().center() - rect().center());
    }

    setupUI();

    // Set the current message content
    contentEdit->setPlainText(message.content);
    editedContent = message.content;

    // Focus on the text edit
    contentEdit->setFocus();
    contentEdit->selectAll();
}

void EditMessageDialog::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Title
    titleLabel = new QLabel("Edit Message");
    titleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    color: #495057;"
        "    margin-bottom: 10px;"
        "}"
        );

    // Info label
    QString timeString = originalMessage.timestamp.toString("MMM dd, yyyy hh:mm AP");
    infoLabel = new QLabel(QString("Originally sent on %1").arg(timeString));
    infoLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 12px;"
        "    color: #6c757d;"
        "    margin-bottom: 5px;"
        "}"
        );

    // Content editor
    contentEdit = new QTextEdit();
    contentEdit->setStyleSheet(
        "QTextEdit {"
        "    border: 2px solid #e9ecef;"
        "    border-radius: 8px;"
        "    padding: 10px;"
        "    font-size: 14px;"
        "    font-family: 'Segoe UI', Arial, sans-serif;"
        "    background: #ffffff;"
        "}"
        "QTextEdit:focus {"
        "    border-color: #667eea;"
        "}"
        );

    // Button layout
    buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    cancelButton = new QPushButton("Cancel");
    cancelButton->setFixedSize(80, 35);
    cancelButton->setStyleSheet(
        "QPushButton {"
        "    background: #6c757d;"
        "    border: none;"
        "    border-radius: 6px;"
        "    color: white;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: #545b62;"
        "}"
        "QPushButton:pressed {"
        "    background: #495057;"
        "}"
        );

    saveButton = new QPushButton("Save");
    saveButton->setFixedSize(80, 35);
    saveButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "    border: none;"
        "    border-radius: 6px;"
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
        );

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(saveButton);

    // Add to main layout
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(infoLabel);
    mainLayout->addWidget(contentEdit);
    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(saveButton, &QPushButton::clicked, this, &EditMessageDialog::onSaveClicked);
    connect(cancelButton, &QPushButton::clicked, this, &EditMessageDialog::onCancelClicked);
    connect(contentEdit, &QTextEdit::textChanged, this, &EditMessageDialog::onTextChanged);

    // Apply shadow effect
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setXOffset(0);
    shadow->setYOffset(5);
    shadow->setColor(QColor(0, 0, 0, 60));
    setGraphicsEffect(shadow);
}

QString EditMessageDialog::getEditedContent() const
{
    return editedContent;
}

void EditMessageDialog::onSaveClicked()
{
    editedContent = contentEdit->toPlainText().trimmed();

    if (editedContent.isEmpty()) {
        QMessageBox::warning(this, "Empty Message",
                             "Message cannot be empty. Please enter some text or cancel.");
        return;
    }

    if (editedContent == originalMessage.content) {
        reject(); // No changes made
        return;
    }

    accept();
}

void EditMessageDialog::onCancelClicked()
{
    reject();
}

void EditMessageDialog::onTextChanged()
{
    QString currentText = contentEdit->toPlainText().trimmed();
    saveButton->setEnabled(!currentText.isEmpty() && currentText != originalMessage.content);
}

// EnhancedMessageWidget Implementation
EnhancedMessageWidget::EnhancedMessageWidget(const Message &msg, ChatWindow *chatWindow, QWidget *parent)
    : MessageWidget(msg, parent), parentChatWindow(chatWindow)
{
    // Remove the unused parameter warning by using the msg parameter
    Q_UNUSED(msg); // This tells the compiler we know about the unused parameter

    setContextMenuPolicy(Qt::DefaultContextMenu);

    // Create context menu
    contextMenu = new QMenu(this);

    editAction = contextMenu->addAction("✏️ Edit Message");
    copyAction = contextMenu->addAction("📋 Copy Text");
    separatorAction = contextMenu->addSeparator();
    deleteAction = contextMenu->addAction("🗑️ Delete Message");

    // Style the context menu
    contextMenu->setStyleSheet(
        "QMenu {"
        "    background: #ffffff;"
        "    border: 2px solid #e9ecef;"
        "    border-radius: 8px;"
        "    padding: 5px;"
        "}"
        "QMenu::item {"
        "    padding: 8px 16px;"
        "    border-radius: 4px;"
        "    color: #495057;"
        "    font-size: 14px;"
        "}"
        "QMenu::item:selected {"
        "    background: #667eea;"
        "    color: white;"
        "}"
        "QMenu::item:disabled {"
        "    color: #adb5bd;"
        "}"
        "QMenu::separator {"
        "    height: 1px;"
        "    background: #e9ecef;"
        "    margin: 5px 10px;"
        "}"
        );

    // Only allow editing for current user's messages
    if (!getMessage().isCurrentUser) {
        editAction->setEnabled(false);
        editAction->setText("✏️ Edit Message (Not your message)");
    }

    // Connect actions
    connect(editAction, &QAction::triggered, this, &EnhancedMessageWidget::onEditMessage);
    connect(deleteAction, &QAction::triggered, this, &EnhancedMessageWidget::onDeleteMessage);
    connect(copyAction, &QAction::triggered, this, &EnhancedMessageWidget::onCopyMessage);
}

void EnhancedMessageWidget::contextMenuEvent(QContextMenuEvent *event)
{
    contextMenu->exec(event->globalPos());
}

void EnhancedMessageWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        // Right click handled by contextMenuEvent
        return;
    }
    MessageWidget::mousePressEvent(event);
}

void EnhancedMessageWidget::onEditMessage()
{
    if (getMessage().isCurrentUser) {
        emit editRequested(getMessage().id);
    }
}

void EnhancedMessageWidget::onDeleteMessage()
{
    emit deleteRequested(getMessage().id);
}

void EnhancedMessageWidget::onCopyMessage()
{
    emit copyRequested(getMessage().content);
}

// MessageManager Implementation
MessageManager::MessageManager(ChatWindow *chatWindow, QObject *parent)
    : QObject(parent), chatWindow(chatWindow)
{
}

bool MessageManager::editMessage(const QString &messageId, const QString &newContent)
{
    // This method would need to be implemented in coordination with ChatWindow
    // For now, we'll emit the signal and let ChatWindow handle the actual editing
    emit messageEdited(messageId, newContent);
    return true;
}

bool MessageManager::deleteMessage(const QString &messageId)
{
    // Show confirmation dialog
    QMessageBox::StandardButton reply = QMessageBox::question(
        chatWindow, "Delete Message",
        "Are you sure you want to delete this message?\n\nThis action cannot be undone.",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        emit messageDeleted(messageId);
        return true;
    }

    return false;
}

void MessageManager::copyMessageToClipboard(const QString &content)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(content);

    // Could show a brief notification here
    // For now, we'll just copy silently
}
