#include "../include/mydialog.h"
#include <QFormLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QMessageBox>
#include <QComboBox>

myDialog::myDialog(tableNum table,int model,QWidget *parent,const QStringList &list_contents)
    :QDialog(parent)
{   
    setWindowTitle("数据更新");
    tableNow = table;
    this->model=model;
    this->list_contents=list_contents;
    if(!list_contents.isEmpty() && model==1)pk=list_contents[0];
    QPushButton *buttonOk = new QPushButton("确认",this);
    buttonOk->setFocus();
    QPushButton *buttonCancle = new QPushButton("取消",this);
    QHBoxLayout *button_layout = new QHBoxLayout();
    button_layout->addWidget(buttonCancle);
    button_layout->addWidget(buttonOk);
    connect(buttonOk,&QPushButton::clicked,[&]{
        doCommand();
        QDialog::accept();
    });
    connect(buttonCancle,&QPushButton::clicked,this,&QDialog::reject);
    formlayout = new QFormLayout();
    switch(table){
        case bookNum:
            initBook();
            break;
        case genreNum:{
            initGenre();
            break;
        }
        default:break;
    }
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formlayout);
    mainLayout->addLayout(button_layout);
}

void myDialog::initGenre(){
    QStringList column1,column2;
    column1<<"kind"<<"type"<<"floor";
    column2<<"种类"<<"类型"<<"所在楼层";
    for(int i = 0;i<3;i++){
        QLineEdit *lineEdit = new QLineEdit(this);
        lineEdit->setObjectName(column1[i]);
        if(i==2)lineEdit->setValidator(new QRegExpValidator(QRegExp("[1-4]{1}")));
        connect(lineEdit, &QLineEdit::textChanged, this, [=](const QString &str) {
            QLineEdit *lineEditSender = qobject_cast<QLineEdit*>(sender());
            if (lineEditSender) {
                QString senderName = lineEditSender->objectName();
                if (senderName == "kind") {
                    genre_kind = str;
                } else if (senderName == "type") {
                    genre_type = str;
                } else if (senderName == "floor") {
                    genre_floor = str.toInt();
                }
            }
        });
        if(model==1)lineEdit->setText(list_contents[i]);
        formlayout->addRow(column2[i],lineEdit);
    }
}

void myDialog::doGenreCommand(){
    QSqlQuery qry;
    if(model==1){
        qry.prepare("update genre set kind=:kind,type=:type,floor=:floor where kind=:pk");
        qry.bindValue(":kind",genre_kind);
        qry.bindValue(":type",genre_type);
        qry.bindValue(":floor",genre_floor);
        qry.bindValue(":pk",pk);
        if(!qry.exec()){
            QMessageBox::warning(this,"错误","更新错误");
            qDebug()<<qry.lastError().text();
        }
        else
            QMessageBox::information(this,"提示","更新成功");
    }
    else if(model==0){
        qry.prepare("insert into genre values(:kind,:type,:floor)");
        qry.bindValue(":kind",genre_kind);
        qry.bindValue(":type",genre_type);
        qry.bindValue(":floor",genre_floor);
        if(!qry.exec()){
            QMessageBox::warning(this,"错误","插入错误");
            qDebug()<<qry.lastError().text();
        }
        else
            QMessageBox::information(this,"提示","插入成功");
    }
}

void myDialog::initBook(){
    QStringList column1,column2;
    column1<<"bno"<<"bname"<<"bauthor"<<"bhouse"<<"bnumber"<<"kind";
    column2<<"编号"<<"书名"<<"出版社"<<"作者"<<"余量"<<"种类";
    for(int i = 0;i<5;i++){
        QLineEdit *lineEdit = new QLineEdit(this);
        lineEdit->setObjectName(column1[i]);
        if(i==4)lineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]{3}")));  //余量限制数字输入
        connect(lineEdit, &QLineEdit::textChanged, this, [=](const QString &str) {
            QLineEdit *lineEditSender = qobject_cast<QLineEdit*>(sender());
            if (lineEditSender) {
                QString senderName = lineEditSender->objectName();
                if (senderName == column1[0]) {
                    book.bno=str;
                } else if (senderName == column1[1]) {
                    book.bname=str;
                } else if (senderName == column1[2]) {
                    book.bauthor=str;
                } else if (senderName == column1[3]) {
                    book.bhouse=str;
                } else if (senderName == column1[4]) {
                    book.bnumber=str.toUInt();
                }
            }
        });
        if(model==1)lineEdit->setText(list_contents[i]);
        formlayout->addRow(column2[i],lineEdit);
    }
    QStringList list;
    QComboBox *comboBox = new QComboBox(this);  //最后一项，外码，限制输入
    connect(comboBox,&QComboBox::currentTextChanged,[&](const QString &str){
        book.kind=str;
    });
    QSqlQuery qry;
    if(!qry.exec("select kind from genre")){
        QMessageBox::warning(this,"警告","外码查询失败");
        return;
    }
    while(qry.next()){
        list.append(qry.value(0).toString());
    }
    comboBox->addItems(list);
    if(model==1)comboBox->setCurrentIndex(list.indexOf(list_contents.last()));
    formlayout->addRow(column2.last(),comboBox);

}

void myDialog::doBookCommand(){
    QSqlQuery qry;
    if(model==1){//更新
        qry.prepare("update book set bno=:bno,bname=:bname,bauthor=:bauthor,bhouse=:bhouse,bnumber=:bnumber,kind=:kind where bno=:pk");
        qry.bindValue(":bno",book.bno);
        qry.bindValue(":bname",book.bname);
        qry.bindValue(":bauthor",book.bauthor);
        qry.bindValue(":bhouse",book.bhouse);
        qry.bindValue(":bnumber",book.bnumber);
        qry.bindValue(":kind",book.kind);
        qry.bindValue(":pk",pk);
        if(!qry.exec()){
            QMessageBox::warning(this,"错误","更新错误");
            qDebug()<<qry.lastError().text();
        }
        else
            QMessageBox::information(this,"提示","更新成功");
    }
    else if(model==0){
        qry.prepare("insert into book values(:bno,:bname,:bauthor,:bhouse,:bnumber,:kind)");
        qry.bindValue(":bno",book.bno);
        qry.bindValue(":bname",book.bname);
        qry.bindValue(":bauthor",book.bauthor);
        qry.bindValue(":bhouse",book.bhouse);
        qry.bindValue(":bnumber",book.bnumber);
        qry.bindValue(":kind",book.kind);
        if(!qry.exec()){
            QMessageBox::warning(this,"错误","插入错误");
            qDebug()<<qry.lastError().text();
        }
        else
            QMessageBox::information(this,"提示","插入成功");
    }
}

void myDialog::doCommand(){
    switch(tableNow){
        case bookNum:
            doBookCommand();
            break;
        case genreNum:
            doGenreCommand();
            break;
        default:break;
    }
}
