#include "PCH.h"
#include "Application.h"
#include "Resource.h"

const WCHAR* gTitle = L"TLD Recipes";
const WCHAR* gWindowClass = L"MainWindowClass";

static ATOM RegisterWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Application::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = gWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALLICON));
	return RegisterClassExW(&wcex);
}

BOOL Application::Init(HINSTANCE hInstance)
{
	hInst = hInstance;
	RegisterWindowClass(hInstance);

	const DWORD style = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;  // WS_POPUP
	const DWORD exstyle = WS_EX_LTRREADING;

	RECT winRect = { 0, 0, m_WindowWidth, m_WindowHeight };
	AdjustWindowRectEx(&winRect, style, false, exstyle);

	hWindow = CreateWindowExW(exstyle, gWindowClass, gTitle, style, CW_USEDEFAULT, 0,
		winRect.right - winRect.left, winRect.bottom - winRect.top, nullptr, nullptr, hInst, this);

	if (!hWindow)
		return FALSE;

	ShowWindow(hWindow, SW_SHOW);
	UpdateWindow(hWindow);

	return TRUE;
}


int Application::Run()
{
	Reset();
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

void Application::IncreaseRecipe()
{
	m_Recipes++;
	if (m_Recipes > 8)
	{
		m_Recipes = 8;
	}
	SetTextFile(m_Recipes);
}

void Application::DecreaseRecipe()
{
	m_Recipes--;
	if (m_Recipes < 0)
	{
		m_Recipes = 0;
	}
	SetTextFile(m_Recipes);
}

void Application::Reset()
{
	m_Recipes = 0;
	SetTextFile(0);
}

void Application::SetTextFile(int count)
{
	std::wifstream inputrecipesfile;
	inputrecipesfile.open("Recipes.txt");
	if (!inputrecipesfile.is_open())
	{
		MessageBox(nullptr, nullptr, L"Can not open Recipes.txt", MB_OK);
		return;
	}

	std::wofstream outputfile;
	outputfile.open("StreamText.txt");
	if (!outputfile.is_open())
	{
		MessageBox(nullptr, nullptr, L"Can not open StreamText.txt", MB_OK);
		return;
	}

	std::wstring recipes[m_MAXRECIPES];

	for (int i = 0; i < m_MAXRECIPES; i++)
	{
		std::getline(inputrecipesfile, recipes[i]);
	}
	std::wstring recipetitle = L"Recipes Found: " + std::to_wstring(count) + L"\n";
	outputfile << recipetitle;
	for (int i = 0; i < count; i++)
	{
		outputfile << recipes[i];
		outputfile << L"\n";
	}
	SetWindowTextW(hWindow, recipetitle.c_str());
}

LRESULT CALLBACK Application::InternalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int MouseDelay = 0;
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWindow, &ps);
		EndPaint(hWindow, &ps);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		IncreaseRecipe();
	}
	break;
	case WM_RBUTTONDOWN:
	{
		DecreaseRecipe();
	}
	break;
	case WM_MBUTTONDOWN:
	{
		Reset();
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK Application::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static Application* app = nullptr;
	if (app)
		return app->InternalWndProc(hWnd, message, wParam, lParam);
	else
	{
		if (message == WM_CREATE)
		{
			app = reinterpret_cast<Application*>(((CREATESTRUCTW*)lParam)->lpCreateParams);
			return 0;
		}
		else
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
}
