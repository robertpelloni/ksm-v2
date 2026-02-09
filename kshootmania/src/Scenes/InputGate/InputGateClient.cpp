#include "InputGateClient.hpp"

namespace InputGate
{
	Co::Task<Array<SongInfo>> InputGateClient::fetchSongList()
	{
		// ネットワーク遅延をシミュレート
		co_await Co::Delay(0.5s);

		// ダミーデータを返す
		co_return Array<SongInfo>{
			{ U"Test Song 1", U"Test Artist 1", U"http://example.com/jacket1.jpg", U"http://example.com/song1.zip" },
			{ U"Test Song 2", U"Test Artist 2", U"http://example.com/jacket2.jpg", U"http://example.com/song2.zip" },
			{ U"K-Shoot MANIA Theme", U"K-Shoot MANIA", U"", U"" },
			{ U"New Feature Demo", U"Dev Team", U"", U"" },
			{ U"Network Test", U"Server", U"", U"" },
		};
	}
}
