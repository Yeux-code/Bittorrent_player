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
    // 创建数据模型
    QStandardItemModel model;
    ButtonDelegate* delegate;
    Download_Thread thread;
    

protected:
    void closeEvent(QCloseEvent* event) override {
        // 显示确认对话框
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认关闭", "确定要退出程序吗？",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            qDebug() << "用户确认退出";
            event->accept(); // 允许窗口关闭

            thread.close_BT_download();
            thread.wait();
        }
        else {
            qDebug() << "用户取消关闭";
            event->ignore(); // 阻止窗口关闭
        }
    }
signals:
    void s_wait_for_bt();

private slots:
    void onButtonClicked();
    void show_wait_for_Bt() {
        // 模拟一些工作
        ui.label_2->setText("正在打开文件");
        QCoreApplication::processEvents();
    }
    void download_task_items(QList<Torrent_Msg*>*);
    void update_items_msg(QList<Torrent_Msg*>*);
    void showContextMenu(const QPoint& pos);
    void remove_item_by_text(QString);
};
