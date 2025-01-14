#include "stdafx.h"
///*
//#include "my_tolls.h"
//
//bool g_done = false;
//QList<Torrent_Msg*> tm_list;
//std::string download_torrent_name;
//lt::torrent_handle g_handle;
//bool g_isDownloading = false;
//int count_resume = 0;
//namespace {
//
//    using clk = std::chrono::steady_clock;
//
//    // return the name of a torrent status enum
//    char const* state(lt::torrent_status::state_t s)
//    {
//#ifdef __clang__
//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wcovered-switch-default"
//#endif
//        switch (s) {
//        case lt::torrent_status::checking_files: return "checking";
//        case lt::torrent_status::downloading_metadata: return "dl metadata";
//        case lt::torrent_status::downloading: return "downloading";
//        case lt::torrent_status::finished: return "finished";
//        case lt::torrent_status::seeding: return "seeding";
//        case lt::torrent_status::checking_resume_data: return "checking resume";
//        default: return "<>";
//        }
//#ifdef __clang__
//#pragma clang diagnostic pop
//#endif
//    }
//
//    std::vector<char> load_file(char const* filename)
//    {
//        std::ifstream ifs(filename, std::ios_base::binary);
//        ifs.unsetf(std::ios_base::skipws);
//        return { std::istream_iterator<char>(ifs), std::istream_iterator<char>() };
//    }
//
//    // set when we're exiting
//    std::atomic<bool> shut_down{ false };
//
//    void sighandler(int) {
//        shut_down = true;
//
//    }
//
//} // anonymous namespace
//
//lt::session* init_session() {
//    // load session parameters
//    auto session_params = load_file(".session");
//    lt::session_params params = session_params.empty()
//        ? lt::session_params() : lt::read_session_params(session_params);
//    params.settings.set_int(lt::settings_pack::alert_mask
//        , lt::alert_category::error
//        | lt::alert_category::storage
//        | lt::alert_category::status);
//    lt::session* ses = new lt::session(params);
//    //  lt::session ses(params);
//
//
//    std::vector<std::string> resume_files = get_resume_files();
//    lt::torrent_handle handle;
//    for (std::string f : resume_files) {
//        auto buf = load_file(f.c_str());
//        lt::error_code ec;
//        if (buf.size()) {
//            lt::add_torrent_params atp = lt::read_resume_data(buf);
//            handle = ses->add_torrent(atp, ec);
//
//            //添加每个种子的下载信息到list
//            std::shared_ptr<const libtorrent::torrent_info> info = handle.torrent_file();
//            if (info) {
//                Torrent_Msg* m = new Torrent_Msg();
//                m->title = QString::fromStdString(info->name());
//                tm_list.append(m);
//            }
//        }
//
//    }
//
//
//    return ses;
//}
//
////获取resume文件
//std::vector<std::string> get_resume_files() {
//    std::vector<std::string> resumeFiles;
//
//    // 获取当前目录
//    fs::path currentDir = fs::current_path();
//
//    for (const auto& entry : fs::directory_iterator(currentDir)) {
//        if (entry.is_regular_file()) {
//            // 获取文件名
//            std::string filename = entry.path().filename().string();
//
//            // 检查文件名是否以 ".resume" 开头
//            if (filename.rfind(".resume", 0) == 0) {
//                resumeFiles.push_back(filename);  // 将符合条件的文件加入到 vector 中
//            }
//        }
//    }
//
//
//    /*
//    for (int i = 0; i < handles.size(); i++) {
//         std::shared_ptr<const libtorrent::torrent_info> info = handles[i].torrent_file();
//        std::string file = ".resume_";
//        if (info) {
//            file.append(info->name());
//        }
//
//        qDebug() << file;
//        resume_files.push_back(file);
//    }*/
//    return resumeFiles;
//}
//
//bool isDownloading() {
//    return g_isDownloading;
//}
//
//lt::torrent_handle add_torrent_to_ses_by_magent(lt::session* ses, std::string magent_uri, std::string save_path) {
//
//
//
//    std::vector<std::string> resume_files = get_resume_files();
//    lt::add_torrent_params magnet = lt::parse_magnet_uri(magent_uri);
//    for (std::string file : resume_files) {
//        auto buf = load_file(file.c_str());
//        if (buf.size()) {
//            lt::add_torrent_params atp = lt::read_resume_data(buf);
//            if (atp.info_hashes == magnet.info_hashes) {
//                magnet = std::move(atp);
//                break;
//            }
//        }
//    }
//
//
//    magnet.save_path = save_path; // save in current dir
//    //ses->async_add_torrent(std::move(magnet));
//    //g_handle = ses->add_torrent(std::move(magnet));
//    lt::error_code ec;
//    lt::torrent_handle handle = ses->add_torrent(std::move(magnet), ec);
//
//    if (ec) {
//        std::cerr << "添加 torrent 失败: " << ec.message() << std::endl;
//    }
//    else {
//        std::cout << "torrent 添加成功！" << std::endl;
//    }
//
//    return handle;
//}
//
//
//
//
//std::string getDownloadTorrentName(lt::session* ses, std::string magent_uri, std::string save_path) {
//    int i = 200;
//    lt::torrent_handle h;
//
//    while (i--) {
//        if (ses) {
//
//            if (h.is_valid()) {
//                std::shared_ptr<const libtorrent::torrent_info> info = h.torrent_file();
//                if (info) {
//                    download_torrent_name = info->name();
//                    break;
//                }
//            }
//            else {
//                h = add_torrent_to_ses_by_magent(ses, magent_uri, save_path);
//            }
//
//        }
//
//
//        QThread::msleep(100);
//    }
//    if (i < 0) {
//        /* QMessageBox::information(0, "提醒", "地址无法下载", QMessageBox::Yes ,
//             QMessageBox::Yes);*/
//        download_torrent_name = "";
//    }
//
//
//    return download_torrent_name;
//}
//
//QList<Torrent_Msg*>* get_torrent_list() {
//    return &tm_list;
//}
//
//
//
//void close_download() {
//    shut_down = true;
//}
//void start_download(lt::session* ses) {
//
//    clk::time_point last_save_resume = clk::now();
//
//    // this is the handle we'll set once we get the notification of it being
//        // added
//    lt::torrent_handle h;
//
//    std::signal(SIGINT, &sighandler);
//
//
//
//    for (;;) {
//        std::vector<lt::alert*> alerts;
//        ses->pop_alerts(&alerts);
//
//        auto const handles = ses->get_torrents();
//        for (int i = 0; i < handles.size(); i++) {
//            std::shared_ptr<const libtorrent::torrent_info> info = handles[i].torrent_file();
//            // qDebug() << "******************************" ;
//        }
//
//        if (shut_down) {
//            shut_down = false;
//            auto const handles = ses->get_torrents();
//            for (int i = 0; i < handles.size(); i++) {
//                handles[i].save_resume_data(lt::torrent_handle::only_if_modified
//                    | lt::torrent_handle::save_info_dict);
//                g_done = true;
//            }
//
//            /* if (handles.size() == 1) {
//                 handles[0].save_resume_data(lt::torrent_handle::only_if_modified
//                     | lt::torrent_handle::save_info_dict);
//                 g_done = true;
//             }*/
//        }
//
//        for (lt::alert const* a : alerts) {
//            if (auto at = lt::alert_cast<lt::add_torrent_alert>(a)) {
//                h = at->handle;
//            }
//            // if we receive the finished alert or an error, we're done
//            if (lt::alert_cast<lt::torrent_finished_alert>(a)) {
//                h.save_resume_data(lt::torrent_handle::only_if_modified
//                    | lt::torrent_handle::save_info_dict);
//                g_done = true;
//            }
//            if (lt::alert_cast<lt::torrent_error_alert>(a)) {
//                std::cout << a->message() << std::endl;
//                g_done = true;
//                h.save_resume_data(lt::torrent_handle::only_if_modified
//                    | lt::torrent_handle::save_info_dict);
//            }
//
//            // when resume data is ready, save it
//            if (auto rd = lt::alert_cast<lt::save_resume_data_alert>(a)) {
//
//                // std::shared_ptr<const libtorrent::torrent_info> info = h.torrent_file();
//                std::string resume_file = ".resume_";
//                // if (info) {
//                resume_file.append(rd->torrent_name());
//                // }
//                std::ofstream of(resume_file, std::ios_base::binary);
//                of.unsetf(std::ios_base::skipws);
//                auto const b = write_resume_data_buf(rd->params);
//                of.write(b.data(), int(b.size()));
//                if (g_done) goto done;
//            }
//
//            if (lt::alert_cast<lt::save_resume_data_failed_alert>(a)) {
//                if (g_done) goto done;
//            }
//
//            if (auto st = lt::alert_cast<lt::state_update_alert>(a)) {
//                if (st->status.empty()) continue;
//
//                // we only have a single torrent, so we know which one
//                // the status is for
//                lt::torrent_status const& s = st->status[0];
//                qDebug() << '\r' << state(s.state) << ' '
//                    << (s.download_payload_rate / 1000) << " kB/s "
//                    << (s.total_done / 1000) << " kB ("
//                    << (s.progress_ppm / 10000) << "%) downloaded ("
//                    << s.num_peers << " peers)\x1b[K";
//                //emit update_alert_msg();
//               // std::cout.flush();
//            }
//        }
//        std::this_thread::sleep_for(std::chrono::milliseconds(200));
//
//        // ask the session to post a state_update_alert, to update our
//        // state output for the torrent
//        ses->post_torrent_updates();
//
//
//
//        // save resume data once every 30 seconds
//        if (clk::now() - last_save_resume > std::chrono::seconds(30)) {
//            h.save_resume_data(lt::torrent_handle::only_if_modified
//                | lt::torrent_handle::save_info_dict);
//            last_save_resume = clk::now();
//        }
//    }
//
//done:
//    qDebug() << "\nsaving session state";
//    {
//        std::ofstream of(".session", std::ios_base::binary);
//        of.unsetf(std::ios_base::skipws);
//        auto const b = write_session_params_buf(ses->session_state()
//            , lt::save_state_flags_t::all());
//        of.write(b.data(), int(b.size()));
//    }
//    g_isDownloading = false;
//}
//
////int test() {
////
////    try
////    {
////
////        lt::session ses = init_session();
////
////       
////
////        add_torrent_to_ses_by_magent(&ses,
////            "magnet:?xt=urn:btih:4A03090BA7D3FE33D8784EBBC543723CA2B50467&dn=Various%20Artists%20-%20NOW%20That%27s%20What%20I%20Call%20A%20Love%20Song%20%284CD%29%20%282023%29%20Mp3%20320kbps%20%5bPMEDIA%5d%20%e2%ad%90%ef%b8%8f&tr=udp%3a%2f%2ftracker.opentrackr.org%3a1337&tr=udp%3a%2f%2fopen.stealth.si%3a80%2fannounce&tr=udp%3a%2f%2ftracker.torrent.eu.org%3a451%2fannounce&tr=udp%3a%2f%2ftracker.bittor.pw%3a1337%2fannounce&tr=udp%3a%2f%2fpublic.popcorn-tracker.org%3a6969%2fannounce&tr=udp%3a%2f%2ftracker.dler.org%3a6969%2fannounce&tr=udp%3a%2f%2fexodus.desync.com%3a6969&tr=udp%3a%2f%2fopen.demonii.com%3a1337%2fannounce"
////            , "e:\\downloads");
////
////        start_download(&ses);
////
////        qDebug() << "\ndone, shutting down" ;
////    }
////    catch (std::exception& e)
////    {
////        qDebug() << "Error: " << e.what() ;
////        return -1;
////    }
////    return 0;
////}
//
//*/