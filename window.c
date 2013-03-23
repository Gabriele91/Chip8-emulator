/*******************
** Gabriele Di Bari
** 2011/06/15
********************/
#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

HDC			hDC=NULL;		// DC openGL Handler
HGLRC		hRC=NULL;
HWND		hWnd=NULL;		// Window Handler
MSG		    msg;
HINSTANCE	hInstance;		// Instance App. Handler

unsigned char char_key[256];
unsigned char closeWindow;
unsigned int windowW;
unsigned int windowH;

// CALLBACK INPUT
LRESULT CALLBACK WndProc(	HWND	hWnd,
							UINT	uMsg,
							WPARAM	wParam,
							LPARAM	lParam)
{
	switch (uMsg)
	{

		case WM_CLOSE:
		{
			closeWindow=1;
			return 0;
		}

		case WM_KEYDOWN:
		{
			char_key[wParam] = 1;
			return 0;
		}

		case WM_KEYUP:
		{
			char_key[wParam] = 0;
			return 0;
		}

		case WM_SIZE:
		{
			windowW=LOWORD(lParam);
			windowH=HIWORD(lParam);
			glViewport(0,0,windowW,windowH);
			return 0;
		}
	}

	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

//open window
void StartWindow(int sizex,int sizey){
    //save window size
    int windowW=sizex;
    int windowH=sizey;
    //initialize keys
    int i;
    for(i=0;i<256;i++)char_key[i];
    closeWindow=0;
    // // // // // // // // // // // //
    // // // // // // // // // // // //
    // // // // // // // // // // // //
    WNDCLASS	        wc;
    hInstance			= GetModuleHandle(NULL);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= "C8_Gabriele_di_bari";
    RegisterClass(&wc);

    // // // // // // // // // // // //
    // // // // // // // // // // // //
    // // // // // // // // // // // //

	RECT		WindowRect;
	WindowRect.left=(long)0;
	WindowRect.right=(long)sizex;
	WindowRect.top=(long)0;
	WindowRect.bottom=(long)sizey;

    AdjustWindowRectEx(&WindowRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);

        hWnd=CreateWindowEx(	WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
								"C8_Gabriele_di_bari",
								"EMULETOR CHIP 8, Gabriele Di Bari",
								WS_OVERLAPPEDWINDOW |
								WS_CLIPSIBLINGS |
								WS_CLIPCHILDREN,
								0, 0,
								WindowRect.right-WindowRect.left,
								WindowRect.bottom-WindowRect.top,
								NULL,
								NULL,
								hInstance,
								NULL  );

	static	PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		16,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	hDC=GetDC(hWnd);
	GLuint PixelFormat=ChoosePixelFormat(hDC,&pfd);
    SetPixelFormat(hDC,PixelFormat,&pfd);
    hRC=wglCreateContext(hDC);
    wglMakeCurrent(hDC,hRC);


    ShowWindow(hWnd,SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

    /////////////////////////////
    // OPENGL
    /////////////////////////////
	glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    glViewport(0,   0,sizex,sizey);
    glOrtho(0, sizex, sizey, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    /////////////////////////////
    /////////////////////////////
    glEnable(GL_CULL_FACE);
    /////////////////////////////
    /////////////////////////////

}
//get window Width
int GetWW(){ return windowW; }
//get window Height 
int GetWH(){ return windowH; }
//updata window
void UpDataWindow(){
                PeekMessage(&msg,NULL,0,0,PM_REMOVE);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
}
//draw on window
void FlipScreen(){
                SwapBuffers(hDC);
}
//close window
void CloseTheWindow(){

    wglMakeCurrent(NULL,NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd,hDC);
    DestroyWindow(hWnd);
    UnregisterClass("C8_Gabriele_di_bari",NULL);
}
