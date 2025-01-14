#pragma once
#pragma execution_character_set("utf-8")
#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication1.h"
#include <ButtonDelegate.h>
#include "Download_Thread.h"
class QtWidgetsApplication1 : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication1(QWidget *parent = nullptr);
    ~QtWidgetsApplication1();
    QStandardItem* get_item_by_text(QString text);
   

private:
    Ui::QtWidgetsApplication1Class ui;
    // ��������ģ��
    QStandardItemModel model;
    ButtonDelegate* delegate;
    Download_Thread thread;
    

protected:
    void closeEvent(QCloseEvent* event) override {
        // ��ʾȷ�϶Ի���
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "ȷ�Ϲر�", "ȷ��Ҫ�˳�������",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            qDebug() << "�û�ȷ���˳�";
            event->accept(); // �����ڹر�

            thread.close_BT_download();
            thread.wait();
        }
        else {
            qDebug() << "�û�ȡ���ر�";
            event->ignore(); // ��ֹ���ڹر�
        }
    }
signals:
    void s_wait_for_bt();

private slots:
    void onButtonClicked();
    void show_wait_for_Bt() {
        // ģ��һЩ����
        ui.label_2->setText("���ڴ��ļ�");
        QCoreApplication::processEvents();
    }
    void download_task_items(QList<Torrent_Msg*>*);
    void update_items_msg(QList<Torrent_Msg*>*);
    void showContextMenu(const QPoint& pos);
    void remove_item_by_text(QString);
};
