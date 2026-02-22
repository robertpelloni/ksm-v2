#include "DownloadTask.hpp"

namespace InputGate
{
    Co::Task<bool> DownloadTask::Download(StringView url, FilePathView savePath, std::function<void(double)> progressCallback)
    {
        // Use Siv3D's AsyncHTTPTask
        AsyncHTTPTask task = SimpleHTTP::CreateGetTask(url, savePath);

        while (!task.isReady())
        {
            if (progressCallback)
            {
                // progress() returns download progress in bytes? No, usually not available in basic interface easily without Content-Length
                // But let's check Siv3D docs memory: AsyncHTTPTask has getProgress().
                // It returns { downloaded, total }.
                const auto progress = task.getProgress();
                if (progress.total_bytes > 0)
                {
                    progressCallback(static_cast<double>(progress.downloaded_bytes) / progress.total_bytes);
                }
            }
            co_await Co::NextFrame();
        }

        if (task.getResponse().isOK())
        {
            if (progressCallback) progressCallback(1.0);
            co_return true;
        }
        else
        {
            co_return false;
        }
    }
}
