#pragma once
#include <libtorrent/alert.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/session.hpp>
class Torrent_Msg
{
public:
	lt::torrent_handle handle;
	QString title;//torrent����
	int download_payload_rate;//�����ٶ�
	int total_done;//����������
	int progress_ppm;//�����ذٷֱ�
	int num_peers;//�û���
	lt::sha1_hash hash;
};

