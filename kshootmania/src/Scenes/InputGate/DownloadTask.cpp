#include "DownloadTask.hpp"

namespace InputGate
{
    Co::Task<DownloadResult> DownloadTask::Download(StringView url, FilePathView savePath, std::function<void(double)> progressCallback)
    {
        // Use Siv3D's AsyncHTTPTask
        AsyncHTTPTask task = SimpleHTTP::SaveAsync(url, savePath);

        while (!task.isReady())
        {
            if (progressCallback)
            {
                // progress() returns download progress in bytes
                if (task.getProgress().has_value())
                {
                    const auto progress = task.getProgress().value();
                    if (progress.total_bytes && progress.total_bytes.value() > 0)
                    {
                        progressCallback(static_cast<double>(progress.downloaded_bytes) / progress.total_bytes.value());
                    }
                }
            }
            co_await Co::NextFrame();
        }

        const auto response = task.getResponse();

        if (response.isOK())
        {
            if (progressCallback) progressCallback(1.0);
            co_return DownloadResult::Success;
        }
        else if (response.getStatusCode() == HTTPStatusCode::OK && !FileSystem::Exists(savePath))
        {
            // Edge case: HTTP OK but file writing failed locally
            co_return DownloadResult::FileWriteError;
        }
        else
        {
            Logger << U"[ksm error] HTTP GET failed for URL: {} with status: {}"_fmt(url, EnumToString(response.getStatusCode()));
            co_return DownloadResult::NetworkError;
        }
    }
}
