#include "stdafx.h"
#include "Download_Thread.h"



// return the name of a torrent status enum
char const* Download_Thread::state(lt::torrent_status::state_t s)
{
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#endif
    switch (s) {
    case lt::torrent_status::checking_files: return "checking";
    case lt::torrent_status::downloading_metadata: return "dl metadata";
    case lt::torrent_status::downloading: return "downloading";
    case lt::torrent_status::finished: return "finished";
    case lt::torrent_status::seeding: return "seeding";
    case lt::torrent_status::checking_resume_data: return "checking resume";
   // case lt::torrent_status::paused: return "paused";
    default: return "<>";
    }
#ifdef __clang__
#pragma clang diagnostic pop
#endif
}


//删除某个下载任务
void Download_Thread::remove_task_by_name(QString name)
{
    Torrent_Msg* tm = get_tm_by_name(name);
    if (tm && tm->handle.is_valid()) {

        tm->handle.pause();


        ses->remove_torrent(tm->handle);

      
            lt::sha1_hash hash = tm->hash;
            for (Torrent_Msg* tm : tm_list) {
                if (tm->hash == hash) {
                    //删除listview中的对应item
                    emit remove_task_by_name_sig(tm->title);
                    tm_list.removeOne(tm);
                    delete tm;
                }
            }

            //删除.resume文件
            std::string resume_file = ".resume_" + name.toStdString();
            std::wstring wfile = stringToWstring(resume_file);
            // 删除文件
            if (DeleteFileW(wfile.c_str()) == 0) {
                std::cout << "文件删除成功: " << resume_file << std::endl;
            }
            else {
                std::perror("删除文件失败");
            }

      

        qDebug() << "删除任务：" << name;
    }
}



std::vector<char> Download_Thread::load_file(char const* filename)
{
    std::ifstream ifs(filename, std::ios_base::binary);
    ifs.unsetf(std::ios_base::skipws);
    return { std::istream_iterator<char>(ifs), std::istream_iterator<char>() };
}


//停止所有下载
void Download_Thread::sighandler(int) {
    shut_down = true;

}


void Download_Thread::set_uri(QString uri)
{
	this->magnet_uri = uri;
}



lt::session* Download_Thread::init_session() {
    // load session parameters
    auto session_params = load_file(".session");
    lt::session_params params = session_params.empty()
        ? lt::session_params() : lt::read_session_params(session_params);
    params.settings.set_int(lt::settings_pack::alert_mask
        , lt::alert_category::error
        | lt::alert_category::storage
        | lt::alert_category::status);
    ses = new lt::session(params);
    //  lt::session ses(params);

    if (!ses) return 0;

    std::vector<std::string> resume_files = get_resume_files();
    lt::torrent_handle handle;
    for (std::string f : resume_files) {
        auto buf = load_file(f.c_str());
        lt::error_code ec;
        if (buf.size()) {
            lt::add_torrent_params atp = lt::read_resume_data(buf);
            //handle = ses->add_torrent(atp, ec);

            lt::error_code ec;
            lt::torrent_handle handle = ses->add_torrent(atp, ec);
          //  ses->async_add_torrent(atp);
            if (ec) {
                qDebug() << "添加 torrent 失败: " << ec.message();
            }
            else {
                qDebug() << "torrent 添加成功！";
            }
            

           
        }

    }

    //ses->pause();
    return ses;
}


//获取resume文件
std::vector<std::string> Download_Thread::get_resume_files() {
    std::vector<std::string> resumeFiles;

    // 获取当前目录
    fs::path currentDir = fs::current_path();

    for (const auto& entry : fs::directory_iterator(currentDir)) {
        if (entry.is_regular_file()) {
            // 获取文件名
            std::string filename = entry.path().filename().string();

            // 检查文件名是否以 ".resume" 开头
            if (filename.rfind(".resume", 0) == 0) {
                resumeFiles.push_back(filename);  // 将符合条件的文件加入到 vector 中
            }
        }
    }

    return resumeFiles;
}

