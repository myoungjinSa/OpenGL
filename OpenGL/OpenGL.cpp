#include "OpenGL.h"

OpenGL::OpenGL() {
	deviceContext = NULL;
	renderingContext = NULL;
}

OpenGL::OpenGL(const OpenGL& other) {

}

OpenGL::~OpenGL() {

}


bool OpenGL::InitializeExtensions(HWND hWnd) {
	HDC deviceContext;
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

bool OpenGL::InitializeOpenGL(HWND hWnd, int screenWidth, int screenHeight, float screenDepth, float screenNear, bool vSync) {
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

	//Support for OpenGL rendering
	attributeListInt[0] = WGL_SUPPORT_OPENGL_ARB;
	attributeListInt[1] = TRUE;

	//Support for rendering to a window
	attributeListInt[2] = WGL_DRAW_TO_WINDOW_ARB;
	attributeListInt[3] = TRUE;

	//Support for hardware acceleration
	attributeListInt[4] = WGL_ACCELERATION_ARB;
	attributeListInt[5] = WGL_FULL_ACCELERATION_ARB;

	//Support for 24bit color.
	attributeListInt[6] = WGL_COLOR_BITS_ARB;
	attributeListInt[7] = 24;

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

	//Set the polygon winding to front facing for the left handed system.
	glFrontFace(GL_CW);

	//Enable back face culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	//Initialize the world/model matrix to the identity matrix.
	BuildIdentityMatrix(worldMatrix);

	//Set the field of view and screen aspect ratio.
	fieldOfView = 3.14159265358979323846f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	//Build the perspective projection matrix.
	BuildPerspectiveFovLHMatrix(projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

	return true;
}

void OpenGL::GetWorldMatrix(Matrix<float, 4, 4>& matrix) {
	for (size_t iVal = 0; iVal < matrix.rows * matrix.cols; iVal++) {
		matrix[iVal] = worldMatrix[iVal];
	}
}

void OpenGL::GetProjectionMatrix(Matrix<float, 4, 4>& matrix) {
	for (size_t iVal = 0; iVal < matrix.rows * matrix.cols; iVal++) {
		matrix[iVal] = projectionMatrix[iVal];
	}
}

void OpenGL::GetWorldMatrix(float* matrix) {
	matrix[0] = worldMatrix[0];
	matrix[1] = worldMatrix[1];
	matrix[2] = worldMatrix[2];
	matrix[3] = worldMatrix[3];

	matrix[4] = worldMatrix[4];
	matrix[5] = worldMatrix[5];
	matrix[6] = worldMatrix[6];
	matrix[7] = worldMatrix[7];

	matrix[8] = worldMatrix[8];
	matrix[9] = worldMatrix[9];
	matrix[10] = worldMatrix[10];
	matrix[11] = worldMatrix[11];

	matrix[12] = worldMatrix[12];
	matrix[13] = worldMatrix[13];
	matrix[14] = worldMatrix[14];
	matrix[15] = worldMatrix[15];
}




void OpenGL::GetProjectionMatrix(float* matrix)
{
	matrix[0] = projectionMatrix[0];
	matrix[1] = projectionMatrix[1];
	matrix[2] = projectionMatrix[2];
	matrix[3] = projectionMatrix[3];

	matrix[4] = projectionMatrix[4];
	matrix[5] = projectionMatrix[5];
	matrix[6] = projectionMatrix[6];
	matrix[7] = projectionMatrix[7];

	matrix[8] = projectionMatrix[8];
	matrix[9] = projectionMatrix[9];
	matrix[10] = projectionMatrix[10];
	matrix[11] = projectionMatrix[11];

	matrix[12] = projectionMatrix[12];
	matrix[13] = projectionMatrix[13];
	matrix[14] = projectionMatrix[14];
	matrix[15] = projectionMatrix[15];
}

void OpenGL::GetVideoCardInfo(char* cardName)
{
	strcpy_s(cardName, 128, videoCardDescription);
}

void OpenGL::BuildIdentityMatrix(Matrix<float, 4, 4>& matrix)
{
	matrix[0] = 1.0f;
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 1.0f;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = 1.0f;
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;
}
void OpenGL::BuildPerspectiveFovLHMatrix(Matrix<float, 4, 4>& matrix, float fieldOfView, float screenAspect, float screenNear, float screenDepth)
{
	matrix[0] = 1.0f / (screenAspect * tan(fieldOfView * 0.5f));
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 1.0f / tan(fieldOfView * 0.5f);
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = screenDepth / (screenDepth - screenNear);
	matrix[11] = 1.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = (-screenNear * screenDepth) / (screenDepth - screenNear);
	matrix[15] = 0.0f;
}

void OpenGL::MatrixRotationY(Matrix<float, 4, 4>& matrix, float angle)
{
	matrix[0] = cosf(angle);
	matrix[1] = 0.0f;
	matrix[2] = -sinf(angle);
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 1.0f;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = sinf(angle);
	matrix[9] = 0.0f;
	matrix[10] = cosf(angle);
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;
}

void OpenGL::MatrixTranslation(Matrix<float, 4, 4>& matrix, float x, float y, float z)
{
	matrix[0] = 1.0f;
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 1.0f;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = 1.0f;
	matrix[11] = 0.0f;

	matrix[12] = x;
	matrix[13] = y;
	matrix[14] = z;
	matrix[15] = 1.0f;

}

void OpenGL::MatrixMultiply(Matrix<float, 4, 4>& result, const Matrix<float, 4, 4>& matrix1, const Matrix<float, 4, 4>& matrix2)
{
	result[0] = (matrix1[0] * matrix2[0]) + (matrix1[1] * matrix2[4]) + (matrix1[2] * matrix2[8]) + (matrix1[3] * matrix2[12]);
	result[1] = (matrix1[0] * matrix2[1]) + (matrix1[1] * matrix2[5]) + (matrix1[2] * matrix2[9]) + (matrix1[3] * matrix2[13]);
	result[2] = (matrix1[0] * matrix2[2]) + (matrix1[1] * matrix2[6]) + (matrix1[2] * matrix2[10]) + (matrix1[3] * matrix2[14]);
	result[3] = (matrix1[0] * matrix2[3]) + (matrix1[1] * matrix2[7]) + (matrix1[2] * matrix2[11]) + (matrix1[3] * matrix2[15]);

	result[4] = (matrix1[4] * matrix2[0]) + (matrix1[5] * matrix2[4]) + (matrix1[6] * matrix2[8]) + (matrix1[7] * matrix2[12]);
	result[5] = (matrix1[4] * matrix2[1]) + (matrix1[5] * matrix2[5]) + (matrix1[6] * matrix2[9]) + (matrix1[7] * matrix2[13]);
	result[6] = (matrix1[4] * matrix2[2]) + (matrix1[5] * matrix2[6]) + (matrix1[6] * matrix2[10]) + (matrix1[7] * matrix2[14]);
	result[7] = (matrix1[4] * matrix2[3]) + (matrix1[5] * matrix2[7]) + (matrix1[6] * matrix2[11]) + (matrix1[7] * matrix2[15]);

	result[8] = (matrix1[8] * matrix2[0]) + (matrix1[9] * matrix2[4]) + (matrix1[10] * matrix2[8]) + (matrix1[11] * matrix2[12]);
	result[9] = (matrix1[8] * matrix2[1]) + (matrix1[9] * matrix2[5]) + (matrix1[10] * matrix2[9]) + (matrix1[11] * matrix2[13]);
	result[10] = (matrix1[8] * matrix2[2]) + (matrix1[9] * matrix2[6]) + (matrix1[10] * matrix2[10]) + (matrix1[11] * matrix2[14]);
	result[11] = (matrix1[8] * matrix2[3]) + (matrix1[9] * matrix2[7]) + (matrix1[10] * matrix2[11]) + (matrix1[11] * matrix2[15]);

	result[12] = (matrix1[12] * matrix2[0]) + (matrix1[13] * matrix2[4]) + (matrix1[14] * matrix2[8]) + (matrix1[15] * matrix2[12]);
	result[13] = (matrix1[12] * matrix2[1]) + (matrix1[13] * matrix2[5]) + (matrix1[14] * matrix2[9]) + (matrix1[15] * matrix2[13]);
	result[14] = (matrix1[12] * matrix2[2]) + (matrix1[13] * matrix2[6]) + (matrix1[14] * matrix2[10]) + (matrix1[15] * matrix2[14]);
	result[15] = (matrix1[12] * matrix2[3]) + (matrix1[13] * matrix2[7]) + (matrix1[14] * matrix2[11]) + (matrix1[15] * matrix2[15]);
}

