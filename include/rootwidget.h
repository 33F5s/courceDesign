#ifndef ROOTWIDGET_H
#define ROOTWIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql/QtSql>
#include <QStandardItem>

namespace Ui {
class rootWidget;
}

class rootWidget : public QWidget
{
    Q_OBJECT

public:
    explicit rootWidget(QWidget *parent = nullptr);
    ~rootWidget();
public slots:
    void receviceDB(QSqlDatabase db);
    void receviceRootUser(QString user);
signals:
    void backHome();
private:
    Ui::rootWidget *ui;
    QSqlDatabase db;
    QString rootUser;
};

#endif // ROOTWIDGET_H
