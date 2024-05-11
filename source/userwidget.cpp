#include "../include/userwidget.h"
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

    ui->borrowWidget->hide();

    ui->pushButton_borrow->setText("借书");
    ui->pushButton_home->setText("退出");
    ui->pushButton_re->setText("刷新");
    ui->pushButton_return->setText("还书");
    ui->pushButton_delete->setText("注销账号");

    //tableView borrow 部分
    ui->label_bookName->setText("请输入借书的书名/编号/作者");
    borrow_model = new QStandardItemModel(this);
    ui->tableView_borrow->setModel(borrow_model);
    borrow_headList<<"编号"<<"书名"<<"出版社"<<"作者"<<"余量"<<"种类"<<"类型"<<"所在楼层";
    borrow_model->setHorizontalHeaderLabels(borrow_headList);

    //table view return 部分
    ui->label->setText("当前所借书");
    return_model = new QStandardItemModel(this);
    ui->tableView->setModel(return_model);
    return_headList<<"账户"<<"书编号"<<"书名"<<"借出时间"<<"已借天数"<<"还书时间"<<"欠费";
    return_model->setHorizontalHeaderLabels(return_headList);

    connect(ui->pushButton_borrow,&QPushButton::clicked,this,&userWidget::borrowShowSlot);
    connect(ui->pushButton_home,&QPushButton::clicked,this,&userWidget::backSlot);
    connect(ui->lineEdit_bookName,&QLineEdit::textEdited,this,&userWidget::searchSlot);
    connect(ui->pushButton_re,&QPushButton::clicked,this,&userWidget::reSlot);
    connect(ui->pushButton_return,&QPushButton::clicked,this,&userWidget::returnSlot);
    connect(ui->pushButton_delete,&QPushButton::clicked,this,&userWidget::deleteSlot);

    //双击选择书
    connect(ui->tableView_borrow,&QTableView::doubleClicked,this,&userWidget::borrowDoubleClickedSlot);
    
}

userWidget::~userWidget()
{
    delete ui;
}

void userWidget::borrowShowSlot(){
    if(!ui->borrowWidget->isHidden())return;
    QSqlQuery qry;
    qry.exec(QString("select name from borrow where name='%1' and return_time is null").arg(user));
    qry.next();
    if(qry.isValid()){
        QMessageBox::warning(this,"警告","当前有书未还");
        return;
    }
    ui->borrowWidget->show();
}

void userWidget::searchSlot(const QString &str){
    QSqlQuery qry;
    QString command;
    if(str.size()==0)
        command="select bno,bname,bauthor,bhouse,bnumber,book.kind,type,floor from book,grenre where book.kind=grenre.kind";
    else
        command=QString("select bno,bname,bauthor,bhouse,bnumber,book.kind,type,floor from book,grenre where book.kind=grenre.kind and (bname like '%%1%' or bno like '%%1%' or bauthor like '%%1%')").arg(str);
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
    QSqlQuery qry;
    qry.exec(QString("select name,borrow.bno,bname,borrow_time,time_cnt,return_time,money from borrow,book where borrow.bno=book.bno and name='%1' and return_time is null").arg(this->user));
    int i = 0;
    return_model->clear();
    return_model->setHorizontalHeaderLabels(return_headList);
    while(qry.next()){
        for(int j = 0;j<return_headList.length();j++){
            return_model->setItem(i, j, new QStandardItem(qry.value(j).toString()));
        }
        i++;
    }

    //列出有的书
    i = 0;
    qry.exec(QString("select bno,bname,bauthor,bhouse,bnumber,book.kind,type,floor from book,grenre where book.kind=grenre.kind"));
    borrow_model->clear();
    borrow_model->setHorizontalHeaderLabels(borrow_headList);
    while(qry.next()){
        for(int j = 0;j<borrow_headList.length();j++){
            borrow_model->setItem(i, j, new QStandardItem(qry.value(j).toString()));
        }
        i++;
    }
}

