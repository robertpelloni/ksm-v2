#include "ksmplatform_macos/input_method.h"
#include <Carbon/Carbon.h>

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
