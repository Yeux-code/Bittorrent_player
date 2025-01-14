#pragma once
#pragma execution_character_set("utf-8")
#include "D:\Qt\QT_NEW\6.8.1\msvc2022_64\include\QtWidgets\qstyleditemdelegate.h"
#include <qdebug.h>
#include <Download_Thread.h>

class ButtonDelegate :
    public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ButtonDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {
        connect(this, &ButtonDelegate::buttonClicked, this,&ButtonDelegate::itemChickButSlot);
    }
    void itemChickButSlot(QString);
   // void setDownloadThread(Download_Thread* thread);

    // 自定义绘制
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QStyledItemDelegate::paint(painter, option, index);

        // 绘制按钮
       /* QPushButton button;
        button.setText("Click");
        button.setGeometry(option.rect);*/
        QStyleOptionButton buttonOption;
        buttonOption.rect = QRect(option.rect.right() - 70, option.rect.top() + 5, 60, option.rect.height() - 10); // 按钮区域
       // buttonOption.text = isDownloading[index.row()] ? "暂停" : "开始下载";

        buttonOption.text = isDownload.value(index.row(), false) ? "暂停" : "开始下载"; // 根据状态设置文本

        buttonOption.state = QStyle::State_Enabled;

        QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter);
    }

    // 处理鼠标事件
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
        const QModelIndex& index) override {
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            QRect buttonRect = QRect(option.rect.right() - 70, option.rect.top() + 5, 60, option.rect.height() - 10); // 按钮区域
            if (buttonRect.contains(mouseEvent->pos())) {
                if (event->type() == QEvent::MouseButtonRelease) {

                    bool currentState = isDownload.value(index.row(), false); // 获取当前状态
                    isDownload[index.row()] = !currentState; // 切换状态
                    //isDownloading[index.row()] = !isDownloading[index.row()];
                    QVariant itemText = model->data(index);
                    QString text = itemText.toString(); // 转换为 QString
                    // 通知视图更新（重新绘制按钮文本）
                    emit model->dataChanged(index, index);
                    emit buttonClicked(text);  // 发出信号
                                 
                }
                return true;
            }
        }
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

signals:
    void buttonClicked(QString) const;
    void pauseOneTask(QString);
private:
    QString buttonText;
    QList<bool> isDownload;
    //Download_Thread* thread;
public slots:
    void onRowsInserted(const QModelIndex& parent, int first, int last) {
        Q_UNUSED(parent);
        // 增加行时，调整 isDownloading 的大小
        for (int i = first; i <= last; ++i) {
            isDownload.insert(i, true); // 新增行状态默认为 true
        }
        qDebug() << "Rows inserted, new isDownloading size:" << isDownload.size();
    }

    //void onRowsRemoved(const QModelIndex& parent, int first, int last) {
    //    Q_UNUSED(parent);
    //    // 删除行时，调整 isDownloading 的大小
    //    for (int i = last; i >= first; --i) {
    //        isDownload.removeAt(i);
    //    }
    //    qDebug() << "Rows removed, new isDownloading size:" << isDownload.size();
    //}
};

