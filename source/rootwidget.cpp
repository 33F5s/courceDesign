#include "../include/rootwidget.h"
#include "ui_rootwidget.h"
#include <QCheckBox>
#include <QButtonGroup>
#include <QSqlQuery>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include "../include/dialog.h"

rootWidget::rootWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rootWidget)
{
    ui->setupUi(this);

    ui->label->setText("选择表");
    ui->label_search->setText("输入用户名查找:");
    ui->label_search_2->setText("输入书编号查找");

    ui->pushButton_delete->hide();
    ui->pushButton_insert->hide();

    //user表对应功能菜单
    userOptionWidget = new QWidget(this);
    userOptionWidget->setGeometry(1040,220,200,150);
    QVBoxLayout *userLayout = new QVBoxLayout(userOptionWidget);
    QHBoxLayout *userTypeLayout = new QHBoxLayout();
    QHBoxLayout *userBorrowStateLayout = new QHBoxLayout();
    comboBox_userKind = new QComboBox(userOptionWidget);
    comboBox_borrowed = new QComboBox(userOptionWidget);
    //qDebug()<<comboBox_userKind->size();
    comboBox_userKind->addItem("管理员/普通");
    comboBox_userKind->addItem("普通用户");
    comboBox_userKind->addItem("管理员");
    userTypeLayout->addWidget(new QLabel("用户类型"));
    userTypeLayout->addWidget(comboBox_userKind);       //标签和combobox的layout
    comboBox_borrowed->addItem("已借/未借");
    comboBox_borrowed->addItem("已借书");
    comboBox_borrowed->addItem("未借书");
    userBorrowStateLayout->addWidget(new QLabel("借书状态"));
    userBorrowStateLayout->addWidget(comboBox_borrowed);    //标签和combobox的layout
    userLayout->addLayout(userTypeLayout);
    userLayout->addLayout(userBorrowStateLayout);
    connect(comboBox_userKind,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&rootWidget::comboBoxUserSlot);
    connect(comboBox_borrowed,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&rootWidget::comboBoxBorrowSlot);

    //book表对应功能菜单
    bookOptionWidget = new QWidget(this);
    bookOptionWidget->setGeometry(1040,220,230,100);
    QVBoxLayout *bookLayout = new QVBoxLayout(bookOptionWidget);
    QHBoxLayout *bNumberRangeLayout = new QHBoxLayout();    //书籍数量范围
    QHBoxLayout *bOrderLayout = new QHBoxLayout();          //排序方式
    comboBox_bOrder = new QComboBox(bookOptionWidget);
    comboBox_bOrder->addItem("升序");
    comboBox_bOrder->addItem("降序");
    bOrderLayout->addWidget(new QLabel("按数量排序"));
    bOrderLayout->addWidget(comboBox_bOrder);
    connect(comboBox_bOrder,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[&](int index){
        bkOption.order=(index==0)?option::ASC:option::DESC;
    });
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
    bookLayout->addLayout(bOrderLayout);
    connect(lineEdit_minBNumer,&QLineEdit::textEdited,this,&rootWidget::minBnumberSlot);
    connect(lineEdit_maxBNumer,&QLineEdit::textEdited,this,&rootWidget::maxBnumberSlot);

    //genre表对应功能菜单
    genreOptionWidget = new QWidget(this);
    genreOptionWidget->setGeometry(1040,220,230,100);
    QVBoxLayout *genreLayout = new QVBoxLayout(genreOptionWidget);
    QHBoxLayout *bookFloorLayout = new QHBoxLayout();
    lineEdit_minFloor = new QLineEdit(genreOptionWidget);
    lineEdit_minFloor->setValidator(new QRegExpValidator(QRegExp("[1-4]{1}"))); //限制只能输入数字（1位）
    lineEdit_minFloor->setText("1");
    lineEdit_maxFloor = new QLineEdit(genreOptionWidget);
    lineEdit_maxFloor->setValidator(new QRegExpValidator(QRegExp("[1-4]{1}"))); //限制只能输入数字（1位）
    lineEdit_maxFloor->setText("4");
    bookFloorLayout->addWidget(lineEdit_minFloor);
    bookFloorLayout->addWidget(new QLabel("到"));
    bookFloorLayout->addWidget(lineEdit_maxFloor);
    genreLayout->addWidget(new QLabel("输入楼层范围(共4楼):"));
    genreLayout->addLayout(bookFloorLayout);
    connect(lineEdit_minFloor,&QLineEdit::textChanged,this,&rootWidget::minFloorSlot);
    connect(lineEdit_maxFloor,&QLineEdit::textChanged,this,&rootWidget::maxFloorSlot);

    //borrow表对应菜单
    borrowOptionWidget = new QWidget(this);
    borrowOptionWidget->setGeometry(1035,220,250,250);
    QVBoxLayout *borrowLayout = new QVBoxLayout(borrowOptionWidget);

    QHBoxLayout *borrowStateLayout = new QHBoxLayout();                     //选择借书状态
    combobox_borrowState = new QComboBox(borrowOptionWidget);
    combobox_borrowState->addItem("所有");
    combobox_borrowState->addItem("已还");
    combobox_borrowState->addItem("已借未还");
    borrowStateLayout->addWidget(new QLabel("选择借书状态"));
    borrowStateLayout->addWidget(combobox_borrowState);
    connect(combobox_borrowState,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[&](int index){
        switch(index){
            case 0:
                boOption.state=option::allState;break;
            case 1:
                boOption.state=option::returned;break;
            case 2:
                boOption.state=option::NotReturned;break;
            default:break;
        }
    });

    QHBoxLayout *orderWayLayout = new QHBoxLayout();                        //选择排序方式
    orderColumn = new QComboBox(borrowOptionWidget);
    orderColumn->addItem("借书时间");
    orderColumn->addItem("还书时间");
    orderColumn->addItem("借出天数");
    orderColumn->addItem("需交金额");
    connect(orderColumn,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[&](int index){
        switch(index){
            case 0:
                boOption.o_column=option::C_BORROW_TIME;break;
            case 1:
                boOption.o_column=option::C_RETURN_TIME;break;
            case 2:
                boOption.o_column=option::C_TIME_CNT;break;
            case 3:
                boOption.o_column=option::C_MONEY;break;
            default:break;
        }
    });
    orderWay = new QComboBox(borrowOptionWidget);
    orderWay->addItem("升序");
    orderWay->addItem("降序");
    connect(orderWay,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[&](int index){
        boOption.order=(index==0)?option::ASC:option::DESC;
    });
    orderWayLayout->addWidget(new QLabel("按"));
    orderWayLayout->addWidget(orderColumn);
    orderWayLayout->addWidget(orderWay);
    orderWayLayout->addWidget(new QLabel("排序"));

    QHBoxLayout *borrowDateLayout = new QHBoxLayout();                       //借书时间范围
    borrowDateStart = new QDateTimeEdit(QDate(2024,5,1));   
    borrowDateStart->setDisplayFormat("yyyy-MM-dd");
    borrowDateStart->setCalendarPopup(true);
    connect(borrowDateStart,&QDateTimeEdit::dateChanged,[&](QDate date){
        boOption.borrowStartDate=date;
    });
    borrowDateEnd = new QDateTimeEdit(QDate::currentDate());
    borrowDateEnd->setDisplayFormat("yyyy-MM-dd");
    borrowDateEnd->setCalendarPopup(true);
    connect(borrowDateEnd,&QDateTimeEdit::dateChanged,[&](QDate date){
        boOption.borrowEndDate=date;
    });
    borrowDateLayout->addWidget(borrowDateStart);
    borrowDateLayout->addWidget(new QLabel("-"));
    borrowDateLayout->addWidget(borrowDateEnd);

    QHBoxLayout *returnDateLayout = new QHBoxLayout();                      //还书时间范围
    returnDateStart = new QDateTimeEdit(QDate(2024,5,1));    
    returnDateStart->setDisplayFormat("yyyy-MM-dd");
    returnDateStart->setCalendarPopup(true);
    connect(returnDateStart,&QDateTimeEdit::dateChanged,[&](QDate date){
        boOption.returnStartDate=date;
    });
    returnDateEnd = new QDateTimeEdit(QDate::currentDate());
    returnDateEnd->setDisplayFormat("yyyy-MM-dd");
    returnDateEnd->setCalendarPopup(true);
    connect(returnDateEnd,&QDateTimeEdit::dateChanged,[&](QDate date){
        boOption.returnEndDate=date;
    });
    returnDateLayout->addWidget(returnDateStart);
    returnDateLayout->addWidget(new QLabel("-"));
    returnDateLayout->addWidget(returnDateEnd);
    //returnDateEnd->setEnabled(false);

    borrowLayout->addLayout(borrowStateLayout);
    borrowLayout->addLayout(orderWayLayout);
    borrowLayout->addWidget(new QLabel("借书时间范围："));
    borrowLayout->addLayout(borrowDateLayout);
    borrowLayout->addWidget(new QLabel("已还书时间范围："));
    borrowLayout->addLayout(returnDateLayout);


    ui->pushButton_home->setText("退出");
    ui->pushButton_re->setText("刷新");
    ui->pushButton_cPasswd->setText("重置密码");
    ui->pushButton_sub->setText("注册用户");

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(ui->checkBox_book);
    group->addButton(ui->checkBox_borrow);
    group->addButton(ui->checkBox_genre);
    group->addButton(ui->checkBox_user);
    ui->checkBox_user->setChecked(true);

    tableName.push_back("user");
    tableName.push_back("book");
    tableName.push_back("borrow");
    tableName.push_back("genre");

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
    connect(ui->checkBox_genre,&QCheckBox::toggled,this,&rootWidget::genreSlot);
    //刷新
    connect(ui->pushButton_re,&QPushButton::clicked,this,&rootWidget::selectTable);
    //注册用户
    connect(ui->pushButton_sub,&QPushButton::clicked,this,&rootWidget::subSlot);
    //重置用户密码
    connect(ui->pushButton_cPasswd,&QPushButton::clicked,this,&rootWidget::cPasswdSlot);
    //注销用户
    connect(ui->pushButton_delUser,&QPushButton::clicked,this,&rootWidget::delUser);
    //还书
    connect(ui->pushButton_return,&QPushButton::clicked,this,&rootWidget::returnBookSlot);

    //添加条目
    connect(ui->pushButton_insert,&QPushButton::clicked,[&]{
        if(tableNow==userNum || tableNow==borrowNum)return;
        myDialog dialog(tableNow,0,this);
        if(dialog.exec()==QDialog::Accepted){
            selectTable();
        }
    });
    //修改条目
    connect(ui->tableView,&QTableView::doubleClicked,[&](const QModelIndex &index){
        if(tableNow==userNum || tableNow==borrowNum)return;
        //qDebug()<<index.row();
        QStringList list;
        for(int i = 0;i<model->columnCount();i++){
            list.append(model->data(model->index(index.row(),i)).toString());
        }
        myDialog dialog(tableNow,1,this,list);
        if(dialog.exec()==QDialog::Accepted){
            selectTable();
        }
    });
    //删除条目
    connect(ui->pushButton_delete,&QPushButton::clicked,[&]{
        if(tableNow==userNum)return;
        QStringList tableName,tablePK;
        tableName<<"user"<<"book"<<"borrow"<<"genre";
        tablePK<<"name"<<"bno"<<"id"<<"kind";
        QModelIndexList list = ui->tableView->selectionModel()->selectedRows();
        if(list.isEmpty()){
            QMessageBox::information(this,"提示","请先选中");
            return;
        }
        QSqlQuery qry;
        qry.prepare(QString("delete from %1 where %2=:pk").arg(tableName[tableNow]).arg(tablePK[tableNow]));
        foreach(const QModelIndex &index,list){
            qry.bindValue(":pk",model->data(model->index(index.row(),0)).toString());
            if(!qry.exec()){
                qDebug()<<qry.lastError().text();
                if(tableNow==borrowNum || tableNow==bookNum)
                    QMessageBox::warning(this,"警告","删除出错,请检查书籍是否有未归还记录");
                else
                    QMessageBox::warning(this,"警告","删除出错");
                return;
            }
        }
        QMessageBox::information(this,"提示","删除成功");
        selectTable();
    });
}

