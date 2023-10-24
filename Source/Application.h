#include "PCH.h"
#include "Renderer.h"
#include "Image.h"

class Application
{
public:
	BOOL Init(HINSTANCE hInstance);
	int Run();
	void Update();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	LRESULT CALLBACK InternalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void Reset();
	void SetTextFile();
	Renderer m_Renderer;
	HINSTANCE hInst = nullptr;
	HWND hWindow = nullptr;
	int m_WindowWidth = 300;
	int m_WindowHeight = 450;
	static const int m_MAXRECIPES = 8;
	Image m_RecipeCountImage;
	Image m_Images[m_MAXRECIPES];
	int m_ButtonArray[m_MAXRECIPES] = { 7, 1, 2, 3, 4, 5, 6, 0 };
	std::wstring m_RecipeCountString;
	std::wstring m_RecipeStrings[m_MAXRECIPES];
	int m_RecipeCount = 0;
	void SelectRecipe(int index);
	int GetButtonIndex();
	int m_SelectedArray[m_MAXRECIPES] = {};
	int mouseX = -1;
	int mouseY = -1;
	ComPtr<ID2D1SolidColorBrush> HighlightBrush;
	int m_GrabbedIndex = -1;
};