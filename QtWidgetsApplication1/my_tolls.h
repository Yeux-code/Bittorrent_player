#pragma once
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
#include "Torrent_Msg.h"
//#include "Download_Thread.h"
#pragma execution_character_set("utf-8")
namespace fs = std::filesystem;
std::string getDownloadTorrentName(lt::session* ses, std::string magent_uri, std::string save_path);
int test();
void close_download();
void start_download(lt::session* ses);
lt::torrent_handle add_torrent_to_ses_by_magent(lt::session* ses, std::string magent_uri, std::string save_path);
QList<Torrent_Msg*>* get_torrent_list();
lt::session* init_session();
std::vector<std::string> get_resume_files();
//void update_alert_msg();