rootWidget::~rootWidget()
{
    delete ui;
}

void rootWidget::initRoot(){
    //初始筛选条件
    uOption={
        .type=option::bothUser,
        .state=option::allState
    };
    bkOption={
        .minCnt=0,
        .maxCnt=999,
        .order=option::ASC
    };
    gOption={
        .minFloor=1,
        .maxFloor=4
    };
    boOption={
        .state=option::allState,
        .o_column=option::C_BORROW_TIME,
        .order=option::ASC,
        .borrowStartDate=QDate(2024,5,1),
        .borrowEndDate=QDate(QDate::currentDate()),
        .returnStartDate=QDate(2024,5,1),
        .returnEndDate=QDate(QDate::currentDate()),
    };

    combobox_borrowState->setCurrentIndex(0);
    comboBox_bOrder->setCurrentIndex(0);
    comboBox_borrowed->setCurrentIndex(0);
    comboBox_userKind->setCurrentIndex(0);
    orderColumn->setCurrentIndex(0);
    orderWay->setCurrentIndex(0);

    borrowDateStart->setDate(QDate(2024,5,1));
    borrowDateEnd->setDate(QDate::currentDate());
    returnDateStart->setDate(QDate(2024,5,1));
    returnDateEnd->setDate(QDate::currentDate());
}

