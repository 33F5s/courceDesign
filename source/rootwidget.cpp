#include "../include/rootwidget.h"
#include "ui_rootwidget.h"

rootWidget::rootWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rootWidget)
{
    ui->setupUi(this);

    ui->pushButton_home->setText("退出");
    connect(ui->pushButton_home,&QPushButton::clicked,this,&rootWidget::backHome);
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
}
