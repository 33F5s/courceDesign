#include "../include/rootwidget.h"
#include "ui_rootwidget.h"
#include <QCheckBox>
#include <QButtonGroup>
#include <QSqlQuery>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include "../include/dialog.h"

rootWidget::rootWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rootWidget)
{
    ui->setupUi(this);

    ui->label->setText("选择表");
    ui->label_search->setText("输入用户名查找:");

    //user表对应功能菜单
    uOption={
        .type=option::bothUser,
        .state=option::bothState
        };
    userOptionWidget = new QWidget(this);
    userOptionWidget->setGeometry(1040,220,200,150);
    QVBoxLayout *userLayout = new QVBoxLayout(userOptionWidget);
    QHBoxLayout *userTypeLayout = new QHBoxLayout();
    QHBoxLayout *borrowStateLayout = new QHBoxLayout();
    QComboBox *comboBox_userKind = new QComboBox(userOptionWidget);
    QComboBox *comboBox_borrowed = new QComboBox(userOptionWidget);
    //qDebug()<<comboBox_userKind->size();
    comboBox_userKind->addItem("管理员/普通");
    comboBox_userKind->addItem("普通用户");
    comboBox_userKind->addItem("管理员");
    userTypeLayout->addWidget(new QLabel("用户类型"));
    userTypeLayout->addWidget(comboBox_userKind);       //标签和combobox的layout
    comboBox_borrowed->addItem("已借/未借");
    comboBox_borrowed->addItem("已借书");
    comboBox_borrowed->addItem("未借书");
    borrowStateLayout->addWidget(new QLabel("借书状态"));
    borrowStateLayout->addWidget(comboBox_borrowed);    //标签和combobox的layout
    userLayout->addLayout(userTypeLayout);
    userLayout->addLayout(borrowStateLayout);
    connect(comboBox_userKind,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&rootWidget::comboBoxUserSlot);
    connect(comboBox_borrowed,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&rootWidget::comboBoxBorrowSlot);

    //book表对应功能菜单
    bkOption={
        .minCnt=0,
        .maxCnt=999
    };
    bookOptionWidget = new QWidget(this);
    bookOptionWidget->setGeometry(1040,220,230,100);
    QVBoxLayout *bookLayout = new QVBoxLayout(bookOptionWidget);
    QHBoxLayout *bNumberRangeLayout = new QHBoxLayout();
    lineEdit_minBNumer = new QLineEdit(bookOptionWidget);
    lineEdit_minBNumer->setValidator(new QRegExpValidator(QRegExp("[0-9]{3}"))); //限制只能输入数字（三位）
    lineEdit_minBNumer->setText("0");
    lineEdit_maxBNumer = new QLineEdit(bookOptionWidget);
    lineEdit_maxBNumer->setValidator(new QRegExpValidator(QRegExp("[0-9]{3}"))); //限制只能输入数字（三位）
    lineEdit_maxBNumer->setText("999");
    bNumberRangeLayout->addWidget(lineEdit_minBNumer);
    bNumberRangeLayout->addWidget(new QLabel("到"));
    bNumberRangeLayout->addWidget(lineEdit_maxBNumer);
    bookLayout->addWidget(new QLabel("输入书籍数量范围(上限999):"));
    bookLayout->addLayout(bNumberRangeLayout);
    connect(lineEdit_minBNumer,&QLineEdit::textEdited,this,&rootWidget::minBnumberSlot);
    connect(lineEdit_maxBNumer,&QLineEdit::textEdited,this,&rootWidget::maxBnumberSlot);

    //grenre表对应功能菜单
    gOption={
        .minFloor=1,
        .maxFloor=4
    };
    grenreOptionWidget = new QWidget(this);
    grenreOptionWidget->setGeometry(1040,220,230,100);
    QVBoxLayout *grenreLayout = new QVBoxLayout(grenreOptionWidget);
    QHBoxLayout *bookFloorLayout = new QHBoxLayout();
    lineEdit_minFloor = new QLineEdit(grenreOptionWidget);
    lineEdit_minFloor->setValidator(new QRegExpValidator(QRegExp("[1-4]{1}"))); //限制只能输入数字（1位）
    lineEdit_minFloor->setText("1");
    lineEdit_maxFloor = new QLineEdit(grenreOptionWidget);
    lineEdit_maxFloor->setValidator(new QRegExpValidator(QRegExp("[1-4]{1}"))); //限制只能输入数字（1位）
    lineEdit_maxFloor->setText("4");
    bookFloorLayout->addWidget(lineEdit_minFloor);
    bookFloorLayout->addWidget(new QLabel("到"));
    bookFloorLayout->addWidget(lineEdit_maxFloor);
    grenreLayout->addWidget(new QLabel("输入楼层范围(共4楼):"));
    grenreLayout->addLayout(bookFloorLayout);

    ui->pushButton_home->setText("退出");
    ui->pushButton_re->setText("刷新");
    ui->pushButton_cPasswd->setText("重置密码");
    ui->pushButton_sub->setText("注册用户");

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
    list<<"用户名"<<"权限"<<"借书单号";
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
    //刷新
    connect(ui->pushButton_re,&QPushButton::clicked,this,&rootWidget::selectTable);
    //注册用户
    connect(ui->pushButton_sub,&QPushButton::clicked,this,&rootWidget::subSlot);
    //重置用户密码
    connect(ui->pushButton_cPasswd,&QPushButton::clicked,this,&rootWidget::cPasswdSlot);
}

