#include "stdafx.h"
#include "ButtonDelegate.h"


void ButtonDelegate::itemChickButSlot(QString text) {
	if (text.isEmpty()) return;
    int newlineIndex = text.indexOf("\n");
    QString title;
    // 如果找到了换行符，截取换行符之前的内容
    if (newlineIndex != -1) {
        title = text.left(newlineIndex);
    }
    emit pauseOneTask(title);
	

}

//void ButtonDelegate::setDownloadThread(Download_Thread* thread)
//{
//    this->thread = thread;
//}
