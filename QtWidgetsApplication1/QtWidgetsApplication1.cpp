#include "stdafx.h"
#include "QtWidgetsApplication1.h"
#include "ButtonDelegate.h"

QtWidgetsApplication1::QtWidgetsApplication1(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	setWindowTitle("磁力下载器 (作者:Yeux，qq:2845672658)");
	setWindowIcon(QIcon("magnet.jpg"));  // 设置窗口图标
	connect(ui.download_btn, &QPushButton::clicked, this, &QtWidgetsApplication1::onButtonClicked);
	connect(this, &QtWidgetsApplication1::s_wait_for_bt, this, &QtWidgetsApplication1::show_wait_for_Bt);
	connect(&thread, &Download_Thread::update_listview_items, this, &QtWidgetsApplication1::download_task_items);
	connect(&thread, &Download_Thread::update_items_msg, this, &QtWidgetsApplication1::update_items_msg);
	connect(&thread, &Download_Thread::remove_task_by_name_sig, this, &QtWidgetsApplication1::remove_item_by_text);
	// 添加数据到模型

	delegate = new ButtonDelegate(ui.listView);

	ui.listView->setItemDelegate(delegate);
	ui.listView->setModel(&model);

	// 启用右键菜单请求信号
	ui.listView->setContextMenuPolicy(Qt::CustomContextMenu);

	// 连接信号和槽
	connect(ui.listView, &QListView::customContextMenuRequested,
		this, &QtWidgetsApplication1::showContextMenu);

	connect(&model, &QStandardItemModel::rowsInserted, delegate, &ButtonDelegate::onRowsInserted);
	connect(delegate, &ButtonDelegate::pauseOneTask, &thread, &Download_Thread::pause_download_by_name);

	thread.start();

	//QList<Torrent_Msg*>* p = thread.get_torrent_list();
	//if (p) {
	//	for (Torrent_Msg* m : *p) {
	//		QStandardItem* item = new QStandardItem(QString(m->title));
	//		item->setEditable(false);
	//		item->setSizeHint(QSize(200, 50));
	//		model.appendRow(item);  // 将项添加到模型
	//	}
	//}

}

QtWidgetsApplication1::~QtWidgetsApplication1()
{

}

void QtWidgetsApplication1::download_task_items(QList<Torrent_Msg*>* list)
{
	if (list) {
		for (Torrent_Msg* m : *list) {
			QStandardItem* item = new QStandardItem(QString(m->title));
			item->setEditable(false);
			item->setSizeHint(QSize(200, 50));
			model.appendRow(item);  // 将项添加到模型
		}
	}

}

QStandardItem* QtWidgetsApplication1::get_item_by_text(QString text) {
	for (int i = 0; i < model.rowCount(); i++) {
		QStandardItem* item = model.item(i, 0);
		QString item_text = item->text();
		if (item_text.startsWith(text))
			return item;
	}
	return NULL;
}

void QtWidgetsApplication1::update_items_msg(QList<Torrent_Msg*>* tm_list)
{
	if (tm_list) {
		for (Torrent_Msg* tm : *tm_list) {
			//QModelIndex index = model.index(tm->index, 0);
			//QStandardItem* item = model.itemFromIndex(index);
			QStandardItem* item = get_item_by_text(tm->title);

			if (item) {
				QString rate = QString::number(tm->download_payload_rate / 1000) + " kB/s ";
				QString total_done = QString::number(tm->total_done / 1000) + "kB ";
				QString progress_ppm = QString::number(tm->progress_ppm / 10000) + "% downloaded";
				QString num_peers = QString::number(tm->num_peers) + " peers";
				item->setText(tm->title + "\n" + rate + total_done + "(" + progress_ppm + ") " + num_peers);
			}
			else {
				QStandardItem* item = new QStandardItem(QString(tm->title));
				item->setEditable(false);
				item->setSizeHint(QSize(200, 50));
				model.appendRow(item);  // 将项添加到模型
			}

			
		}
		//QCoreApplication::processEvents();
	}
	
}

void QtWidgetsApplication1::showContextMenu(const QPoint& pos)
{
	QListView* listView = qobject_cast<QListView*>(sender());
	QModelIndex index = listView->indexAt(pos);

	if (index.isValid()) {
		QStandardItemModel* model = qobject_cast<QStandardItemModel*>(listView->model());
		QStandardItem* item = model->itemFromIndex(index);

		// 获取该项的文本
		QString itemText = item->text();
		int line = itemText.indexOf("\n");
		if (line != -1) {
			itemText = itemText.left(line);
		}
		else
			itemText = "";
		// 创建右键菜单
		QMenu contextMenu;

		// 创建菜单项
		QAction* action1 = new QAction("删除任务", this);
	

		// 连接菜单项的槽
		connect(action1, &QAction::triggered, &thread, [=]() {
			QString task_name = itemText;  // 你可以根据需要传递不同的任务名称
			thread.remove_task_by_name(task_name);  // 调用 remove_task_by_name 并传递参数
			});
		

		// 添加菜单项到菜单
		contextMenu.addAction(action1);
		

		// 显示菜单
		contextMenu.exec(listView->viewport()->mapToGlobal(pos));
	}
}

void QtWidgetsApplication1::remove_item_by_text(QString text)
{
	if (text.isEmpty()) return;
	QStandardItem* item = get_item_by_text(text);
	if (item) {
		//如果某个item以text开头，则把他删除
		if (item->text().startsWith(text)) {
			QModelIndex index = model.indexFromItem(item);  // itemToDelete 是你要删除的 QStandardItem
			// 删除该行
			model.removeRow(index.row());
		}
		
	}
}



void QtWidgetsApplication1::onButtonClicked() {
	QString uri = ui.lineEdit->text();
	QString folderPath = QFileDialog::getExistingDirectory(this, "选择文件夹", "", QFileDialog::ShowDirsOnly);
	if (!uri.isEmpty()&&!folderPath.isEmpty()) {
		thread.set_uri(uri);
		thread.set_save_path(folderPath);
		//QThread::sleep(1);

		emit s_wait_for_bt();  // 发射信号
		
		
		//返回种子名字，并把种子下载任务添加到session中
		//QString torrent_name = thread.getDownloadTorrentName();
		lt::torrent_handle h = thread.add_torrent_to_ses_by_magent();
		if (h.is_valid())
			ui.label_2->setText(QString("开始下载..."));
		else
			ui.label_2->setText("磁力链接无效！！！");
	
	}
}

//void QtWidgetsApplication1::closeEvent(QCloseEvent* event) {
//    if (downloadThread->isRunning()) {
//        worker->stop();  // 停止工作线程中的任务逻辑
//        downloadThread->quit();  // 退出线程的事件循环
//        downloadThread->wait();  // 等待线程完全退出
//    }
//    QMainWindow::closeEvent(event);
//}
