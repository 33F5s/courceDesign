#include "../include/widget.h"
#include "ui_widget.h"
#include "../include/dialog.h"
#include "../include/userwidget.h"
#include "../include/rootwidget.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>

enum page{loginPageNum=0,userPageNum,rootPageNum};

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->resize(640,480);

    QFile info("../serverInfo.txt");
    info.open(QIODevice::ReadOnly);
    QByteArray getInfo[5];
    int i=0;
    while(!info.atEnd()){
        getInfo[i]=info.readLine();
        getInfo[i] = getInfo[i].mid(0,getInfo[i].lastIndexOf('\n'));
        i++;
    }

    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(getInfo[0]);
    db.setPort(getInfo[1].toInt());
    db.setUserName(getInfo[2]);
    db.setPassword(getInfo[3]);
    db.setDatabaseName(getInfo[4]);

    if(db.open()){
        qDebug()<<"open success";
        emit sendDB(db);
    }

    ui->pushButton_user->setText("用户登录");
    ui->pushButton_root->setText("管理员登陆");
    ui->pushButton_sub->setText("新用户注册");

    //注册
    connect(ui->pushButton_sub,&QPushButton::clicked,this,&Widget::subSlot);

    //普通用户部分
    userWidget *userPage= new userWidget(this);
    ui->stackedWidget->insertWidget(userPageNum,userPage);
    connect(userPage,&userWidget::backHome,[&]{
        ui->stackedWidget->setCurrentIndex(loginPageNum);
        this->resize(640,480);
    });
    connect(this,&Widget::sendDB,userPage,&userWidget::receviceDB);
    connect(this,&Widget::sendUser,userPage,&userWidget::receviceUser);

    //root用户部分
    rootWidget *rootPage= new rootWidget(this);
    ui->stackedWidget->insertWidget(rootPageNum,rootPage);
    connect(rootPage,&rootWidget::backHome,[&]{
        ui->stackedWidget->setCurrentIndex(loginPageNum);
        this->resize(640,480);
    });
    connect(this,&Widget::sendDB,rootPage,&rootWidget::receviceDB);
    connect(this,&Widget::sendRootUser,rootPage,&rootWidget::receviceRootUser);

   connect(ui->pushButton_user,&QPushButton::clicked,this,&Widget::userSlot);
   connect(ui->pushButton_root,&QPushButton::clicked,this,&Widget::rootSlot);
}

Widget::~Widget()
{
    delete ui;
    db.close();
}

//注册 槽函数
void Widget::subSlot(){
    Dialog dialog(Register,this);
    dialog.exec();
}

void Widget::userSlot(){
    Dialog dialog(Login,this);
    if (dialog.exec() == QDialog::Accepted) {
        ui->stackedWidget->setCurrentIndex(userPageNum);
        emit sendUser(dialog.admin());
        this->resize(1280,720);
    }
}

void Widget::rootSlot(){
    Dialog dialog(Login,this,1);
    if (dialog.exec() == QDialog::Accepted) {
        ui->stackedWidget->setCurrentIndex(rootPageNum);
        emit sendRootUser(dialog.admin(),dialog.getPermission());
        this->resize(1280,720);
    }
}