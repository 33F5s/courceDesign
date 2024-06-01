#include "../include/userwidget.h"
#include "../include/dialog.h"
#include "ui_userwidget.h"
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QInputDialog>

userWidget::userWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::userWidget)
{
    ui->setupUi(this);

    ui->pushButton_home->setText("退出");
    ui->pushButton_re->setText("刷新");
    ui->pushButton_return->setText("还书");
    ui->pushButton_delete->setText("注销账号");
    ui->pushButton_search->setText("搜索");
    ui->pushButton_cPassWd->setText("修改密码");

    //tableView borrow 部分
    ui->tableView_borrow->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_borrow->verticalHeader()->setMinimumSectionSize(35);
    ui->label_bookName->setText("请输入借书的书名/编号/作者");
    borrow_model = new QStandardItemModel(this);
    ui->tableView_borrow->setModel(borrow_model);
    borrow_headList<<"编号"<<"书名"<<"出版社"<<"作者"<<"余量"<<"种类"<<"类型"<<"所在楼层";
    borrow_model->setHorizontalHeaderLabels(borrow_headList);

    //table view return 部分
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setMinimumSectionSize(35);
    ui->label->setText("当前所借书");
    return_model = new QStandardItemModel(this);
    ui->tableView->setModel(return_model);
    return_headList<<"账户"<<"书编号"<<"书名"<<"借出时间"<<"已借天数"<<"还书时间"<<"欠费";
    return_model->setHorizontalHeaderLabels(return_headList);

    connect(ui->pushButton_home,&QPushButton::clicked,this,&userWidget::backSlot);
    connect(ui->pushButton_search,&QPushButton::clicked,[&]{
        searchSlot(ui->lineEdit_bookName->text());
    });
    connect(ui->lineEdit_bookName,&QLineEdit::textEdited,this,&userWidget::searchSlot);
    connect(ui->pushButton_re,&QPushButton::clicked,this,&userWidget::reSlot);
    connect(ui->pushButton_return,&QPushButton::clicked,this,&userWidget::returnSlot);
    connect(ui->pushButton_delete,&QPushButton::clicked,this,&userWidget::deleteSlot);
    connect(ui->pushButton_cPassWd,&QPushButton::clicked,this,&userWidget::cPassWdSlot);

    //双击选择书
    connect(ui->tableView_borrow,&QTableView::doubleClicked,this,&userWidget::borrowDoubleClickedSlot);
    
}

userWidget::~userWidget()
{
    delete ui;
}

//查询书籍
void userWidget::searchSlot(const QString &str){
    QSqlQuery qry;
    QString command;
    //搜索书籍（内连接book和genre表）
    if(str.size()==0)
        command="select bno,bname,bauthor,bhouse,bnumber,book.kind,type,floor from book,genre where book.kind=genre.kind";
    else
        command=QString("select bno,bname,bauthor,bhouse,bnumber,book.kind,type,floor from book,genre where book.kind=genre.kind and (bname like '%1%' or bno like '%1%' or bauthor like '%1%')").arg(str);
    if(!qry.exec(command)){
        qDebug()<<"query wrong";
        return;
    }
    //i j 行列循环填表
    int i = 0;
    borrow_model->clear();
    borrow_model->setHorizontalHeaderLabels(borrow_headList);
    while(qry.next()){
        for(int j = 0;j<borrow_headList.length();j++){
            borrow_model->setItem(i, j, new QStandardItem(qry.value(j).toString()));
        }
        i++;
    }
    
}

void userWidget::receviceDB(QSqlDatabase db){
    this->db = db;
}

void userWidget::receviceUser(QString user){
    this->user=user;
    ui->label_name->setText(QString("当前用户：%1").arg(user));
    ui->lineEdit_bookName->setText("");
    searchSlot("");//查询所有书籍
    //查询未还的书
    reSlot();
}

//借书
void userWidget::borrowDoubleClickedSlot(const QModelIndex &index){
    QString bookCode = borrow_model->index(index.row(),0).data().toString();
    QSqlQuery qry;

    //重新查询 更新TableView
    //searchSlot(ui->lineEdit_bookName->text());

    QDateTime current_time = QDateTime::currentDateTime();
    //qDebug()<<current_time.toString("yyyy-MM-dd");
    //添加一条借书记录
    bool ok = qry.exec(QString("insert into borrow (name,bno,borrow_time,time_cnt,money) values ('%1','%2','%3',0,0)")
                    .arg(user)
                    .arg(bookCode)
                    .arg(current_time.toString("yyyy-MM-dd")));
    if(!ok){
        qry.prepare("select borrowBook from user where name=:name");
        qry.bindValue(":name",user);
        qry.exec();
        qry.next();
        if(qry.isValid()){
            QMessageBox::warning(this,"错误","请先归还书籍");
            return;
        }
        QMessageBox::warning(this,"错误","数量不足");
        searchSlot(ui->lineEdit_bookName->text());
        return;
    }
    //更新用户表（设置borrowBook为borrow表中的索引） 改为MySql触发器实现
    //qry.exec(QString("update user set borrowBook=select id from ").arg(user));
    
    //刷新显示借书记录表
    reSlot();
    QMessageBox::information(this,"提示","借书成功");
    searchSlot(ui->lineEdit_bookName->text());
}

//显示当前用户所借书
void userWidget::reSlot(){
    QSqlQuery qry;
    qry.exec(QString("select name,borrow.bno,bname,borrow_time,time_cnt,return_time,money from borrow,book where borrow.bno=book.bno and id=(select borrowBook from user where name='%1' and borrowBook is not null)").arg(this->user));
    int i = 0;
    return_model->clear();
    return_model->setHorizontalHeaderLabels(return_headList);
     while(qry.next()){
        for(int j = 0;j<return_headList.length();j++){
            return_model->setItem(i, j, new QStandardItem(qry.value(j).toString()));
        }
        i++;
    }
}

void userWidget::returnSlot(){
    QSqlQuery qry;
    QString bno;
    //查询未还书，以及所需交的罚款
    qry.exec(QString("select name,money,bno from borrow where id=(select borrowBook from user where name='%1' and borrowBook is not null)").arg(user));
    qry.next();
    
    if(!qry.isValid()){
        QMessageBox::information(this,"提示","当前无需还书");
        return;
    }
    else if(qry.value(1).toInt()>0){
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

    bno = qry.value(2).toString();
    QDateTime current_time = QDateTime::currentDateTime();
    //更新还书时间，还完书后数量，user借书数量（MySql触发器实现）此处只需更新borrow表
    qry.exec(QString("select borrowBook from user where name='%1' and borrowBook is not null").arg(user));
    qry.next();
    QString id=qry.value(0).toString();
    if(qry.exec(QString("update borrow set return_time='%1' where id=%2").arg(current_time.toString("yyyy-MM-dd")).arg(id)))
        QMessageBox::information(this,"提示","还书成功");
    reSlot();
    searchSlot(ui->lineEdit_bookName->text());
}

//注销账号
void userWidget::deleteSlot(){
    Dialog dialog(DeleteUser,this,2);   //注销自己账号，赋予最高权限
    dialog.setAdmin(user);
    dialog.setNoEditedAdmin();
    if(dialog.exec()==QDialog::Accepted){
        emit backHome();
    }
}

//修改密码槽函数
void userWidget::cPassWdSlot(){
    Dialog dialog(ChangePassword,this,2);   //修改自己密码，赋予最高权限
    dialog.setAdmin(user);
    dialog.setNoEditedAdmin();
    dialog.exec();
}

void userWidget::backSlot(){
    emit backHome();
}