#include <Siv3D.hpp>

void Main()
{
    Window::SetTitle(U"K-Shoot MANIA Editor");

    while (System::Update())
    {
        ClearPrint();
        Print << U"Editor Placeholder";
    }
}
