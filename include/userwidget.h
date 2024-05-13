#ifndef USERWIDGET_H
#define USERWIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql/QtSql>
#include <QStandardItem>

namespace Ui {
class userWidget;
}

class userWidget : public QWidget
{
    Q_OBJECT

public:
    explicit userWidget(QWidget *parent = nullptr);
    ~userWidget();
signals:
    void backHome();
public slots:
    void receviceDB(QSqlDatabase db);
    void receviceUser(QString user);
private slots:
    void borrowShowSlot();
    void searchSlot(const QString &str);
    void borrowDoubleClickedSlot(const QModelIndex &index);
    void reSlot();
    void returnSlot();
    void deleteSlot();
    void backSlot();
private:
    Ui::userWidget *ui;
    QSqlDatabase db;
    //显示书库存
    QStandardItemModel *borrow_model;
    //显示未还书
    QStandardItemModel *return_model;
    QStringList borrow_headList;
    QStringList return_headList;

    QString user;
};

#endif // USERWIDGET_H
