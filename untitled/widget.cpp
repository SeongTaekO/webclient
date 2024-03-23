#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    QObject::connect(&socket_tcp, &QAbstractSocket::connected, this, &Widget::doConnected);
    QObject::connect(&socket_tcp, &QAbstractSocket::disconnected, this, &Widget::doDisconnected);
    QObject::connect(&socket_tcp, &QAbstractSocket::readyRead, this, &Widget::doReadyRead);

    QObject::connect(&socket_ssl, &QAbstractSocket::connected, this, &Widget::doConnected);
    QObject::connect(&socket_ssl, &QAbstractSocket::disconnected, this, &Widget::doDisconnected);
    QObject::connect(&socket_ssl, &QAbstractSocket::readyRead, this, &Widget::doReadyRead);

    connected_state_check();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::connected_state_check()
{
    if (socket_ssl.state() || socket_tcp.state()) {
        ui->pbConnect->setDisabled(true);
        ui->pbDisconnect->setDisabled(false);

        ui->cbSslTcp->setDisabled(true);
        connect_state = true;
    }

    else if (!socket_ssl.state() || !socket_tcp.state()) {
        ui->pbConnect->setDisabled(false);
        ui->pbDisconnect->setDisabled(true);

        ui->cbSslTcp->setDisabled(false);
        connect_state = false;
    }
}

void Widget::on_pbConnect_clicked()
{
    if (checkbox_checked)
        socket_ssl.connectToHostEncrypted(ui->leHost->text(), ui->lePort->text().toUShort());
    else
        socket_tcp.connectToHost(ui->leHost->text(), ui->lePort->text().toUShort());
}

void Widget::doConnected()
{
    if(checkbox_checked)
        ui->pteMessage->insertPlainText("SSL Connected\r\n");
    else
        ui->pteMessage->insertPlainText("TCP Connected\r\n");
    connected_state_check();
}


void Widget::on_pbDisconnect_clicked()
{
    if (checkbox_checked)
        socket_ssl.close();
    else
        socket_tcp.close();
}

void Widget::doDisconnected()
{
    ui->pteMessage->insertPlainText("Disconnected\r\n");
    connected_state_check();
}

void Widget::on_pbClear_clicked()
{
    ui->pteMessage->clear();
}


void Widget::on_cbSslTcp_stateChanged(int arg1)
{
    if (ui->cbSslTcp->isChecked()) {
        ui->lePort->setText("443");
        checkbox_checked = true;
    }
    else {
        ui->lePort->setText("80");
        checkbox_checked = false;
    }
}


void Widget::on_pbSend_clicked()
{
    if (!connect_state)
        ui->pteMessage->insertPlainText("not connected at server\r\n");
    else {
        if (checkbox_checked)
            socket_ssl.write(ui->pteSend->toPlainText().toUtf8());
        else
            socket_tcp.write(ui->pteSend->toPlainText().toUtf8());
    }
}

void Widget::doReadyRead()
{
    if (checkbox_checked)
        ui->pteMessage->insertPlainText(socket_ssl.readAll());
    else
        ui->pteMessage->insertPlainText(socket_tcp.readAll());
}


