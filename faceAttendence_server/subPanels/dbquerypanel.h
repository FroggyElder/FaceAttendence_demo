#ifndef DBQUERYPANEL_H
#define DBQUERYPANEL_H

#include <QWidget>
class QTableView;
class QHBoxLayout;
class ElaComboBox;

#include <QSqlTableModel>


class dbQueryPanel : public QWidget
{
    Q_OBJECT
private:
    QTableView* m_outTable{};
    QString m_tableName{};
    QHBoxLayout* m_filterLayout{};
    ElaComboBox* m_pickDBBox{};
    QSqlTableModel* m_tableModel{};

public:
    explicit dbQueryPanel(QWidget *parent = nullptr);
    ~dbQueryPanel();

signals:

private slots:
    void setTable (const QString& tableName);
    void getTable ();
    void exportTable ();
};

#endif // DBQUERYPANEL_H
