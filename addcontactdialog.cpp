#include "AddContactDialog.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

AddContactDialog::AddContactDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Add New Contact");
    setFixedSize(400, 500);
    setModal(true);

    // Set window properties for modern look
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    // Center the dialog
    if (parent) {
        QRect parentGeometry = parent->geometry();
        int x = parentGeometry.x() + (parentGeometry.width() - width()) / 2;
        int y = parentGeometry.y() + (parentGeometry.height() - height()) / 2;
        move(x, y);
    } else {
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }

    setupUI();
}

AddContactDialog::~AddContactDialog()
{
}

void AddContactDialog::setupUI()
{
    // Main layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Content frame with modern styling
    contentFrame = new QFrame();
    contentFrame->setStyleSheet(
        "QFrame {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "        stop:0 #f8f9fa, stop:1 #e9ecef);"
        "    border-radius: 16px;"
        "    border: 1px solid #dee2e6;"
        "}"
        );

    // Add shadow effect
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setXOffset(0);
    shadow->setYOffset(4);
    shadow->setColor(QColor(0, 0, 0, 40));
    contentFrame->setGraphicsEffect(shadow);

    QVBoxLayout *contentLayout = new QVBoxLayout(contentFrame);
    contentLayout->setContentsMargins(30, 30, 30, 30);
    contentLayout->setSpacing(20);

    // Header
    QLabel *headerLabel = new QLabel("👤 Add New Contact");
    headerLabel->setAlignment(Qt::AlignCenter);
    headerLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 24px;"
        "    font-weight: bold;"
        "    color: #2c3e50;"
        "    margin-bottom: 10px;"
        "    background: transparent;"
        "}"
        );

    QLabel *subHeaderLabel = new QLabel("Enter contact details below");
    subHeaderLabel->setAlignment(Qt::AlignCenter);
    subHeaderLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 14px;"
        "    color: #6c757d;"
        "    margin-bottom: 20px;"
        "    background: transparent;"
        "}"
        );

    // Form container
    QFrame *formFrame = new QFrame();
    formFrame->setStyleSheet(
        "QFrame {"
        "    background: rgba(255, 255, 255, 0.8);"
        "    border-radius: 12px;"
        "    border: 1px solid rgba(255, 255, 255, 0.5);"
        "}"
        );

    formLayout = new QFormLayout(formFrame);
    formLayout->setContentsMargins(25, 25, 25, 25);
    formLayout->setSpacing(20);

    // Name input
    QFrame *nameFrame = new QFrame();
    nameFrame->setFixedHeight(50);
    nameFrame->setStyleSheet(
        "QFrame {"
        "    background: #f8f9fa;"
        "    border: 2px solid #e9ecef;"
        "    border-radius: 12px;"
        "}"
        "QFrame:focus-within {"
        "    border-color: #667eea;"
        "    background: #ffffff;"
        "}"
        );

    QHBoxLayout *nameLayout = new QHBoxLayout(nameFrame);
    nameLayout->setContentsMargins(15, 0, 15, 0);
    nameLayout->setSpacing(10);

    QLabel *nameIcon = new QLabel("👤");
    nameIcon->setStyleSheet("font-size: 16px; color: #6c757d;");

    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Enter full name");
    nameEdit->setStyleSheet(
        "QLineEdit {"
        "    border: none;"
        "    background: transparent;"
        "    font-size: 14px;"
        "    color: #2c3e50;"
        "    padding: 8px 4px;"
        "}"
        "QLineEdit::placeholder {"
        "    color: #adb5bd;"
        "}"
        );

    nameLayout->addWidget(nameIcon);
    nameLayout->addWidget(nameEdit);

    // Phone input
    QFrame *phoneFrame = new QFrame();
    phoneFrame->setFixedHeight(50);
    phoneFrame->setStyleSheet(
        "QFrame {"
        "    background: #f8f9fa;"
        "    border: 2px solid #e9ecef;"
        "    border-radius: 12px;"
        "}"
        "QFrame:focus-within {"
        "    border-color: #667eea;"
        "    background: #ffffff;"
        "}"
        );

    QHBoxLayout *phoneLayout = new QHBoxLayout(phoneFrame);
    phoneLayout->setContentsMargins(15, 0, 15, 0);
    phoneLayout->setSpacing(10);

    QLabel *phoneIcon = new QLabel("📞");
    phoneIcon->setStyleSheet("font-size: 16px; color: #6c757d;");

    phoneEdit = new QLineEdit();
    phoneEdit->setPlaceholderText("Enter phone number");
    phoneEdit->setStyleSheet(
        "QLineEdit {"
        "    border: none;"
        "    background: transparent;"
        "    font-size: 14px;"
        "    color: #2c3e50;"
        "    padding: 8px 4px;"
        "}"
        "QLineEdit::placeholder {"
        "    color: #adb5bd;"
        "}"
        );

    // Add phone number validation
    QRegularExpression phoneRegex("^[+]?[0-9\\s\\-\\(\\)]{7,20}$");
    QRegularExpressionValidator *phoneValidator = new QRegularExpressionValidator(phoneRegex, this);
    phoneEdit->setValidator(phoneValidator);

    phoneLayout->addWidget(phoneIcon);
    phoneLayout->addWidget(phoneEdit);

    // Add form rows
    QLabel *nameLabel = new QLabel("Name:");
    nameLabel->setStyleSheet(
        "QLabel {"
        "    font-weight: bold;"
        "    color: #495057;"
        "    font-size: 14px;"
        "    background: transparent;"
        "}"
        );

    QLabel *phoneLabel = new QLabel("Phone:");
    phoneLabel->setStyleSheet(
        "QLabel {"
        "    font-weight: bold;"
        "    color: #495057;"
        "    font-size: 14px;"
        "    background: transparent;"
        "}"
        );

    formLayout->addRow(nameLabel, nameFrame);
    formLayout->addRow(phoneLabel, phoneFrame);

    // Buttons
    QFrame *buttonFrame = new QFrame();
    buttonFrame->setStyleSheet("background: transparent;");
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonFrame);
    buttonLayout->setSpacing(15);

    cancelButton = new QPushButton("Cancel");
    cancelButton->setFixedHeight(45);
    cancelButton->setStyleSheet(
        "QPushButton {"
        "    background: transparent;"
        "    border: 2px solid #dc3545;"
        "    border-radius: 12px;"
        "    color: #dc3545;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    padding: 12px 24px;"
        "}"
        "QPushButton:hover {"
        "    background: rgba(220, 53, 69, 0.1);"
        "    border-color: #c82333;"
        "}"
        "QPushButton:pressed {"
        "    background: rgba(220, 53, 69, 0.2);"
        "}"
        );

    addButton = new QPushButton("Add Contact");
    addButton->setFixedHeight(45);
    addButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "    border: none;"
        "    border-radius: 12px;"
        "    color: white;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    padding: 12px 24px;"
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

    addButton->setEnabled(false); // Initially disabled

    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(addButton);

    // Add widgets to content layout
    contentLayout->addWidget(headerLabel);
    contentLayout->addWidget(subHeaderLabel);
    contentLayout->addWidget(formFrame);
    contentLayout->addWidget(buttonFrame);
    contentLayout->addStretch();

    mainLayout->addWidget(contentFrame);

    // Connect signals
    connect(nameEdit, &QLineEdit::textChanged, this, &AddContactDialog::validateInput);
    connect(phoneEdit, &QLineEdit::textChanged, this, &AddContactDialog::validateInput);
    connect(addButton, &QPushButton::clicked, this, &AddContactDialog::onAddClicked);
    connect(cancelButton, &QPushButton::clicked, this, &AddContactDialog::onCancelClicked);

    // Set initial focus
    nameEdit->setFocus();
}

