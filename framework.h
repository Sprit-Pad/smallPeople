#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <string>
#include "misc.h"
#include "high_resolution_timer.h"

#ifdef USE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

#include <d3d11.h>
#include <filesystem>
#include "sprite.h"
#include "sprite_batch.h"
#include <wrl.h>
#include <memory>
#include "Graphics.h"
#include "Input/Input.h"


CONST BOOL FULLSCREEN{ FALSE };
CONST LPCWSTR APPLICATION_NAME{ L"アクションゲーム" };

class framework
{
public:
	CONST HWND hwnd;

	 //設定
	 Graphics graphics;
	 Input input;


	

	framework(HWND hwnd);
	~framework();

	framework(const framework&) = delete;
	framework& operator=(const framework&) = delete;
	framework(framework&&) noexcept = delete;
	framework& operator=(framework&&) noexcept = delete;

	int run();
	

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	

private:
	bool initialize();
	void update(float elapsed_time/*Elapsed seconds from last frame*/);
	void render(float elapsed_time/*Elapsed seconds from last frame*/);
	bool uninitialize();

private:
	high_resolution_timer tictoc;
	uint32_t frames{ 0 };
	float elapsed_time{ 0.0f };
	
	void calculate_frame_stats();
	
	

};


	 