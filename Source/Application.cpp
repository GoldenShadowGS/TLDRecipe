#include "PCH.h"
#include "Application.h"
#include "Resource.h"

const WCHAR* gTitle = L"TLD Recipe Tracker";
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

	// Graphics Resource Init
	m_Renderer.Init(hWindow);
	ID2D1Factory2* factory = m_Renderer.GetFactory();
	ID2D1DeviceContext* dc = m_Renderer.GetDeviceContext();
	HR(dc->CreateSolidColorBrush(D2D1::ColorF(0.79f, 0.61f, 0.43f, 1.0f), HighlightBrush.ReleaseAndGetAddressOf()));

	m_RecipeCountString = L"Recipes Found: ";
	m_RecipeStrings[0] = L"Breyerhouse Pie\n";
	m_RecipeStrings[1] = L"Camber Flight Porridge\n";
	m_RecipeStrings[2] = L"Coastal Fish Cakes\n";
	m_RecipeStrings[3] = L"Dockworker's Pie\n";
	m_RecipeStrings[4] = L"Lily's Pancakes\n";
	m_RecipeStrings[5] = L"Ranger Stew\n";
	m_RecipeStrings[6] = L"Stalker's Pie\n";
	m_RecipeStrings[7] = L"Thomson Family Stew\n";

	std::wstring recipeCountString = m_RecipeCountString + std::to_wstring(m_RecipeCount) + L"\n";
	m_RecipeCountImage = CreateTextImage(dc, recipeCountString.c_str(), 300.0f, 50.0f, 24.0f, 8.0f);
	for (int i = 0; i < m_MAXRECIPES; i++)
	{
		m_Images[i] = CreateTextImage(dc, m_RecipeStrings[i].c_str(), 300.0f, 50.0f, 24.0f, 8.0f);
	}

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

void Application::Update()
{
	// Rendering
	// Skip Drawing if Window is Minimized
	if (!IsIconic(hWindow))
	{
		ID2D1DeviceContext* dc = m_Renderer.GetDeviceContext();
		dc->BeginDraw();

		D2D1::ColorF backGroundColor = D2D1::ColorF(0.8f, 0.8f, 0.8f, 1.0f);

		dc->Clear(backGroundColor);
		dc->SetTransform(D2D1::Matrix3x2F::Identity());

		{
			D2D_RECT_F Image_Rect = { 0, 0, m_RecipeCountImage.m_Size.width, m_RecipeCountImage.m_Size.height };
			dc->DrawBitmap(m_RecipeCountImage.m_Bitmap.Get(), Image_Rect, 1.0f);
		}
		for (int i = 0; i < m_MAXRECIPES; i++)
		{
			int index = m_ButtonArray[i];
			float height = i * 50.0f + 50.0f;
			D2D_RECT_F Rect = { 0, height, m_Images[index].m_Size.width, m_Images[index].m_Size.height + height };
			if (m_SelectedArray[index])
			{
				D2D_RECT_F Rect2 = { 3, height + 3, m_Images[index].m_Size.width - 3, m_Images[index].m_Size.height + height - 3 };
				dc->FillRectangle(Rect2, HighlightBrush.Get());
			}
			dc->DrawBitmap(m_Images[index].m_Bitmap.Get(), Rect, 1.0f);
		}

		HR(dc->EndDraw());
		HR(m_Renderer.GetSwapChain()->Present(1, 0));
	}
}

void Application::SelectRecipe(int index)
{
	if (index < 0 || index >= m_MAXRECIPES) // return if index out of bounds
		return;

	int buttonindex = m_ButtonArray[index];
	if (m_SelectedArray[buttonindex] == 0)
	{
		++m_RecipeCount;
		m_SelectedArray[buttonindex] = m_RecipeCount;
	}
	else
	{
		int value = m_SelectedArray[buttonindex];
		m_SelectedArray[buttonindex] = 0;

		while (value <= m_RecipeCount)
		{
			for (int i = 0; i < m_MAXRECIPES; i++)
			{
				if (m_SelectedArray[i] == value)
				{
					m_SelectedArray[i]--;
				}
			}
			value++;
		}
		--m_RecipeCount;
	}
	SetTextFile();
}

void Application::Reset()
{
	for (int i = 0; i < m_MAXRECIPES; i++)
	{
		m_SelectedArray[i] = 0;
	}
	m_RecipeCount = 0;
	SetTextFile();
}

int Application::GetButtonIndex()
{
	for (int i = 0; i < m_MAXRECIPES; i++)
	{
		int height = 50 * i;
		if (mouseY > 50 + height + 3 && mouseY <= 100 + height - 3)
			return i;
	}
	return -1;
}

void Application::SetTextFile()
{
	std::wofstream outputfile;
	outputfile.open("StreamText.txt");
	if (!outputfile.is_open())
	{
		MessageBox(nullptr, nullptr, L"Can not open StreamText.txt", MB_OK);
		return;
	}

	std::wstring recipeCountString = m_RecipeCountString + std::to_wstring(m_RecipeCount) + L"\n";
	ID2D1DeviceContext* dc = m_Renderer.GetDeviceContext();
	m_RecipeCountImage = CreateTextImage(dc, recipeCountString.c_str(), 300.0f, 50.0f, 24.0f, 8.0f);

	outputfile << recipeCountString;
	for (int count = 1; count <= m_RecipeCount; count++)
	{
		for (int i = 0; i < m_MAXRECIPES; i++)
		{
			if (m_SelectedArray[i] == count)
			{
				outputfile << m_RecipeStrings[i];
			}
		}
	}
}

LRESULT CALLBACK Application::InternalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int MouseDelay = 0;
	switch (message)
	{
	case WM_PAINT:
	{
		Update();
		PAINTSTRUCT ps;
		BeginPaint(hWindow, &ps);
		EndPaint(hWindow, &ps);
	}
	break;
	case WM_MOUSEMOVE:
	{
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		//if (m_Grabbed)
		//{

		//}
		//SetWindowTextW(hWindow, std::to_wstring(GetButtonIndex()).c_str());
	}
	break;
	case WM_LBUTTONDOWN:
	{
		SelectRecipe(GetButtonIndex());
		RECT rc;
		GetClientRect(hWnd, &rc);
		InvalidateRect(hWindow, &rc, TRUE);
	}
	break;
	case WM_RBUTTONDOWN:
	{
		m_GrabbedIndex = GetButtonIndex();
	}
	break;
	case WM_RBUTTONUP:
	{
		m_GrabbedIndex = -1;
	}
	break;
	case WM_MBUTTONDOWN:
	{
		Reset();
		RECT rc;
		GetClientRect(hWnd, &rc);
		InvalidateRect(hWindow, &rc, TRUE);
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