void userWidget::borrowDoubleClickedSlot(const QModelIndex &index){
    QString bookCode = borrow_model->index(index.row(),0).data().toString();
    QSqlQuery qry;
    if(!qry.exec(QString("update book set bnumber=bnumber-1 where bno='%1'").arg(bookCode))){
        QMessageBox::warning(this,"错误","数量不足");
        searchSlot(ui->lineEdit_bookName->text());
        return;
    }

    //重新查询 更新TableView
    searchSlot(ui->lineEdit_bookName->text());

    QDateTime current_time = QDateTime::currentDateTime();
    //qDebug()<<current_time.toString("yyyy-MM-dd");
    //添加一条借书记录
    qry.exec(QString("insert into borrow (name,bno,borrow_time,time_cnt,money) values ('%1','%2','%3',0,0)")
                    .arg(user)
                    .arg(bookCode)
                    .arg(current_time.toString("yyyy-MM-dd")));
    //更新用户表
    qry.exec(QString("update user set borrowBooks=borrowBooks+1 where name='%1'").arg(user));
    
    //更新借书记录表
    reSlot();
    QMessageBox::information(this,"提示","借书成功");
    ui->borrowWidget->hide();
}

void userWidget::reSlot(){
    QSqlQuery qry;
    qry.exec(QString("select name,borrow.bno,bname,borrow_time,time_cnt,return_time,money from borrow,book where borrow.bno=book.bno and name='%1' and return_time is null").arg(this->user));
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
    qry.exec(QString("select name,money,bno from borrow where name='%1' and return_time is null").arg(user));
    qry.next();
    
    if(!qry.isValid()){
        QMessageBox::warning(this,"警告","当前无需还书");
        return;
    }
    else if(qry.value(1).toInt()>0){
        QMessageBox::warning(this,"警告","还书超时 需交罚款");
        QStringList item;
        bool ok;
        item<<"微信"<<"支付宝";
        QInputDialog::getItem(this,"付款","请选择付款方式",item,0,true,&ok);
        if(ok){
            QMessageBox::information(this,"付款信息","付款成功");
        }
        else 
            return;
    }

    bno = qry.value(2).toString();
    QDateTime current_time = QDateTime::currentDateTime();
    //更新还书时间，还完书后数量，user借书数量
    if(qry.exec(QString("update borrow set return_time='%1' where name='%2' and return_time is null").arg(current_time.toString("yyyy-MM-dd")).arg(user)))
        if(qry.exec(QString("update book set bnumber=bnumber+1 where bno='%1'").arg(bno)))
            if(qry.exec(QString("update user set borrowBooks=borrowBooks-1 where name='%1'").arg(user)));          
                QMessageBox::information(this,"提示","还书成功");

    reSlot();
    searchSlot(ui->lineEdit_bookName->text());
}

//注销账号
void userWidget::deleteSlot(){
    bool ok;
    QSqlQuery qry;
    qry.exec(QString("select * from borrow where name='%1' and return_time is null").arg(user));
    qry.next();
    if(qry.isValid()){
        QMessageBox::information(this,"注意","请先归还书籍");
        return ;
    }

    QString passwd = QInputDialog::getText(this,"注销不可找回","输入密码注销",QLineEdit::Password,"",&ok);
    if(ok){
        //判断密码是否正确
        qry.exec(QString("select name from user where name='%1' and passwd='%2'").arg(user).arg(passwd));
        qry.next();
        if(!qry.isValid()){
            QMessageBox::warning(this,"警告","密码错误");
            return ;
        }
        //先删除借书记录（外码）
        if(!qry.exec(QString("delete from borrow where name='%1'").arg(user))){
            qDebug()<<"删除记录错误";
            return;
        }
        //删除user
        if(!qry.exec(QString("delete from user where name='%1'").arg(user))){
            qDebug()<<"删除user错误";
            return;
        }

        backSlot();
        QMessageBox::information(this,"提示","账号已注销");
    }
}

void userWidget::backSlot(){
    emit backHome();
    ui->borrowWidget->hide();
}