void rootWidget::setOptionWidget(){
    userOptionWidget->hide();
    bookOptionWidget->hide();
    borrowOptionWidget->hide();
    genreOptionWidget->hide();
    switch (tableNow)
    {
    case userNum:
        userOptionWidget->show();break;
    case bookNum:
        bookOptionWidget->show();break;
    case genreNum:
        genreOptionWidget->show();break;
    case borrowNum:
        borrowOptionWidget->show();break;
    default:
        break;
    }
}

void rootWidget::receviceDB(QSqlDatabase db){
    this->db = db;
}

void rootWidget::receviceRootUser(QString user,int permission){
    this->rootUser=user;
    this->userPermission=permission;
    ui->label_2->setText(QString("当前用户：%1").arg(user));
    tableNow=userNum;
    
    initRoot();

    if(ui->checkBox_user->isChecked())
        selectTable();
    else 
        ui->checkBox_user->setChecked(true);
}

void rootWidget::selectTable(){
    model->clear();
    model->setHorizontalHeaderLabels(tableHead[tableNow]);

    setOptionWidget();

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
    ui->pushButton_delete->hide();
    ui->pushButton_insert->hide();
    ui->lineEdit_search->clear();
    ui->lineEdit_search_2->hide();
    ui->label_search_2->hide();
    ui->label_search->setText("输入用户名查找:");
    selectTable();
}

