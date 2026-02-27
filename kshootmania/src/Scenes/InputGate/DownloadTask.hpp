#pragma once
#include <Siv3D.hpp>

namespace InputGate
{
    // Utility to download a file with progress tracking
    // This is a synchronous blocking operation in Siv3D's SimpleHTTP usually,
    // but for progress we might need a custom loop if SimpleHTTP doesn't support it directly in a task-friendly way.
    // However, Siv3D's SimpleHTTP::Save is blocking.
    // To support progress, we might need AsyncHTTPTask or similar if available, or just block for now in a thread.
    // Since we are in a Co::Task, we can use std::thread/std::async but need to be careful with Siv3D main thread restrictions.
    // For now, let's stick to SimpleHTTP::Save which is blocking, and wrap it in a thread if needed, or just accept the block (bad UX).
    // Better: Use `SimpleHTTP::GetAsync` (if it exists) or `SimpleHTTP::CreateGetTask`.

    // Siv3D 0.6.x has SimpleHTTP::Get() which returns HTTPResponse.
    // It also has AsyncHTTPTask SimpleHTTP::CreateGetTask(URL).

    class DownloadTask
    {
    public:
        static Co::Task<bool> Download(StringView url, FilePathView savePath, std::function<void(double)> progressCallback);
    };
}
