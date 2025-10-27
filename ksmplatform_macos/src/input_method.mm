#include "ksmplatform_macos/input_method.h"
#include <Carbon/Carbon.h>
#include <IOKit/hidsystem/ev_keymap.h>
#include <CoreGraphics/CoreGraphics.h>

void KSMPlatformMacOS_DisableIME()
{
	// ASCII入力ソースのリストを取得
	CFArrayRef asciiSources = TISCreateASCIICapableInputSourceList();
	if (asciiSources == NULL)
	{
		return;
	}

	if (CFArrayGetCount(asciiSources) > 0)
	{
		// 最初のASCII入力ソースを選択(通常は英語キーボード)
		TISInputSourceRef asciiSource = static_cast<TISInputSourceRef>(
			const_cast<void*>(CFArrayGetValueAtIndex(asciiSources, 0))
		);
		TISSelectInputSource(asciiSource);
	}

	CFRelease(asciiSources);
}

bool KSMPlatformMacOS_IsKeyPressed(int keyCode)
{
	// CGKeyCodeは0〜127の範囲なので、範囲チェック
	if (keyCode < 0 || keyCode > 127)
	{
		return false;
	}

	return CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, static_cast<CGKeyCode>(keyCode));
}
