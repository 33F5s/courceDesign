#ifndef ROOTWIDGET_H
#define ROOTWIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql/QtSql>
#include <QStandardItem>
#include <QVector>
#include <QVBoxLayout>
#include <QWidget>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QMouseEvent>
#include "../include/option.h"
#include "../include/mydialog.h"

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
    void receviceRootUser(QString user,int permission);
private slots:
    void userSlot(bool b);
    void borrowSlot(bool b);
    void bookSlot(bool b);
    void genreSlot(bool b);

    void subSlot();
    void cPasswdSlot();
    void delUser(); //注销用户槽函数
    void returnBookSlot();

    void comboBoxUserSlot(int index);   //用户类型筛选
    void comboBoxBorrowSlot(int index); //借与未借书筛选

    void minBnumberSlot();  //书的最少数量
    void maxBnumberSlot();  //最多数量

    void minFloorSlot();    //最低楼层（genre）
    void maxFloorSlot();    //最高楼层（genre）

signals:
    void backHome();
private:
    Ui::rootWidget *ui;
    QSqlDatabase db;
    QString rootUser;
    int userPermission;

    QComboBox *comboBox_userKind;
    QComboBox *comboBox_borrowed;

    QComboBox *comboBox_bOrder;
    QComboBox *combobox_borrowState;
    QComboBox *orderColumn;
    QComboBox *orderWay;
    QDateTimeEdit *borrowDateStart;
    QDateTimeEdit *borrowDateEnd;
    QDateTimeEdit *returnDateStart;
    QDateTimeEdit *returnDateEnd;
    
    QStandardItemModel *model;
    QVector<QStringList>tableHead;
    QVector<QString>tableName;

    QString command;    //记录筛选条件

    QWidget *userOptionWidget;  //user表的筛选条件窗口
    QWidget *bookOptionWidget;  //book表的筛选条件窗口
    QWidget *genreOptionWidget;  //genre表的筛选条件窗口
    QWidget *borrowOptionWidget;  //borrow表的筛选条件窗口

    tableNum tableNow;

    QLineEdit *lineEdit_minBNumer;  //book的数量的最小和最大值输入框
    QLineEdit *lineEdit_maxBNumer;

    QLineEdit *lineEdit_minFloor;  //genre楼层
    QLineEdit *lineEdit_maxFloor;

    struct option::userOption uOption;  //记录user表查询筛选条件
    struct option::bookOption bkOption; //记录book表查询筛选条件
    struct option::genreOption gOption;//记录genre表查询筛选条件
    struct option::borrowOption boOption;//记录borrow表查询筛选条件
    void getUserCommand();      //获取user表的筛选条件
    void getBookCommand();      //获取book表筛选条件
    void getgenreCommand();    //获取genre表筛选条件
    void getBorrowCommand();    //获取borrow表筛选条件

    void getCommand();  //获取当前表的筛选条件

    void initRoot();

    void selectTable();
    void setOptionWidget();
};

#endif // ROOTWIDGET_H
