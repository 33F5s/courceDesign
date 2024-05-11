#include "../include/rootwidget.h"
#include "ui_rootwidget.h"
#include <QCheckBox>
#include <QButtonGroup>
#include <QSqlQuery>

rootWidget::rootWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rootWidget)
{
    ui->setupUi(this);

    ui->label->setText("选择表");

    ui->pushButton_home->setText("退出");
    ui->pushButton_re->setText("刷新");
    ui->pushButton_search->setText("搜索");

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(ui->checkBox_book);
    group->addButton(ui->checkBox_borrow);
    group->addButton(ui->checkBox_grenre);
    group->addButton(ui->checkBox_user);
    ui->checkBox_user->setChecked(true);

    tableName.push_back("user");
    tableName.push_back("book");
    tableName.push_back("borrow");
    tableName.push_back("grenre");

    QStringList list;
    list<<"用户名"<<"权限"<<"借书情况";
    tableHead.push_back(list);
    list.clear();
    list<<"编号"<<"书名"<<"出版社"<<"作者"<<"余量"<<"种类";
    tableHead.push_back(list);
    list.clear();
    list<<"id"<<"账户"<<"书编号"<<"借出时间"<<"已借天数"<<"还书时间"<<"欠费";
    tableHead.push_back(list);
    list.clear();
    list<<"种类"<<"类型"<<"所在楼层";
    tableHead.push_back(list);

    model = new QStandardItemModel(this);
    ui->tableView->setModel(model);
    model->setHorizontalHeaderLabels(tableHead[userNum]);

    //返回主界面
    connect(ui->pushButton_home,&QPushButton::clicked,this,&rootWidget::backHome);

    //四个checkBox状态切换
    connect(ui->checkBox_user,&QCheckBox::toggled,this,&rootWidget::userSlot);
    connect(ui->checkBox_book,&QCheckBox::toggled,this,&rootWidget::bookSlot);
    connect(ui->checkBox_borrow,&QCheckBox::toggled,this,&rootWidget::borrowSlot);
    connect(ui->checkBox_grenre,&QCheckBox::toggled,this,&rootWidget::grenreSlot);
}

rootWidget::~rootWidget()
{
    delete ui;
}

void rootWidget::receviceDB(QSqlDatabase db){
    this->db = db;
}

void rootWidget::receviceRootUser(QString user){
    this->rootUser=user;

    ui->checkBox_user->setChecked(true);

    //用户表不显示密码
    QSqlQuery qry;
    qry.exec("select name,permission,borrowBooks from user");
    int i = 0;
    while(qry.next()){
        for(int j=0;j<tableHead[userNum].length();j++){
            model->setItem(i,j,new QStandardItem(qry.value(j).toString()));
        }
        i++;
    }
}

void rootWidget::selectTable(){
    model->clear();
    model->setHorizontalHeaderLabels(tableHead[tableNow]);

    //用户表不显示密码
    QSqlQuery qry;
    if(tableNow == userNum)
        qry.exec("select name,permission,borrowBooks from user");
    else 
        qry.exec(QString("select * from %1").arg(tableName[tableNow]));
    int i = 0;
    while(qry.next()){
        for(int j=0;j<tableHead[tableNow].length();j++){
            model->setItem(i,j,new QStandardItem(qry.value(j).toString()));
        }
        i++;
    }
}

void rootWidget::userSlot(bool b){
    if(!b)return;
    tableNow = userNum;
    selectTable();
}

void rootWidget::borrowSlot(bool b){
    if(!b)return;
    tableNow = borrowNum;
    selectTable();
}

void rootWidget::bookSlot(bool b){
    if(!b)return;
    tableNow = bookNum;
    selectTable();
}

void rootWidget::grenreSlot(bool b){
    if(!b)return;
    tableNow = grenreNum;
    selectTable();
}