void AddContactDialog::validateInput()
{
    QString name = nameEdit->text().trimmed();
    QString phone = phoneEdit->text().trimmed();

    bool isValid = !name.isEmpty() && !phone.isEmpty() && validatePhoneNumber(phone);
    addButton->setEnabled(isValid);
}

bool AddContactDialog::validatePhoneNumber(const QString &phone) const
{
    if (phone.length() < 7) return false;

    // Remove all non-digit characters except +
    QString cleaned = phone;
    cleaned.remove(QRegularExpression("[^+0-9]"));

    // Check if it's a valid length (7-15 digits, optionally starting with +)
    if (cleaned.startsWith("+")) {
        return cleaned.length() >= 8 && cleaned.length() <= 16;
    } else {
        return cleaned.length() >= 7 && cleaned.length() <= 15;
    }
}

void AddContactDialog::onAddClicked()
{
    QString name = nameEdit->text().trimmed();
    QString phone = phoneEdit->text().trimmed();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a name.");
        nameEdit->setFocus();
        return;
    }

    if (phone.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a phone number.");
        phoneEdit->setFocus();
        return;
    }

    if (!validatePhoneNumber(phone)) {
        QMessageBox::warning(this, "Error",
                             "Please enter a valid phone number (7-15 digits, optionally starting with +).");
        phoneEdit->setFocus();
        return;
    }

    contact = Contact(name, phone);
    accept();
}

void AddContactDialog::onCancelClicked()
{
    reject();
}

Contact AddContactDialog::getContact() const
{
    return contact;
}
