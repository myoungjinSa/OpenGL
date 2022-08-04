#include "OpenGL.h"
#include "String/String.h"
#include "Logger.h"
OpenGL::OpenGL() 
	:screenHeight(0.0f),
	 screenWidth(0.0f)
{
	deviceContext = NULL;
	renderingContext = NULL;
}

OpenGL::~OpenGL() {

}


bool OpenGL::InitializeExtensions(HWND hWnd) {
	PIXELFORMATDESCRIPTOR pixelFormat;
	HGLRC renderContext;

	//Get the device context for this window.
	deviceContext = GetDC(hWnd);
	if (!deviceContext)
		return false;

	if (!SetPixelFormat(deviceContext, 1, &pixelFormat))
		return false;


	// Create a temporary rendering context.
	renderContext = wglCreateContext(deviceContext);
	if (!renderContext)
	{
		return false;
	}
	
	//Set the temporary rendering context as the current rendering context for this window.
	if (!wglMakeCurrent(deviceContext, renderContext))
		return false;

	// Initialize the OpenGL extensions needed for this application.  Note that a temporary rendering context was needed to do so.
	if (!LoadExtensionList()) 
		return false;
	
	//Release the temporary rendering context now that the extensions have been loaded.
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(renderContext);
	renderContext = NULL;

	ReleaseDC(hWnd, deviceContext);
	deviceContext = NULL;

	return true;
}

bool OpenGL::Initialize(HWND hWnd, int _screenWidth, int _screenHeight, bool vSync) {
	int attributeListInt[19] = { 0, };
	int pixelFormat[1] = { 0, };
	unsigned int formatCount = 0;
	int result = 0;
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	int attributeList[5] = { 0, };
	float fieldOfView, screenAspect = 0.0f;
	char* venderString, * renderString = nullptr;

	//Get the device context for this window
	deviceContext = GetDC(hWnd);
	if (!deviceContext)
		return false;

	screenWidth = _screenWidth;
	screenHeight = _screenHeight;
	//Support for OpenGL rendering
	attributeListInt[0] = WGL_SUPPORT_OPENGL_ARB;
	attributeListInt[1] = TRUE;

	//Support for rendering to a window
	attributeListInt[2] = WGL_DRAW_TO_WINDOW_ARB;
	attributeListInt[3] = TRUE;

	//Support for hardware acceleration
	attributeListInt[4] = WGL_ACCELERATION_ARB;
	attributeListInt[5] = WGL_FULL_ACCELERATION_ARB;

	//Support for 32bit color.
	attributeListInt[6] = WGL_COLOR_BITS_ARB;
	attributeListInt[7] = 32;

	//Support for 24bit depth buffer.
	attributeListInt[8] = WGL_DEPTH_BITS_ARB;
	attributeListInt[9] = 24;

	//Support for double buffer.
	attributeListInt[10] = WGL_DOUBLE_BUFFER_ARB;
	attributeListInt[11] = TRUE;

	//Support for swapping front and back buffer.
	attributeListInt[12] = WGL_SWAP_METHOD_ARB;
	attributeListInt[13] = WGL_SWAP_EXCHANGE_ARB;

	//Support for the RGBA pixel type.
	attributeListInt[14] = WGL_PIXEL_TYPE_ARB;
	attributeListInt[15] = WGL_TYPE_RGBA_ARB;

	//Support for a 8bit stencil buffer.
	attributeListInt[16] = WGL_STENCIL_BITS_ARB;
	attributeListInt[17] = 8;

	//Null terminate the attribute list
	attributeListInt[18] = 0;


	//Query for a pixel format that fits the attributes we want.
	result = wglChoosePixelFormatARB(deviceContext, attributeListInt, NULL, 1, pixelFormat, &formatCount);
	if (result != TRUE)
		return false;

	//if the video card/display can handle our desired pixel format then we set it as he current one
	result = SetPixelFormat(deviceContext, pixelFormat[0], &pixelFormatDescriptor);
	if (result != TRUE)
		return false;

	//Set the 4.0 Version of OpenGL in the attribute list.
	attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
	attributeList[1] = 4;
	attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
	attributeList[3] = 0;

	//Null termination the attribute list.
	attributeList[4] = 0;

	//Create a OpenGL 4.0 rendering context.
	renderingContext = wglCreateContextAttribsARB(deviceContext, 0, attributeList);
	if (renderingContext == NULL)
		return false;

	//Set the rendering context to active
	result = wglMakeCurrent(deviceContext, renderingContext);
	if (result != TRUE)
		return false;

	//Set the depth buffer to be entirely cleared to 1.0 values
	glClearDepth(1.0f);

	//Enable depth testing.
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	//Set the polygon winding to front facing for the left handed system.
	glFrontFace(GL_CW);

	//Enable back face culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Get the name of the video card.
	venderString = (char*)glGetString(GL_VENDOR);
	renderString = (char*)glGetString(GL_RENDERER);

	//Store the video card name in a class member variable so it can be retrieved later.
	strcpy_s(videoCardDescription, venderString);
	strcpy_s(videoCardDescription, " - ");
	strcpy_s(videoCardDescription, renderString);

	//Turn on or off the vertical sync depending on the inpu bool value;
	(vSync) ? result = wglSwapIntervalEXT(1) : result = wglSwapIntervalEXT(0);


	//Check if vSync was set correctly.
	if (result != TRUE)
		return false;
	return true;
} 

void OpenGL::Shutdown(HWND hWnd) {
	//Relase the rendering context.
	if (renderingContext) {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(renderingContext);
		renderingContext = NULL;
	}

	if (deviceContext) {
		ReleaseDC(hWnd, deviceContext);
		deviceContext = NULL;
	}
}

