#ifndef ROOTWIDGET_H
#define ROOTWIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql/QtSql>
#include <QStandardItem>
#include <QVector>

namespace Ui {
class rootWidget;
}

typedef enum tableNum{userNum=0,bookNum,borrowNum,grenreNum}tablNum;

class rootWidget : public QWidget
{
    Q_OBJECT

public:
    explicit rootWidget(QWidget *parent = nullptr);
    ~rootWidget();
public slots:
    void receviceDB(QSqlDatabase db);
    void receviceRootUser(QString user);
private slots:
    void userSlot(bool b);
    void borrowSlot(bool b);
    void bookSlot(bool b);
    void grenreSlot(bool b);
signals:
    void backHome();
private:
    Ui::rootWidget *ui;
    QSqlDatabase db;
    QString rootUser;
    
    QStandardItemModel *model;
    QVector<QStringList>tableHead;
    QVector<QString>tableName;

    tableNum tableNow;

    void selectTable();
};

#endif // ROOTWIDGET_H
