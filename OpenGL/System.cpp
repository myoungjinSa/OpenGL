#include "System.h"
#include "OpenGL.h"
#include "Input.h"
#include "Scene.h"
#include "Renderer.h"

System::System() 
	: pOpenGL(nullptr),
	pScene(nullptr),
	pInput(nullptr),
	pRenderer(nullptr)
{

}

System::System(const System& other) {

}

System::~System() {

}

bool System::Initialize() {
	int screenWidth, screenHeight;
	bool result;

	pOpenGL = new OpenGL();
	if (!pOpenGL)
		return false;

	if (!InitializeWindows(pOpenGL, screenWidth, screenHeight)) {
		MessageBox(hWnd, L"Could not initialize the window", L"Error", MB_OK);
		return false;
	}

	if (!Input::Initialize()) {
		MessageBox(hWnd, L"Failed load KeyCodes.txt", L"Error", MB_OK);
		return false;
	}

	timer.Start();

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	pRenderer = new Renderer();
	if (!pRenderer)
	{
		return false;
	}
	
	// Initialize the graphics object.
	result = pRenderer->Initialize(pOpenGL, hWnd);
	if (!result)
	{
		return false;
	}

	pScene = new Scene();
	if (!pScene)
	{
		return false;
	}
	if (!pScene->BuildObject(*pRenderer, hWnd)) {
		return false;
	}
	return true;
}

void System::Shutdown() {
	if (pScene) {
		pScene->Shutdown(*pRenderer);
		delete pScene;
		pScene = nullptr;
	}
	// Release the graphics object.
	if (pRenderer)
	{
		delete pRenderer;
		pRenderer = nullptr;
	}

	// Release the input object.
	if (pInput)
	{
		delete pInput;
		pInput = nullptr;
	}

	// Release the OpenGL object.
	if (pOpenGL)
	{
		pOpenGL->Shutdown(hWnd);
		delete pOpenGL;
		pOpenGL = nullptr;
	}

	timer.Reset();
	// Shutdown the window.
	ShutdownWindows();
}

void System::Run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			timer.Tick(60.0f);
			
			// Otherwise do the frame processing.
			result = Frame(timer.GetEalapsedTime());
			if (!result)
			{
				done = true;
			}
		}

	}

	return;
}

bool System::Frame(float elapsedTime)
{
	// Check if the user pressed escape and wants to exit the application.
	if (Input::IsKeyDown(KEY_ESCAPE))
	{
		return false;
	}


	bool result = false;
	if (pScene) {
		pScene->Update(elapsedTime);
		result = pScene->Render(*pRenderer);
	}

	return result;
}

LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
		// Check if a key has been pressed on the keyboard.
	case WM_KEYDOWN:
	{
		// If a key is pressed send it to the input object so it can record that state.
		Input::KeyDown((unsigned int)wParam);
		return 0;
	}

	// Check if a key has been released on the keyboard.
	case WM_KEYUP:
	{
		// If a key is released then send it to the input object so it can unset the state for that key.
		Input::KeyUp((unsigned int)wParam);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		
		Input::ProcessLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		SetCapture(hWnd);
		
		return 0;
	}
	case WM_MOUSEMOVE:
	{	
		
		Input::ProcessMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_LBUTTONUP:
	{

		Input::ProcessLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		ReleaseCapture();
		
		return 0;
	}
	default:
	{
		return DefWindowProc(hwnd, umsg, wParam, lParam);
	}
	}
}


bool System::InitializeWindows(OpenGL* pOpenGL, int& screenWidth, int& screenHeight) {
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;
	bool result;

	//Get an external pointer to this object.
	applicationHandle = this;

	//Get the instance of this application.
	hInstance = GetModuleHandle(NULL);

	//Give the application a name.
	applicationName = L"Engine";

	//Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Create a temporary window for the OpenGL extension setup.
	hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName, WS_POPUP,
		0, 0, 640, 480, NULL, NULL, hInstance, NULL);
	if (hWnd == NULL)
	{
		return false;
	}
	// Don't show the window.
	ShowWindow(hWnd, SW_HIDE);

	//Initialize a temporary OpenGL window and load the OpenGL extensions.
	result = pOpenGL->InitializeExtensions(hWnd);
	if (!result)
	{
		MessageBox(hWnd, L"Could not initialize the OpenGL extensions.", L"Error", MB_OK);
		return false;
	}

	//Release the temporary window now that the extensions have been initialized.
	DestroyWindow(hWnd);
	hWnd = NULL;

	//Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//Setup the screen settings depending on wheter it is running in full screen or in windowed mode.
	if (FULL_SCREEN) {
		//If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize			= sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight	= (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel	= 32;
		dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else {
		//If windowed then set it to 800x600 resolution
		screenWidth  = 1280;
		screenHeight = 960;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName, WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, hInstance, NULL);

	// Initialize OpenGL now that the window has been created.
	result = pOpenGL->InitializeOpenGL(hWnd, screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR, VSYNC_ENABLED);
	if (!result)
	{
		MessageBox(hWnd, L"Could not initialize OpenGL, check if video card supports OpenGL 4.0.", L"Error", MB_OK);
		return false;
	}
	// Bring the window up on the screen and set it as main focus.
	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	// Hide the mouse cursor.
	ShowCursor(false);
	return true;
}


void System::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(hWnd);
	hWnd = NULL;

	// Remove the application instance.
	UnregisterClass(applicationName, hInstance);
	hInstance = NULL;

	// Release the pointer to this class.
	applicationHandle = NULL;

	return;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return applicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}