#include "patch.h"
#include "keygen.h"
#include "graphics/graphicIds.h"
#include "clip.h"

#include <stdexcept>
#include <string>
#include <Windows.h>
#include <windowsx.h>
#include <xutility>

enum class ButtonState
{
	Default,
	Focused,
	Pushed
};

bool IsKeygenFocused(LPARAM lParam)
{
	auto xPos = GET_X_LPARAM(lParam);
	auto yPos = GET_Y_LPARAM(lParam);

	if (xPos >= 269 && xPos <= 381 &&
		yPos >= 359 && yPos <= 389)
	{
		return true;
	}

	return false;
}

void DrawObject(HDC hdc, HDC hdcbitmap, HBITMAP bitmap, int x, int y)
{
	BITMAP bm;

	auto hOldBitmap = reinterpret_cast<HBITMAP> (SelectObject(hdcbitmap, bitmap));
	GetObject(bitmap, sizeof(BITMAP), &bm);
	BitBlt(hdc, x, y, bm.bmWidth, bm.bmHeight, hdcbitmap,
		0, 0, SRCCOPY);
	SelectObject(hdcbitmap, hOldBitmap);
}

INT_PTR DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP Logo = LoadBitmapA(GetModuleHandleA(nullptr), 
		MAKEINTRESOURCEA(IDB_BITMAP1));
	static HBITMAP FocusedButton = LoadBitmapA(GetModuleHandleA(nullptr),
		MAKEINTRESOURCEA(IDB_FOCUSED));
	static HBITMAP PushedButton = LoadBitmapA(GetModuleHandleA(nullptr),
		MAKEINTRESOURCEA(IDB_PUSHED));
	static ButtonState state = ButtonState::Default;
	static constexpr RECT rectangle{ 269, 359, 381, 389 };

	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		{		
			state = (IsKeygenFocused(lParam)) ? ButtonState::Focused
				: ButtonState::Default;
			SendMessageA(hwnd, WM_PAINT, 0, 0);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			if (IsKeygenFocused(lParam))
			{
				state = ButtonState::Pushed;
				SendMessageA(hwnd, WM_PAINT, 0, 0);

				try {
					Keygen generator;
					ClipBoard clip(GetDesktopWindow());
					const auto& UserName = generator.GetName();
					auto license = generator.GenerateLicenseKeyByName(UserName);
					for (auto it = license.begin(); it != std::prev(license.end()); ++it)
					{
						if (*it == 0x0A)
						{
							*it = '\r';
							it = license.insert(std::next(it), '\n');
						}
					}
					patch::FixPublicKey();
					clip.SetTextToClipboard(license);
					MessageBoxA(hwnd, "Key was copied to clipboard",
						"Success", MB_ICONEXCLAMATION);
				}
				catch (const std::exception& error)
				{
					MessageBoxA(hwnd, (LPSTR)error.what(), "Error", MB_ICONERROR);
				}
			}
		}
		break;
	case WM_PAINT:
		{			
			InvalidateRect(hwnd, &rectangle, TRUE);

			PAINTSTRUCT ps;
			auto hDC = BeginPaint(hwnd, &ps);
			auto hBitmapDC = CreateCompatibleDC(hDC);

			if (state == ButtonState::Focused)
			{
				DrawObject(hDC, hBitmapDC, FocusedButton,
					rectangle.left, rectangle.top);
			}
			else if (state == ButtonState::Pushed)
			{
				DrawObject(hDC, hBitmapDC, PushedButton, 
					rectangle.left, rectangle.top);
			}
			else
			{	
				DrawObject(hDC, hBitmapDC, Logo, 0, 0);
			}

			DeleteDC(hBitmapDC);
			EndPaint(hwnd, &ps);
			return FALSE;
		}
		break;
	case WM_INITDIALOG:
		CryptoInit::Init();		
		break;
	case WM_CLOSE:
		DeleteObject(Logo);
		DeleteObject(FocusedButton);
		DeleteObject(PushedButton);
		EndDialog(hwnd, NULL);
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,int nShowCmd)
{	
	if (DialogBoxParamA(GetModuleHandle(nullptr), "MyDialog",
		NULL, reinterpret_cast<DLGPROC> (DialogProc), NULL) < 0)
	{
		MessageBoxA(NULL, std::to_string(GetLastError()).c_str(), "Error", MB_ICONERROR);
	}
	return 0;
}