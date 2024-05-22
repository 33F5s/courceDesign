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
    Dialog dialog(1,this);
    if(dialog.exec() == Dialog::Accepted){
        bool confirm = dialog.confirmPasswd();
        if(!confirm){
            QMessageBox::warning(this,"警告","两次密码不一致");
            return ;
        }
        QString admin = dialog.admin();
        QString passwd = dialog.passwd();
        QSqlQuery qry;
        if(!qry.exec(QString("insert into user(name,passwd,permission) values ('%1','%2',0)").arg(admin).arg(passwd))){
            QMessageBox::warning(this,"警告","账户名已存在");
            return;
        }
        QMessageBox::information(this,"提示","注册成功");
        //qDebug()<<QString("insert into user values ('%1','%2',0)").arg(admin).arg(passwd);
    }
}

void Widget::userSlot(){
    Dialog dialog(0,this);
    if (dialog.exec() == QDialog::Accepted) {
        QString admin = dialog.admin();
        QString passwd = dialog.passwd();
        if(admin.size()==0 || passwd.size()==0){
            QMessageBox::warning(this,"警告","用户名或密码为空");
            return;
        }
        QSqlQuery qry;
        qry.exec(QString("select permission from user where name='%1' and passwd='%2';").arg(admin).arg(passwd));
        if(qry.next()){
            ui->stackedWidget->setCurrentIndex(userPageNum);
            emit sendUser(admin);
            this->resize(1280,720);
        }
        else{
            QMessageBox::warning(this,"登录错误","请检查用户名与密码或进行注册");
        }
    }
}

void Widget::rootSlot(){
    Dialog dialog(0,this);
    if (dialog.exec() == QDialog::Accepted) {
        QString admin = dialog.admin();
        QString passwd = dialog.passwd();
        QSqlQuery qry;
        // qry.exec(QString("select permission from user where name='%1' and passwd='%2';").arg(admin).arg(passwd));
        // if(qry.next()){
        //     if(qry.value(0).toInt()<1)
        //         QMessageBox::warning(this,"权限错误","该用户无管理员权限");
        //     else
                ui->stackedWidget->setCurrentIndex(rootPageNum);
                emit sendRootUser(admin);
                this->resize(1280,720);
        // }
        // else{
        //     QMessageBox::warning(this,"登录错误","请检查用户名与密码");
        // }
    }
}