//增加下载任务到session
lt::torrent_handle Download_Thread::add_torrent_to_ses_by_magent() {

    std::vector<std::string> resume_files = get_resume_files();
    lt::add_torrent_params magnet;
    try {
        magnet = lt::parse_magnet_uri(magnet_uri.toStdString());
    }
    catch (const std::exception& excep) {
        // Print the error message
        qDebug() << "Caught exception: " << excep.what() ;
    }
   



    for (std::string file : resume_files) {
        auto buf = load_file(file.c_str());
        if (buf.size()) {
            lt::add_torrent_params atp = lt::read_resume_data(buf);
            if (atp.info_hashes == magnet.info_hashes) {
                magnet = std::move(atp);
                break;
            }
        }
    }

   
    magnet.save_path = save_path.toStdString(); // save in current dir

    lt::error_code ec;
    lt::torrent_handle handle = ses->add_torrent(std::move(magnet), ec);

    if (ec) {
        qDebug() << "添加 torrent 失败: " << ec.message();
    }
    else {
        qDebug() << "torrent 添加成功！";
    }

    return handle;
}

//QString Download_Thread::getDownloadTorrentName() {
//    lt::torrent_handle h;
//    std::shared_ptr<const libtorrent::torrent_info> info;
//    QString download_torrent_name="";
//    lt::add_torrent_params* params=0;
//    int count = 100;
//        if (ses) {
//            h = add_torrent_to_ses_by_magent();
//            while (count--) {
//                if (h.is_valid()) {
//                    //download_torrent_name = QString::fromStdString(params->name.c_str());
//                    libtorrent::torrent_status status = h.status();
//                    if (info = status.handle.torrent_file())
//                        download_torrent_name = QString::fromStdString(info->name());
//                    //emit update_listview_items(&tm_list);
//                }
//                QThread::msleep(100);
//            }
//           
//
//        }
//
//
//    return download_torrent_name;
//}


//通过名字获取tm_list的某一个信息
Torrent_Msg* Download_Thread::get_tm_by_name(QString name) {
    for (Torrent_Msg* tm : tm_list) {
        if (tm->title == name)
            return tm;
    }
    return 0;
}

//停止某一个下载任务
void Download_Thread::pause_download_by_name(QString name)
{
   // ses->pause();
    auto const handles = ses->get_torrents();
    lt::sha1_hash* psh = get_hash_by_name(name);
    if (psh) {
        for (int i = 0; i < handles.size(); i++) {


            if (handles[i].info_hash() == *psh) {
               
                lt::torrent_status ts = handles[i].status();
               
                //bool IsPaused = (ts.flags & (lt::torrent_flags::auto_managed | lt::torrent_flags::paused)) == lt::torrent_flags::paused ? true : false;
                libtorrent::torrent_flags_t flags = handles[i].flags();
                if (flags & libtorrent::torrent_flags::paused) {
                    flags |= libtorrent::torrent_flags::auto_managed;  
                    handles[i].set_flags(flags);
                    handles[i].resume();
                }
                else {
                    flags &= ~libtorrent::torrent_flags::auto_managed;  // 通过按位与操作清除 auto_managed 标志

                    // 设置新的 flags
                    //handles[i].set_flags(flags);
                    handles[i].unset_flags(libtorrent::torrent_flags::auto_managed);
                    handles[i].pause();
                }
               
            }

           
        }
    }
}


//通过哈希值获取某一个信息
Torrent_Msg* Download_Thread::get_tm_by_hash(lt::sha1_hash hash)
{
    for (Torrent_Msg* tm : tm_list) {
        if (tm->hash==hash)
            return tm;
    }
    return nullptr;
}
//通过名字获取某一个哈希值
lt::sha1_hash* Download_Thread::get_hash_by_name(QString name) {
    for (Torrent_Msg* m : tm_list) {
        if (m->title == name) {
            return &m->hash;
        }
    }
    return 0;
}


