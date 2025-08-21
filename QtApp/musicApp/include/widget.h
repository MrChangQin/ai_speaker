#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_registerButton_clicked();
    void on_loginButton_clicked();
    void server_reply_slot();

private:
    void widget_send_data(QJsonObject &obj);
    void widget_recv_data(QByteArray &ba);
    void app_register_handler(QJsonObject &obj);
    void app_login_handler(QJsonObject &obj);

private:
    Ui::Widget *ui;
    QTcpSocket *socket;
    QString m_appid;
};

#endif // WIDGET_H