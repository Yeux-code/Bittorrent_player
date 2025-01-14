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

    // �Զ������
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QStyledItemDelegate::paint(painter, option, index);

        // ���ư�ť
       /* QPushButton button;
        button.setText("Click");
        button.setGeometry(option.rect);*/
        QStyleOptionButton buttonOption;
        buttonOption.rect = QRect(option.rect.right() - 70, option.rect.top() + 5, 60, option.rect.height() - 10); // ��ť����
       // buttonOption.text = isDownloading[index.row()] ? "��ͣ" : "��ʼ����";

        buttonOption.text = isDownload.value(index.row(), false) ? "��ͣ" : "��ʼ����"; // ����״̬�����ı�

        buttonOption.state = QStyle::State_Enabled;

        QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter);
    }

    // ��������¼�
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
        const QModelIndex& index) override {
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            QRect buttonRect = QRect(option.rect.right() - 70, option.rect.top() + 5, 60, option.rect.height() - 10); // ��ť����
            if (buttonRect.contains(mouseEvent->pos())) {
                if (event->type() == QEvent::MouseButtonRelease) {

                    bool currentState = isDownload.value(index.row(), false); // ��ȡ��ǰ״̬
                    isDownload[index.row()] = !currentState; // �л�״̬
                    //isDownloading[index.row()] = !isDownloading[index.row()];
                    QVariant itemText = model->data(index);
                    QString text = itemText.toString(); // ת��Ϊ QString
                    // ֪ͨ��ͼ���£����»��ư�ť�ı���
                    emit model->dataChanged(index, index);
                    emit buttonClicked(text);  // �����ź�
                                 
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
        // ������ʱ������ isDownloading �Ĵ�С
        for (int i = first; i <= last; ++i) {
            isDownload.insert(i, true); // ������״̬Ĭ��Ϊ true
        }
        qDebug() << "Rows inserted, new isDownloading size:" << isDownload.size();
    }

    //void onRowsRemoved(const QModelIndex& parent, int first, int last) {
    //    Q_UNUSED(parent);
    //    // ɾ����ʱ������ isDownloading �Ĵ�С
    //    for (int i = last; i >= first; --i) {
    //        isDownload.removeAt(i);
    //    }
    //    qDebug() << "Rows removed, new isDownloading size:" << isDownload.size();
    //}
};