rootWidget::~rootWidget()
{
    delete ui;
}

void rootWidget::setOptionWidget(){
    
}

void rootWidget::receviceDB(QSqlDatabase db){
    this->db = db;
}

void rootWidget::receviceRootUser(QString user){
    this->rootUser=user;
    tableNow=userNum;

    ui->checkBox_user->setChecked(true);

    //显示用户表与操作菜单
    selectTable();
    setOptionWidget();
}

void rootWidget::selectTable(){
    model->clear();
    model->setHorizontalHeaderLabels(tableHead[tableNow]);

    userOptionWidget->hide();
    bookOptionWidget->hide();
    grenreOptionWidget->hide();
    switch (tableNow)
    {
    case userNum:
        userOptionWidget->show();break;
    case bookNum:
        bookOptionWidget->show();break;
    case grenreNum:
        grenreOptionWidget->show();break;
    default:
        break;
    }

    //用户表不显示密码
    QSqlQuery qry;
    QString c;  //sql查询语句
    if(tableNow == userNum)
        c="select name,permission,borrowBook from user";
    else 
        c=QString("select * from %1").arg(tableName[tableNow]);
            //获取当前的筛选条件
    getCommand();
    if(command.size()!=0) {
        c+=" where ";
        c+=command;
    }
    qDebug()<<c;
    qry.exec(c);
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
    ui->label_search->setText("输入用户名查找");
    selectTable();
}

void rootWidget::borrowSlot(bool b){
    if(!b)return;
    tableNow = borrowNum;
    ui->label_search->setText("输入用户名查找");
    selectTable();
}

void rootWidget::bookSlot(bool b){
    if(!b)return;
    tableNow = bookNum;
    ui->label_search->setText("输入书名查找");
    selectTable();
}

void rootWidget::grenreSlot(bool b){
    if(!b)return;
    tableNow = grenreNum;
    ui->label_search->setText("输入类型查找");
    selectTable();
}

