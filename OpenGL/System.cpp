#include "System.h"
#include "OpenGL.h"
#include "Input.h"
#include "Scene.h"
#include "SceneEdit.h"
#include "Renderer.h"
#include "Logger.h"
#include "RayCast.h"

System::System() 
	: pGraphicDevice(nullptr),
	pScene(nullptr),
	pSceneEdit(nullptr),
	pInput(nullptr),
	pRenderer(nullptr),
	screenHeight(0),
	screenWidth(0)
{

}

System::System(const System& other) {

}

System::~System() {

}

bool System::Initialize() {
	bool result;
	
	std::setlocale(LC_ALL, "ko_KR");

	pGraphicDevice = new OpenGL();
	if (!pGraphicDevice)
		return false;

	if (!InitializeWindows(pGraphicDevice, screenWidth, screenHeight)) {
		MessageBox(hWnd, L"Could not initialize the window", L"Error", MB_OK);
		return false;
	}

	if (!KeyboardInput::Initialize()) {
		MessageBox(hWnd, L"Failed load KeyCodes.txt", L"Error", MB_OK);
		return false;
	}

	if (!MouseInput::Initialize()) {
		MessageBox(hWnd, L"Failed Initialize Mouse.", L"Error", MB_OK);
		return false;
	}

	timer.Start();

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	pRenderer = new OpenGLRenderer();
	if (!pRenderer)
	{
		return false;
	}
	
	// Initialize the graphics object.
	result = pRenderer->Initialize(pGraphicDevice);
	if (!result)
	{
		return false;
	}

	pScene = new Scene();
	if (!pScene)
	{
		return false;
	}
	if (!pScene->BuildObject(*pRenderer)) {
		return false;
	}

	pSceneEdit = new SceneEdit(*pScene);
	if (!pSceneEdit)
	{
		return false;
	}


	return true;
}

void System::Shutdown() {
	if (pSceneEdit) {
		delete pSceneEdit;
	}
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
	if (pGraphicDevice)
	{
		pGraphicDevice->Shutdown(hWnd);
		delete pGraphicDevice;
		pGraphicDevice = nullptr;
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
			// Check if the user pressed escape and wants to exit the application.
			if (KeyboardInput::IsKeyDown(KEY_ESCAPE)) {
				done = true;
				continue;
			}

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
	ProcessInput();
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
		KeyboardInput::KeyDown((unsigned int)wParam);
		return 0;
	}

	// Check if a key has been released on the keyboard.
	case WM_KEYUP:
	{
		// If a key is released then send it to the input object so it can unset the state for that key.
		KeyboardInput::KeyUp((unsigned int)wParam);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		SetCapture(hWnd);

		MouseInput::ProcessRButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		
		return 0;
	}
	case WM_RBUTTONUP:
	{
		MouseInput::ProcessRButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		ReleaseCapture();
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		SetCapture(hWnd);
		MouseInput::ProcessLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_MOUSEMOVE:
	{	
		MouseInput::ProcessMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_LBUTTONUP:
	{
		MouseInput::ProcessLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		ReleaseCapture();
		
		return 0;
	}
	default:
	{
		return DefWindowProc(hwnd, umsg, wParam, lParam);
	}
	}
}

void System::ProcessInput() {
	if (GetCapture() == hWnd) {
		POINT ptCursorPos;
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		ScreenToClient(hWnd, &ptCursorPos);
		MouseInput::ProcessMouseMove(ptCursorPos.x, ptCursorPos.y);
	}
}
bool System::InitializeWindows(GraphicDevice* pGraphicDevice, int& screenWidth, int& screenHeight) {
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
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Failed To Register The Window Class.", L"ERROR",
			MB_OK | MB_ICONEXCLAMATION);
		return FALSE; // Exit And Return FALSE
	}

	// Create a temporary window for the OpenGL extension setup.
	hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName, WS_POPUP,
		0, 0, 1200, 800, NULL, NULL, hInstance, NULL);
	if (hWnd == NULL)
	{
		return false;
	}
	// Don't show the window.
	ShowWindow(hWnd, SW_HIDE);

	//Initialize a temporary OpenGL window and load the OpenGL extensions.
	result = pGraphicDevice->InitializeExtensions(hWnd);
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
		screenWidth  = 1200;
		screenHeight = 800;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName, WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, hInstance, NULL);

	// Initialize OpenGL now that the window has been created.
	result = pGraphicDevice->Initialize(hWnd, screenWidth, screenHeight, VSYNC_ENABLED);
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
	ShowCursor(true);
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