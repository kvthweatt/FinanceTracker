// main.cpp
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QMessageBox>
#include <QDateEdit>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QHeaderView>
#include <vector>
#include <map>

class Expense {
public:
    QString date;
    QString category;
    double amount;
    QString description;

    Expense(const QString& d, const QString& c, double a, const QString& desc)
        : date(d), category(c), amount(a), description(desc) {}

    QString serialize() const {
        return QString("%1,%2,%3,%4").arg(date).arg(category)
                                    .arg(amount).arg(description);
    }
};

class FinanceTrackerGUI : public QMainWindow {
    Q_OBJECT

private:
    std::vector<Expense> expenses;
    QString filename;

    // GUI Elements
    QTableWidget* expenseTable;
    QComboBox* categoryCombo;
    QLineEdit* amountEdit;
    QLineEdit* descriptionEdit;
    QDateEdit* dateEdit;
    QLabel* totalLabel;

    void setupUI() {
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

        // Input Form
        QGroupBox* inputGroup = new QGroupBox("Add New Expense");
        QGridLayout* formLayout = new QGridLayout;

        dateEdit = new QDateEdit(QDate::currentDate());
        dateEdit->setCalendarPopup(true);
        formLayout->addWidget(new QLabel("Date:"), 0, 0);
        formLayout->addWidget(dateEdit, 0, 1);

        categoryCombo = new QComboBox();
        categoryCombo->addItems({"Food", "Transport", "Utilities", "Other"});
        formLayout->addWidget(new QLabel("Category:"), 1, 0);
        formLayout->addWidget(categoryCombo, 1, 1);

        amountEdit = new QLineEdit();
        amountEdit->setPlaceholderText("Enter amount");
        formLayout->addWidget(new QLabel("Amount:"), 2, 0);
        formLayout->addWidget(amountEdit, 2, 1);

        descriptionEdit = new QLineEdit();
        descriptionEdit->setPlaceholderText("Enter description");
        formLayout->addWidget(new QLabel("Description:"), 3, 0);
        formLayout->addWidget(descriptionEdit, 3, 1);

        QPushButton* addButton = new QPushButton("Add Expense");
        formLayout->addWidget(addButton, 4, 0, 1, 2);
        
        inputGroup->setLayout(formLayout);
        mainLayout->addWidget(inputGroup);

        // Expense Table
        expenseTable = new QTableWidget(0, 4);
        expenseTable->setHorizontalHeaderLabels({"Date", "Category", "Amount", "Description"});
        expenseTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        mainLayout->addWidget(expenseTable);

        // Analytics Section
        QGroupBox* analyticsGroup = new QGroupBox("Analytics");
        QVBoxLayout* analyticsLayout = new QVBoxLayout;
        totalLabel = new QLabel("Total Expenses: $0.00");
        analyticsLayout->addWidget(totalLabel);
        analyticsGroup->setLayout(analyticsLayout);
        mainLayout->addWidget(analyticsGroup);

        // Connect signals
        connect(addButton, &QPushButton::clicked, this, &FinanceTrackerGUI::addExpense);

        // Set window properties
        setWindowTitle("Personal Finance Tracker");
        resize(800, 600);
    }

    void loadExpenses() {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(',');
            if (fields.size() >= 4) {
                expenses.emplace_back(fields[0], fields[1], 
                                    fields[2].toDouble(), fields[3]);
            }
        }
        file.close();
        updateTable();
        updateAnalytics();
    }

    void saveExpenses() {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file);
        for (const auto& exp : expenses) {
            out << exp.serialize() << "\n";
        }
        file.close();
    }

    void updateTable() {
        expenseTable->setRowCount(0);
        for (const auto& exp : expenses) {
            int row = expenseTable->rowCount();
            expenseTable->insertRow(row);
            expenseTable->setItem(row, 0, new QTableWidgetItem(exp.date));
            expenseTable->setItem(row, 1, new QTableWidgetItem(exp.category));
            expenseTable->setItem(row, 2, new QTableWidgetItem(
                QString("$%1").arg(exp.amount, 0, 'f', 2)));
            expenseTable->setItem(row, 3, new QTableWidgetItem(exp.description));
        }
    }

    void updateAnalytics() {
        double total = 0.0;
        std::map<QString, double> categoryTotals;

        for (const auto& exp : expenses) {
            total += exp.amount;
            categoryTotals[exp.category] += exp.amount;
        }

        QString analyticsText = QString("Total Expenses: $%1\n\nCategory Breakdown:")
                                      .arg(total, 0, 'f', 2);
        
        for (const auto& cat : categoryTotals) {
            double percentage = (cat.second / total) * 100;
            analyticsText += QString("\n%1: $%2 (%3%)")
                            .arg(cat.first)
                            .arg(cat.second, 0, 'f', 2)
                            .arg(percentage, 0, 'f', 1);
        }

        totalLabel->setText(analyticsText);
    }

private slots:
    void addExpense() {
        bool ok;
        double amount = amountEdit->text().toDouble(&ok);
        
        if (!ok || amount <= 0) {
            QMessageBox::warning(this, "Invalid Input", 
                               "Please enter a valid positive amount.");
            return;
        }

        expenses.emplace_back(
            dateEdit->date().toString("yyyy-MM-dd"),
            categoryCombo->currentText(),
            amount,
            descriptionEdit->text()
        );

        saveExpenses();
        updateTable();
        updateAnalytics();

        // Clear input fields
        dateEdit->setDate(QDate::currentDate());
        amountEdit->clear();
        descriptionEdit->clear();
    }

public:
    FinanceTrackerGUI(const QString& fname, QWidget* parent = nullptr)
        : QMainWindow(parent), filename(fname)
    {
        setupUI();
        loadExpenses();
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    FinanceTrackerGUI tracker("expenses.csv");
    tracker.show();
    return app.exec();
}

#include "main.moc"