//开始下载任务循环
void Download_Thread::start_download(lt::session* ses) {

    clk::time_point last_save_resume = clk::now();

    // this is the handle we'll set once we get the notification of it being
        // added
    lt::torrent_handle h;

    //std::signal(SIGINT, &sighandler);



    for (;;) {
        std::vector<lt::alert*> alerts;
        ses->pop_alerts(&alerts);
       
       

        if (shut_down) {
            shut_down = false;
            auto const handles = ses->get_torrents();
            for (int i = 0; i < handles.size(); i++) {
                handles[i].save_resume_data(lt::torrent_handle::only_if_modified
                    | lt::torrent_handle::save_info_dict);
                pending_saves++;
                //g_done = true;
            }
            g_done = true;
            if (handles.size() == 0)
                goto done;
        }

        for (lt::alert const* a : alerts) {
            if (auto at = lt::alert_cast<lt::add_torrent_alert>(a)) {
                h = at->handle;
               
            }
            // if we receive the finished alert or an error, we're done
            if (lt::alert_cast<lt::torrent_finished_alert>(a)) {
                h.save_resume_data(lt::torrent_handle::only_if_modified
                    | lt::torrent_handle::save_info_dict);
                //g_done = true;
            }
            if (lt::alert_cast<lt::torrent_error_alert>(a)) {
                std::cout << a->message() << std::endl;
                g_done = true;
                h.save_resume_data(lt::torrent_handle::only_if_modified
                    | lt::torrent_handle::save_info_dict);
            }

            // when resume data is ready, save it
            if (auto rd = lt::alert_cast<lt::save_resume_data_alert>(a)) {
               

                // std::shared_ptr<const libtorrent::torrent_info> info = h.torrent_file();
                std::string resume_file = ".resume_";
                resume_file.append(rd->torrent_name());
                std::wstring wfile = stringToWstring(resume_file);

                std::ofstream of(wfile, std::ios_base::binary);
                of.unsetf(std::ios_base::skipws);
                auto const b = write_resume_data_buf(rd->params);
                of.write(b.data(), int(b.size()));
                if (g_done&&!(--pending_saves)) goto done;
            }

            if (lt::alert_cast<lt::save_resume_data_failed_alert>(a)) {
                if (g_done) goto done;
            }

            if (auto st = lt::alert_cast<lt::state_update_alert>(a)) {
                if (st->status.empty()) continue;

                // we only have a single torrent, so we know which one
                // the status is for
                lt::torrent_status const& s = st->status[0];
                qDebug() << '\r' << state(s.state) << ' '
                    << (s.download_payload_rate / 1000) << " kB/s "
                    << (s.total_done / 1000) << " kB ("
                    << (s.progress_ppm / 10000) << "%) downloaded ("
                    << s.num_peers << " peers)\x1b[K";
   
                for (const auto& ts : st->status) {
                    lt::torrent_handle handle = ts.handle;
                   
                      if(handle.is_valid()){
                        lt::sha1_hash hash = handle.info_hash();
                        Torrent_Msg* ptm = get_tm_by_hash(hash);
                        
                        if (!ptm) {
                            ptm = new Torrent_Msg();
                            ptm->handle = handle;
                            ptm->title = QString::fromStdString(ts.name);
                            ptm->hash = hash;
                            tm_list.append(ptm);
                        }
                        
                       
                           
                        ptm->download_payload_rate = ts.download_payload_rate;
                        ptm->total_done = ts.total_done;
                        ptm->progress_ppm = ts.progress_ppm;
                        ptm->num_peers = ts.num_peers;
                        emit update_items_msg(&tm_list);
                    }
                      else {
                          qDebug() << "句柄无效";
                      }
                          
                    
                       
                }
               
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // ask the session to post a state_update_alert, to update our
        // state output for the torrent
        ses->post_torrent_updates();



        // save resume data once every 30 seconds
        if (clk::now() - last_save_resume > std::chrono::seconds(30)) {
            h.save_resume_data(lt::torrent_handle::only_if_modified
                | lt::torrent_handle::save_info_dict);
            last_save_resume = clk::now();
        }
    }

done:
    qDebug() << "\nsaving session state";
    {
        std::ofstream of(".session", std::ios_base::binary);
        of.unsetf(std::ios_base::skipws);
        auto const b = write_session_params_buf(ses->session_state()
            , lt::save_state_flags_t::all());
        of.write(b.data(), int(b.size()));
    }
}

std::wstring Download_Thread::stringToWstring(const std::string& str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t* wideStr = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wideStr, len);
    std::wstring wstr(wideStr);
    delete[] wideStr;
    return wstr;
}