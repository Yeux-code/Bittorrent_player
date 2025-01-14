#include "stdafx.h"
#include "QtWidgetsApplication1.h"
#include <QtWidgets/QApplication>

void download_torrent_file_by_magnet() {
    try {
        // 创建 libtorrent 会话
        libtorrent::session ses;

        // 你的磁力链接（可以替换为实际的磁力链接）
        std::string magnet_link = "magnet:?xt=urn:btih:4A03090BA7D3FE33D8784EBBC543723CA2B50467&dn=Various%20Artists%20-%20NOW%20That%27s%20What%20I%20Call%20A%20Love%20Song%20%284CD%29%20%282023%29%20Mp3%20320kbps%20%5bPMEDIA%5d%20%e2%ad%90%ef%b8%8f&tr=udp%3a%2f%2ftracker.opentrackr.org%3a1337&tr=udp%3a%2f%2fopen.stealth.si%3a80%2fannounce&tr=udp%3a%2f%2ftracker.torrent.eu.org%3a451%2fannounce&tr=udp%3a%2f%2ftracker.bittor.pw%3a1337%2fannounce&tr=udp%3a%2f%2fpublic.popcorn-tracker.org%3a6969%2fannounce&tr=udp%3a%2f%2ftracker.dler.org%3a6969%2fannounce&tr=udp%3a%2f%2fexodus.desync.com%3a6969&tr=udp%3a%2f%2fopen.demonii.com%3a1337%2fannounce";

        // 通过磁力链接构建 torrent 参数
        libtorrent::add_torrent_params p = libtorrent::parse_magnet_uri(magnet_link);
        p.save_path = "A.torrent";
        // 将 torrent 添加到会话中
        libtorrent::torrent_handle handle = ses.add_torrent(p);
        std::shared_ptr<const libtorrent::torrent_info> info = 0;
        // 等待元数据下载并保存 .torrent 文件
        while (true) {
            std::vector<libtorrent::alert*> alerts;
            if (info = handle.torrent_file()) {


            }
            handle.save_resume_data(lt::torrent_handle::only_if_modified
                | lt::torrent_handle::save_info_dict);


            ses.pop_alerts(&alerts);
            for (libtorrent::alert* alert : alerts) {
                qDebug() << alert->message();
                if (auto rd = lt::alert_cast<lt::save_resume_data_alert>(alert)) {
                    if (info) {
                        std::ofstream of("B.torrent", std::ios_base::binary);
                        of.unsetf(std::ios_base::skipws);
                        auto const b = write_torrent_file_buf(rd->params, libtorrent::write_torrent_flags_t{});
                        of.write(b.data(), int(b.size()));
                        /*rd->params.save_path = "B.torrent";
                        write_torrent_file(rd->params);*/
                        goto done;

                    }

                }

            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 等待
        }

        //save_torrent_file(handle);
    done:
        qDebug() << "SSSSSSSSSSS";
    }
    catch (const std::exception& e) {
        qDebug() << "Error: " << e.what();
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtWidgetsApplication1 w;
   
    w.show();
    return a.exec();

    return 0;
}
