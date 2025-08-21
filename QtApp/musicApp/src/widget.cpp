#include <QDebug>

#include "widget.h"
#include "ui_widget.h"
#include "bind.h"
#include "player.h"


Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowTitle("Music App");

    QFont f("黑体", 14);
    ui->appEdit->setFont(f);
    ui->passEdit->setFont(f);

    socket = new QTcpSocket;
    socket->connectToHost(QHostAddress("8.148.70.46"), 8000);  // 公网ip

    connect(socket, &QTcpSocket::connected, [this]() {
        QMessageBox::information(this, "Connected", "Successfully connected to server");
    });

    connect(socket, &QTcpSocket::disconnected, [this]() {
        QMessageBox::warning(this, "Disconnected", "Disconnected from server");
    });

    connect(socket, &QTcpSocket::readyRead, this, &Widget::server_reply_slot);
}

void Widget::server_reply_slot()
{
    while (socket->bytesAvailable() > 0) {
        QByteArray buffer = socket->readAll();
        // 处理服务器回复
    }
}

Widget::~Widget()
{
    delete ui;
    delete socket;
}

void Widget::on_registerButton_clicked()
{
    //获取appid和password
    QString appid = ui->appEdit->text();
    QString password = ui->passEdit->text();

    //发送给服务器    {"cmd":"app_register", "appid":"1001", "password":"1111"}
    QJsonObject obj;
    obj.insert("cmd", "app_register");
    obj.insert("appid", appid);
    obj.insert("password", password);

    widget_send_data(obj);
}

void Widget::widget_send_data(QJsonObject &obj)
{
    QByteArray sendData;
    QByteArray ba = QJsonDocument(obj).toJson();
    
    int size = ba.size();
    sendData.insert(0, (const char *)&size, 4);  // 头部，4个字节的数据大小
    sendData.append(ba);                         // 实际数据

    socket->write(sendData);
}

void Widget::server_reply_slot()
{
    QByteArray recvData;
    
    widget_recv_data(recvData);

    QJsonObject obj = QJsonDocument::fromJson(recvData).object();
    QString cmd = obj.value("cmd").toString();
    if (cmd == "app_register_reply") {
        app_register_handler(obj);
    }
    else if (cmd == "app_login_reply") {
        app_login_handler(obj);
    }
}

void Widget::app_register_handler(QJsonObject &obj)
{
    QString result = obj.value("result").toString();
    if (result == "success") {
        QMessageBox::information(this, "Registration", "Registration successful");
    } else if (result == "failure") {
        QMessageBox::warning(this, "Registration", "Registration failed");
    }
}

void Widget::app_login_handler(QJsonObject &obj)
{
    QString result = obj.value("result").toString();
    if (result == "not_exist") {
        QMessageBox::warning(this, "Login Tips", "User does not exist, please register first");
    }
    else if (result == "password_error") {
        QMessageBox::warning(this, "Login Tips", "Wrong password, please check");
    }
    else if (result == "not_bind") {
        // 创建新的界面
        QString deviceid = obj.value("deviceid").toString();

        socket->disconnect(SIGNAL(connected()));
        socket->disconnect(SIGNAL(disconnected()));
        socket->disconnect(SIGNAL(readyRead()));

        Bind *m_bind = new Bind(socket, m_appid, deviceid);
        m_bind->show();
        this->hide();
    }
    else if (result == "bind") {
        // 创建新的界面
        socket->disconnect(SIGNAL(connected()));
        socket->disconnect(SIGNAL(disconnected()));
        socket->disconnect(SIGNAL(readyRead()));

        Player *m_player = new Player(socket, m_appid);
        m_player->show();
        this->hide();
    }
}

void Widget::widget_recv_data(QByteArray &ba)
{
    char buf[1024] = {0};
    qint64 size = 0;
    while (true)
    {
        size += socket->read(buf + size, sizeof(int) - size);
        if (size >= sizeof(int))
            break;
    }

    int len = *(int *)buf;
    size = 0;
    memset(buf, 0, sizeof(buf));
    while (true)
    {
        size += socket->read(buf + size, len - size);
        if (size >= len)
            break;
    }
    ba.append(buf);
    qDebug() << "Received data size:" << ba.size();
    qDebug() << "Data content:" << ba;
}

void Widget::on_loginButton_clicked()
{
    QString appid = ui->appEdit->text();
    QString password = ui->passEdit->text();

    this->m_appid = appid;

    QJsonObject obj;
    obj.insert("cmd", "app_login");
    obj.insert("appid", appid);
    obj.insert("password", password);

    widget_send_data(obj);
}
