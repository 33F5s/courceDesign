#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSql/QtSql>
#include <QtSql/qsqldriver.h>
#include <QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QStandardItem>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
signals:
    void sendDB(QSqlDatabase db);
    void sendUser(QString name);
    void sendRootUser(QString name,int permission);
private slots:
    void userSlot();
    void rootSlot();
    void subSlot();
private:
    Ui::Widget *ui;
    QSqlDatabase db;
};

#endif // WIDGET_H