void OpenGL::BeginScene(float red, float green, float blue, float alpha) {
	//Set the color to clear the screen to.
	glClearColor(red, green, blue, alpha);

	//Clear the screen and depth buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	return;
}

void OpenGL::EndScene() {
	//Present the back buffer to the screen since rendering is complete.
	SwapBuffers(deviceContext);
}

bool OpenGL::LoadExtensionList() {
	// Load the OpenGL extensions that this application will be using.
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	if (!wglChoosePixelFormatARB)
	{
		return false;
	}

	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (!wglCreateContextAttribsARB)
	{
		return false;
	}

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if (!wglSwapIntervalEXT)
	{
		return false;
	}

	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	if (!glAttachShader)
	{
		return false;
	}

	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	if (!glBindBuffer)
	{
		return false;
	}

	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
	if (!glBindVertexArray)
	{
		return false;
	}

	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	if (!glBufferData)
	{
		return false;
	}


	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	if (!glCompileShader)
	{
		return false;
	}

	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	if (!glCreateProgram)
	{
		return false;
	}

	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	if (!glCreateShader)
	{
		return false;
	}

	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	if (!glDeleteBuffers)
	{
		return false;
	}

	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	if (!glDeleteProgram)
	{
		return false;
	}

	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	if (!glDeleteShader)
	{
		return false;
	}

	glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
	if (!glDeleteVertexArrays)
	{
		return false;
	}

	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	if (!glDetachShader)
	{
		return false;
	}

	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	if (!glEnableVertexAttribArray)
	{
		return false;
	}

	glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffersEXT");
	if (!glGenFramebuffersEXT) {
		return false;
	}

	glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
	if (!glBindFramebufferEXT)
	{
		return false;
	}

	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	if (!glGenBuffers)
	{
		return false;
	}

	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
	if (!glGenVertexArrays)
	{
		return false;
	}

	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	if (!glGetAttribLocation)
	{
		return false;
	}

	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
	if (!glGetProgramInfoLog)
	{
		return false;
	}

	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	if (!glGetProgramiv)
	{
		return false;
	}

	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	if (!glGetShaderInfoLog)
	{
		return false;
	}

	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	if (!glGetShaderiv)
	{
		return false;
	}

	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	if (!glLinkProgram)
	{
		return false;
	}

	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	if (!glShaderSource)
	{
		return false;
	}

	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	if (!glUseProgram)
	{
		return false;
	}

	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
	if (!glVertexAttribPointer)
	{
		return false;
	}

	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
	if (!glBindAttribLocation)
	{
		return false;
	}

	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	if (!glGetUniformLocation)
	{
		return false;
	}

	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	if (!glUniformMatrix4fv)
	{
		return false;
	}

	glUniformMatrix3fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix3fv");
	if (!glUniformMatrix3fv)
	{
		return false;
	}

	glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
	if (!glActiveTexture)
	{
		return false;
	}

	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	if (!glUniform1i)
	{
		return false;
	}

	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
	if (!glGenerateMipmap)
	{
		return false;
	}

	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	if (!glDisableVertexAttribArray)
	{
		return false;
	}

	glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	if (!glUniform3fv)
	{
		return false;
	}

	glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	if (!glUniform4fv)
	{
		return false;
	}

	glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)wglGetProcAddress("glStencilFuncSeparate");
	if (!glStencilFuncSeparate)
		return false;

	glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)wglGetProcAddress("glStencilOpSeparate");
	if (!glStencilOpSeparate)
		return false;

	glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)wglGetProcAddress("glStencilMaskSeparate");
	if (!glStencilMaskSeparate)
		return false;

	glEnablei = (PFNGLENABLEIPROC)wglGetProcAddress("glEnablei");
	if (!glEnablei)
		return false;
	
	glClearBufferiv = (PFNGLCLEARBUFFERIVPROC)wglGetProcAddress("glClearBufferiv");
	if (!glClearBufferiv)
		return false;

	glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
	if (!glCheckFramebufferStatus)
		return false;

	glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
	if (!glFramebufferTexture2D)
		return false;

	glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
	if (!glUniform1f)
		return false;

	return true;
}

void OpenGL::GetVideoCardInfo(char* cardName)
{
	strcpy_s(cardName, 128, videoCardDescription);
}

void OpenGL::CheckError() {
	GLenum error(glGetError());

	while (error != GL_NO_ERROR) {

		switch (error) {
		case GL_INVALID_OPERATION:				LogError(L"GL-INVALID_OPERATION");	break;
		case GL_INVALID_ENUM:					LogError(L"GL-INVALID_ENUM");		break;
		case GL_INVALID_VALUE:					LogError(L"GL-INVALID_VALUD");		break;
		case GL_OUT_OF_MEMORY:					LogError(L"GL-OUT_OF_MEMORY");		break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:	LogError(L"GL-INVALID_FRAMEBUFFER_OPERATION"); break;
		}
		error = glGetError();
	}
}

char* OpenGL::GetShaderLog(unsigned int shaderID) {
	int logSize = 0;
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logSize);

	//Increment the size by one to handle also the null terminator.
	logSize++;
	char* pInfoLog = new char[logSize];
	if (!pInfoLog)
		return nullptr;

	glGetShaderInfoLog(shaderID, logSize, NULL, pInfoLog);

	return pInfoLog;
}

char* OpenGL::GetLinkerLog(unsigned int programID) {
	int logSize = 0;
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logSize);

	//Increment the size by one to handle also the null terminator.
	logSize++;
	char* pInfoLog = new char[logSize];
	if (!pInfoLog)
		return nullptr;

	glGetProgramInfoLog(programID, logSize, NULL, pInfoLog);

	return pInfoLog;
}