void rootWidget::borrowSlot(bool b){
    if(!b)return;
    tableNow = borrowNum;
    ui->pushButton_delete->show();
    ui->pushButton_insert->hide();
    ui->lineEdit_search->clear();
    ui->lineEdit_search_2->show();
    ui->label_search_2->show();
    ui->label_search->setText("输入用户名查找:");
    selectTable();
}

void rootWidget::bookSlot(bool b){
    if(!b)return;
    tableNow = bookNum;
    ui->pushButton_delete->show();
    ui->pushButton_insert->show();
    ui->lineEdit_search->clear();
    ui->label_search_2->hide();
    ui->lineEdit_search_2->hide();
    ui->label_search->setText("输入书名查找:");
    selectTable();
}

void rootWidget::genreSlot(bool b){
    if(!b)return;
    tableNow = genreNum;
    ui->pushButton_delete->show();
    ui->pushButton_insert->show();
    ui->lineEdit_search->clear();
    ui->label_search_2->hide();
    ui->lineEdit_search_2->hide();
    ui->label_search->setText("输入类型查找:");
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

    Dialog dialog (Register,this,permission);
    dialog.exec();
}
//重置用户密码
void rootWidget::cPasswdSlot(){
    QMessageBox msg(this);
    msg.setText("请选择需重置密码账户");
    QPushButton *b1 = msg.addButton("取消",QMessageBox::RejectRole);
    QPushButton *b2 = msg.addButton("其他用户",QMessageBox::ButtonRole::ActionRole);
    QPushButton *b3 = msg.addButton("当前用户",QMessageBox::ButtonRole::ActionRole);
    msg.exec();
    if(msg.clickedButton()==b3){//当前用户,赋予最高权限修改密码
        Dialog dialog(ChangePassword,this,2);
        dialog.setAdmin(rootUser);
        dialog.setNoEditedAdmin();
        dialog.exec();
    }
    else if(msg.clickedButton()==b2){
        Dialog dialog(ChangePassword,this,userPermission);
        dialog.exec();
    }
}
//注销用户
void rootWidget::delUser(){
    QMessageBox msg(this);
    msg.setText("请选择需注销账户");
    QPushButton *b1 = msg.addButton("取消",QMessageBox::RejectRole);
    QPushButton *b2 = msg.addButton("其他用户",QMessageBox::ButtonRole::ActionRole);
    QPushButton *b3 = msg.addButton("当前用户",QMessageBox::ButtonRole::ActionRole);
    msg.exec();
    if(msg.clickedButton()==b3){//当前用户,赋予最高权限
        Dialog dialog(DeleteUser,this,2);
        dialog.setAdmin(rootUser);
        dialog.setNoEditedAdmin();
        if(dialog.exec()==QDialog::Accepted){
            emit backHome();
        }
    }
    else if(msg.clickedButton()==b2){
        Dialog dialog(DeleteUser,this,userPermission);
        dialog.exec();
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
}

void rootWidget::comboBoxBorrowSlot(int index){
    //qDebug()<<index;
    switch(index){
        case 0:
            uOption.state=option::allState;
            break;
        case 1:
            uOption.state=option::borrowed;
            break;
        case 2:
            uOption.state=option::returned;
            break;
        default:break;
    }
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
        case genreNum:
            getgenreCommand();
            break;
        case borrowNum:
            getBorrowCommand();
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
        case option::allState:
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
    if(!ui->lineEdit_search->text().isEmpty()){
        if(!command.isEmpty())command+=" and ";
        command+=QString(" name like '%1%'").arg(ui->lineEdit_search->text());
    }
}

void rootWidget::getBookCommand(){
   command=QString(" bnumber>=%1 and bnumber<=%2").arg(bkOption.minCnt).arg(bkOption.maxCnt);
   command+=QString(" order by bnumber %1 ").arg(bkOption.order==option::ASC?"ASC":"DESC");
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

void rootWidget::getgenreCommand(){
    command=QString(" floor>=%1 and floor<=%2 ").arg(gOption.minFloor).arg(gOption.maxFloor);
    if(!ui->lineEdit_search->text().isEmpty())
        command+=QString(" and type like '%1%'").arg(ui->lineEdit_search->text());
    command+=" order by floor ";
}

void rootWidget::minFloorSlot(){
    int min = lineEdit_minFloor->text().toInt();
    int max = lineEdit_maxFloor->text().toInt();
    if(min>max){
        QMessageBox::information(this,"提示","最小值大于最大值");
        lineEdit_minFloor->setText(QString::number(gOption.minFloor)); //设置为之前的值
        return;
    }
    gOption.minFloor=min;
}

void rootWidget::maxFloorSlot(){
    int min = lineEdit_minFloor->text().toInt();
    int max = lineEdit_maxFloor->text().toInt();
    if(min>max){
        QMessageBox::information(this,"提示","最小值大于最大值");
        lineEdit_maxFloor->setText(QString::number(gOption.maxFloor)); //设置为之前的值
        return;
    }
    gOption.maxFloor=max;
}

void rootWidget::getBorrowCommand(){
    switch(boOption.state){
        case option::allState:
            command+=" (return_time is null or ";
            command+=QString(" return_time>='%1' and return_time<='%2') ").arg(boOption.returnStartDate.toString("yyyy-MM-dd")).arg(boOption.returnEndDate.toString("yyyy-MM-dd"));
            break;
        case option::returned:
            command+=" return_time is not null and ";
            command+=QString(" return_time>='%1' and return_time<='%2' ").arg(boOption.returnStartDate.toString("yyyy-MM-dd")).arg(boOption.returnEndDate.toString("yyyy-MM-dd"));
            break;
        case option::NotReturned:
            command+=" return_time is null";break;
        default:break;
    }
    command+=" and ";
    command+=QString(" borrow_time>='%1' and borrow_time<='%2' ").arg(boOption.borrowStartDate.toString("yyyy-MM-dd")).arg(boOption.borrowEndDate.toString("yyyy-MM-dd"));
    
    if(!ui->lineEdit_search->text().isEmpty())
        command+=QString(" and name like '%1%'").arg(ui->lineEdit_search->text());
    if(!ui->lineEdit_search_2->text().isEmpty())
        command+=QString(" and bno like '%1%'").arg(ui->lineEdit_search_2->text());

    //排序
    switch(boOption.o_column){
        case option::C_BORROW_TIME:
            command+=" order by borrow_time ";break;
        case option::C_RETURN_TIME:
            command+=" order by return_time ";break;
        case option::C_TIME_CNT:
            command+=" order by time_cnt ";break;
        case option::C_MONEY:
            command+=" order by money ";break;
        default:break;
    }
    command+=boOption.order==option::ASC?"ASC":"DESC";
    command+=QString(" ,id %1").arg(boOption.order==option::ASC?"ASC":"DESC");
}

void rootWidget::returnBookSlot(){
    QSqlQuery qry;
    QString bno;
    //查询未还书，以及所需交的罚款
    bool ok;
    QString user = QInputDialog::getText(this,"还书","输入需还书账户",QLineEdit::Normal,"",&ok);
    if(!ok)return;
    qry.prepare("select name,borrowBook from user where name=:name");
    qry.bindValue(":name",user);
    if(!qry.exec()){
        QMessageBox::warning(this,"警告","查询错误");
        return;
    }
    qry.next();
    if(!qry.isValid()){
        QMessageBox::information(this,"提示","未查询到该账户");
        return;
    }
    bno=qry.value(1).toString();
    
    if(bno.isEmpty()){
        QMessageBox::information(this,"提示","该账户当前无需还书");
        return;
    }
    qry.prepare("select money from borrow where id=:id");
    qry.bindValue(":id",bno);
    if(!qry.exec() || !qry.next()){
        QMessageBox::warning(this,"警告","查询错误");
        return;
    }
    if(qry.value(0).toInt()>0){
        QMessageBox::warning(this,"警告","还书超时 需交罚款");
        QStringList item;
        bool ok;
        item<<"微信"<<"支付宝";
        QString str = QInputDialog::getItem(this,"付款","请选择付款方式",item,0,true,&ok);
        if(ok){
            QMessageBox::information(this,"付款信息","付款成功");
            // if(str==item[0]){

            // }
            // else if(str==str[1]){

            // }
        }
        else 
            return;
    }

    QDateTime current_time = QDateTime::currentDateTime();
    //更新还书时间，还完书后数量，user借书数量（MySql触发器实现）此处只需更新borrow表
    qry.exec(QString("select borrowBook from user where name='%1' and borrowBook is not null").arg(user));
    qry.next();
    QString id=qry.value(0).toString();
    if(qry.exec(QString("update borrow set return_time='%1' where id=%2").arg(current_time.toString("yyyy-MM-dd")).arg(id)))
        QMessageBox::information(this,"提示","还书成功");
}