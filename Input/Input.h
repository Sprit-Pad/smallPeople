#pragma once

#include "GamePad.h"
#include "Mouse.h"

// インプット
class Input
{
public:
	Input(HWND hWnd);
	~Input() {}

public:
	// インスタンス取得
	static Input& Instance() { return *instance; }

	// 更新処理
	void Update();

	// ゲームパッド取得
	GamePad& GetGamePad() { return gamePad; }

	// マウス取得
	Mouse& GetMouse() { return mouse; }

	// マウスホイールの値を取得
	int GetMouseWheelDelta() const { return mouse.GetWheel(); }

private:
	static Input*		instance;
	GamePad				gamePad;
	Mouse				mouse;

};
