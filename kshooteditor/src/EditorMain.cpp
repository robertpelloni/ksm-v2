#ifndef HEADLESS_BUILD
#include <Siv3D.hpp>

void Main()
{
	Window::SetTitle(U"K-Shoot MANIA Editor");

	while (System::Update())
	{
		// TODO: Implement Editor UI with NocoUI
		ClearPrint();
		Print << U"Editor stub initialized.";
	}
}
#else
int main()
{
	return 0; // Empty headless main
}
#endif
