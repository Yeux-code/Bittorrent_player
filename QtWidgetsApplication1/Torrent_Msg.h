#pragma once
#include <libtorrent/alert.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/session.hpp>
class Torrent_Msg
{
public:
	lt::torrent_handle handle;
	QString title;//torrent标题
	int download_payload_rate;//下载速度
	int total_done;//已下载数量
	int progress_ppm;//已下载百分比
	int num_peers;//用户数
	lt::sha1_hash hash;
};

