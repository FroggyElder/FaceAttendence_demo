#include "dbquerypanel.h"
#include "ElaComboBox.h"
#include "ElaPushButton.h"
#include "ElaLineEdit.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QSqlTableModel>
#include <QSqlError>
#include <qsqlquery.h>
#include <QSqlDatabase>
#include <QTableView>

#include "QFileDialog"
#include <QThread>
#include <QApplication>

#include "xlsxdocument.h"

dbQueryPanel::dbQueryPanel(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout* mainLayout {new QVBoxLayout{this}};
    QHBoxLayout* inputLayout{new QHBoxLayout{}};
    mainLayout->addLayout(inputLayout);

    m_pickDBBox = {new ElaComboBox{this}};
    m_pickDBBox->addItem("attendLog");
    m_pickDBBox->addItem("staffInfo");
    inputLayout->addWidget(m_pickDBBox);

    ElaPushButton* refreshBtn{new ElaPushButton{this}};
    refreshBtn->setText("refresh");
    inputLayout->addWidget(refreshBtn);
    connect (refreshBtn,&ElaPushButton::clicked,this,&dbQueryPanel::getTable);

    ElaPushButton* exportBtn{new ElaPushButton{this}};
    exportBtn->setText("export");
    inputLayout->addWidget(exportBtn);
    connect(exportBtn,&QPushButton::clicked,this,&dbQueryPanel::exportTable);

    m_filterLayout = new QHBoxLayout{};
    mainLayout->addLayout(m_filterLayout);

    m_outTable = {new QTableView{this}};
    m_outTable->setSortingEnabled(true);
    mainLayout->addWidget(m_outTable);

    connect(m_pickDBBox,&ElaComboBox::currentTextChanged,this,&dbQueryPanel::setTable);
    emit m_pickDBBox->currentTextChanged(m_pickDBBox->currentText());
}

dbQueryPanel::~dbQueryPanel()
{

}

void dbQueryPanel::setTable(const QString &tableName)
{
    qDebug() << "switching to table:" << tableName;
    m_tableName = tableName;
}

void dbQueryPanel::getTable()
{
    if(!m_tableModel)
    {
        // Wait until the database is opened
        while (!QSqlDatabase::database().isOpen()) {
            QThread::msleep(100); // Sleep for 100 milliseconds
            QApplication::processEvents(); // Allow the application to process other events
        }
        m_tableModel = new QSqlTableModel{this};
    }

    m_tableModel->setTable(m_tableName);

    qDebug() << m_tableModel->database();

    QString tmp_filter{};
    // Iterate over children of m_filterLayout and create filter string
    for (int i = 0; i < m_filterLayout->count(); ++i)
    {
        QLayoutItem *item = m_filterLayout->itemAt(i);
        if (item) {
            QWidget *widget = item->widget();
            auto lineEdit = qobject_cast<ElaLineEdit*>(widget);
            if (lineEdit) {
                if (lineEdit->text().isEmpty()) continue;

                // Create filter string
                if (!tmp_filter.isEmpty()) tmp_filter += " AND ";
                tmp_filter += QString("%1 LIKE '%%2%'")
                                  .arg(lineEdit->placeholderText())
                                  .arg(lineEdit->text());
            } else {
                qDebug() << "Widget is not an ElaLineEdit";
            }
        }
    }

    m_tableModel->setFilter(tmp_filter);

    m_tableModel->select();


    if(m_tableModel->lastError().isValid())
    {
        qDebug() << m_tableModel->lastError().text();
    }
    else
    {
        m_outTable->setModel(m_tableModel);
    }

    //clear everything under m_filterLayout
    QLayoutItem *item;
    while ((item = m_filterLayout->takeAt(0)) != nullptr) {
        delete item->widget();  // Delete the widget
        delete item;            // Delete the layout item
    }

    // Add filter widgets
    for (int i = 0; i < m_tableModel->columnCount(); ++i)
    {
        ElaLineEdit* filterEdit = new ElaLineEdit{this};
        filterEdit->setPlaceholderText(m_tableModel->headerData(i, Qt::Horizontal).toString());
        m_filterLayout->addWidget(filterEdit);
    }
}

void dbQueryPanel::exportTable()
{
    QString fileName = QFileDialog::getSaveFileName
        (this, "Save File", m_pickDBBox->currentText() , "Excel Files (*.xlsx)");
    if (fileName.isEmpty()) return;

    QXlsx::Document xlsx;

    QAbstractItemModel* model = m_outTable->model();

    // Write headers
    for (int col = 0; col < model->columnCount(); ++col) {
        xlsx.write(1, col + 1, model->headerData(col, Qt::Horizontal).toString());
    }

    // Write data
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0; col < model->columnCount(); ++col) {
            xlsx.write(row + 2, col + 1, model->data(model->index(row, col)).toString());
        }
    }

    // Save the file
    if (xlsx.saveAs(fileName)) {
        qDebug() << "Exported successfully!";
    } else {
        qDebug() << "Failed to export.";
    }
}
