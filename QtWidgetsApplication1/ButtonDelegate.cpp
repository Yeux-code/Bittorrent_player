#include "stdafx.h"
#include "ButtonDelegate.h"


void ButtonDelegate::itemChickButSlot(QString text) {
	if (text.isEmpty()) return;
    int newlineIndex = text.indexOf("\n");
    QString title;
    // ����ҵ��˻��з�����ȡ���з�֮ǰ������
    if (newlineIndex != -1) {
        title = text.left(newlineIndex);
    }
    emit pauseOneTask(title);
	

}

//void ButtonDelegate::setDownloadThread(Download_Thread* thread)
//{
//    this->thread = thread;
//}
