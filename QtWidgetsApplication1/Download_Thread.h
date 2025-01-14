#pragma once
#pragma execution_character_set("utf-8")

#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <csignal>

#include <libtorrent/session.hpp>
#include <libtorrent/session_params.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/write_resume_data.hpp>
#include <libtorrent/error_code.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/torrent_info.hpp>
#include <filesystem>
#include <windows.h>
#include "D:\Qt\QT_NEW\6.8.1\msvc2022_64\include\QtCore\qthread.h"
//#include "my_tolls.h"
#include "Torrent_Msg.h"
using clk = std::chrono::steady_clock;
namespace fs = std::filesystem;

class Download_Thread :
    public QThread{
    Q_OBJECT

public:
    void set_uri(QString uri);
    explicit Download_Thread(QObject* parent = nullptr) : QThread(parent) {}

    lt::session* init_session();
    std::vector<std::string> get_resume_files();
    lt::torrent_handle add_torrent_to_ses_by_magent();
    QString getDownloadTorrentName();
    void start_download(lt::session* ses);
    std::vector<char> load_file(char const* filename);
    void sighandler(int);
    Torrent_Msg* get_tm_by_name(QString name);
    Torrent_Msg* get_tm_by_hash(lt::sha1_hash);
    lt::sha1_hash* get_hash_by_name(QString);
    std::wstring stringToWstring(const std::string& str);

    void close_BT_download() {
        shut_down = true;
    }
    char const* state(lt::torrent_status::state_t s);


    bool is_magnet_downloading() {
        // ½âÎö´ÅÁ¦Á´½Ó
        lt::error_code ec;
        lt::add_torrent_params magnet = lt::parse_magnet_uri(magnet_uri.toStdString());
        magnet.save_path = save_path.toStdString();
        lt::torrent_handle h = ses->add_torrent(magnet);
        if (h.is_valid()) {
            auto const handles = ses->get_torrents();
            for (int i = 0; i < handles.size(); i++) {

                if (handles[i].info_hash() == h.info_hash())
                    return true;
            }
        }
        return false;
    }

    void set_save_path(QString path) {
        this->save_path = path;
    }


    QList<Torrent_Msg*>* get_torrent_list() {

        return &tm_list;
       
    }


signals:
    void update_listview_items(QList<Torrent_Msg*>*);
    void update_items_msg(QList<Torrent_Msg*>*);
    void  remove_task_by_name_sig(QString);
protected:
    void run() override {
        try
        {
            
            ses = init_session();
            if (!ses) return;
            start_download(ses);
            delete ses;
            qDebug() << "\ndone, shutting down";
        }
        catch (std::exception& e)
        {
            qDebug() << "Error: " << e.what();
           
        }
    }
    
private:
    QString magnet_uri,save_path;
   // QList<Torrent_Msg*>* list;
    lt::session* ses=0;

    bool g_done = false;
    QList<Torrent_Msg*> tm_list;
    std::string download_torrent_name;
    lt::torrent_handle g_handle;
    bool g_isDownloading = false;
    int count_resume = 0;
    int num_item = 0;
    // set when we're exiting
    std::atomic<bool> shut_down{ false };
    int pending_saves = 0;
public slots:
    void pause_download_by_name(QString name);
    void remove_task_by_name(QString);
};

