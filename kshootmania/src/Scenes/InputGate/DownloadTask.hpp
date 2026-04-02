#pragma once
#include <Siv3D.hpp>

namespace InputGate
{
    enum class DownloadResult
    {
        Success,
        NetworkError,
        FileWriteError
    };

    class DownloadTask
    {
    public:
        static Co::Task<DownloadResult> Download(StringView url, FilePathView savePath, std::function<void(double)> progressCallback);
    };
}
