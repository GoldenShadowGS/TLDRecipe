#include "PCH.h"

class Application
{
public:
	BOOL Init(HINSTANCE hInstance);
	int Run();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	LRESULT CALLBACK InternalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void IncreaseRecipe();
	void DecreaseRecipe();
	void Reset();
	void SetTextFile(int count);
	HINSTANCE hInst = nullptr;
	HWND hWindow = nullptr;
	int m_WindowWidth = 400;
	int m_WindowHeight = 400;
	int m_Recipes = 0;
	static const int m_MAXRECIPES = 8;
};