//注册用户槽函数
void rootWidget::subSlot(){
    QStringList item;
    bool ok;
    int permission;
    item<<"管理员"<<"普通用户";
    QString type = QInputDialog::getItem(this,"注册","选择账号类型",item,0,true,&ok);
    if(!ok)return;

    if(type==item[0]){
        permission=1;
    }
    else if(type==item[1]){
        permission=0;
    }

    Dialog dialog (1,this);
    if(dialog.exec()!=QDialog::Accepted)return;
    if(!dialog.confirmPasswd()){
        QMessageBox::warning(this,"警告","两次密码不一致");
        return;
    }
    
    QString admin = dialog.admin();
    QString passwd = dialog.passwd();
    if(admin.size()==0 || passwd.size()==0){
        QMessageBox::warning(this,"警告","用户名或密码为空");
        return;
    }
    QSqlQuery qry;
    if(qry.exec(QString("insert into user (name,passwd,permission) values ('%1','%2',%3)").arg(admin).arg(passwd).arg(permission))){
        QMessageBox::information(this,"提示","注册成功");
    }
    else{
        QMessageBox::information(this,"提示","注册失败（账号名重复）");
    }
    
}
//重置用户密码
void rootWidget::cPasswdSlot(){
    Dialog dialog(1,this);
    dialog.setWindowTitle("重置密码");
    if(dialog.exec()!=QDialog::Accepted)return;
    if(!dialog.confirmPasswd()){
        QMessageBox::warning(this,"警告","两次密码不一致");
        return;
    }
    QString admin = dialog.admin();
    QString passwd = dialog.passwd();
    if(admin.size()==0 || passwd.size()==0){
        QMessageBox::warning(this,"警告","用户名或密码为空");
        return;
    }
    QSqlQuery qry;
    qry.exec(QString("select name,permission from user where name='%1'").arg(admin));
    qry.next();
    if(!qry.isValid()){
        QMessageBox::information(this,"提示","无该账号");
        return;
    }
    if(qry.value(0).toString()!=rootUser && qry.value(1).toInt()>0){
        QMessageBox::warning(this,"警告","该账号为其他管理员账号,不可更改");
        return;
    }
    if(qry.exec(QString("update user set passwd='%1' where name='%2'").arg(passwd).arg(admin))){
        QMessageBox::information(this,"提示","重置成功");
    }
    else{
        QMessageBox::information(this,"提示","重置失败");
    }
}

void rootWidget::comboBoxUserSlot(int index){
    //qDebug()<<index;
    switch(index){
        case 0:
            uOption.type=option::bothUser;
            break;
        case 1:
            uOption.type=option::normalUser;
            break;
        case 2:
            uOption.type=option::rootUser;
            break;
        default:break;
    }
    selectTable();
}

void rootWidget::comboBoxBorrowSlot(int index){
    //qDebug()<<index;
    switch(index){
        case 0:
            uOption.state=option::bothState;
            break;
        case 1:
            uOption.state=option::borrowed;
            break;
        case 2:
            uOption.state=option::returned;
            break;
        default:break;
    }
    selectTable();
}

void rootWidget::getCommand(){
    command.clear();
    switch(tableNow){
        case userNum:
            getUserCommand();
            break;
        case bookNum:
            getBookCommand();
            break;
        case grenreNum:
            getGrenreCommand();
            break;
        default:
            break;
    }
}

void rootWidget::getUserCommand(){
    switch(uOption.type){
        case option::bothUser:
            break;
        case option::normalUser:
            command+=" permission=0 ";
            break;
        case option::rootUser:
            command+=" permission=1 ";
            break;
        default:
            break;
    }
    if(!command.isNull())command+=" and ";
    switch(uOption.state){
        case option::bothState:
            command=command.mid(0,command.length()-sizeof(" and ")); //删除and
            break;
        case option::borrowed:
            command+=" borrowBook is not null ";
            break;
        case option::returned:
            command+=" borrowBook is null ";
            break;
        default:
            break;
    }
    if(!ui->lineEdit_search->text().isEmpty())
        command+=QString(" and name like '%1%'").arg(ui->lineEdit_search->text());
}

void rootWidget::getBookCommand(){
   command=QString(" bnumber>=%1 and bnumber<=%2").arg(bkOption.minCnt).arg(bkOption.maxCnt);
   if(!ui->lineEdit_search->text().isEmpty())
        command+=QString(" and bname like '%1%'").arg(ui->lineEdit_search->text());
}

//获取最小值
void rootWidget::minBnumberSlot(){
    int min = lineEdit_minBNumer->text().toInt();
    int max = lineEdit_maxBNumer->text().toInt();
    if(min>max){
        QMessageBox::information(this,"提示","最小值大于最大值");
        lineEdit_minBNumer->setText(QString::number(bkOption.minCnt)); //设置为之前的值
        return;
    }
    bkOption.minCnt=min;
}
//获取最大值
void rootWidget::maxBnumberSlot(){
    int min = lineEdit_minBNumer->text().toInt();
    int max = lineEdit_maxBNumer->text().toInt();
    if(min>max){
        QMessageBox::information(this,"提示","最小值大于最大值");
        lineEdit_maxBNumer->setText(QString::number(bkOption.maxCnt)); //设置为之前的值
        return;
    }
    bkOption.maxCnt=max;
}

void rootWidget::getGrenreCommand(){

}