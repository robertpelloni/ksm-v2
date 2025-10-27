#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// IMEを無効化し、ASCII入力ソースに切り替える
void KSMPlatformMacOS_DisableIME();

// プラットフォーム特有のキー状態を取得(仮想キーコードを指定)
bool KSMPlatformMacOS_IsKeyPressed(int keyCode);

#ifdef __cplusplus
}
#endif
