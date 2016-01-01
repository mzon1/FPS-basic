//////////////////////////////////////////////////////////////////////////
//
// main.cpp
// 
// 2010. 3. 12.
// Created by Sun-Jeong Kim
// 
// Description
// - create a window
// - use OpenGL library
// - be useful for animations
//
//////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "glut.h"
#include "geometry.h"
#include <math.h>

#include "GLAUX.H"

#pragma comment(lib, "glaux.lib")

#define PI 3.141592654f
#define RADIAN 3.14159/180
#define TARGET_NUM 9
#define BULLET_NUM 20 //연속 발사 총알 갯수
#define SHOT_DISTANCE 200

// global variables
char *szClassName = "Computer Graphics";

HWND MyMWindowHandle = 0;
GLuint texID;

HDC MyDC;
HGLRC MyRC;

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

typedef struct Camera
{
	float x;
	float y;
	float z;
	float xrot;
	float yrot;
	float zrot;
	float angle;
} Camera;

typedef struct point
{
	float x;
	float y;
} point;

typedef struct targetObject
{
	BOOL hited;
	float x;
	float y;
	float z;
	float radius;
	float angle;
} targetObject;

typedef struct bullet
{
	BOOL fired;
	float fromX;
	float fromY;
	float fromZ;
	float x;
	float y;
	float z;
	float xrot;
	float yrot;
	float zrot;
} bullet;

Camera cam;
point point;
bullet bullets[BULLET_NUM];//총알 
targetObject targets[TARGET_NUM];//타겟

bool bSetupPixelFormat( void );
void Resize( const int cx, const int cy );
void InitScene( void );
void InitObjects( void );
void FinishScene( void );
void UpdateScene( void );
void DrawScene( void );
void DrawObjects( void );
void DrawGround( void );
void DrawShadow( void );
void LoaderTexture( void);
void target();
void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y);
void camera(void);
void mouseMovement(int x, int y);
void fireBullet();
void interfaceview( void );
float distance(float x1, float y1, float z1, float x2, float y2, float z2);
bool doSpheresIntersect( bullet *target1, targetObject *target2 );
void SetLighting( void);


float lightPos[3] = { -10.0f, 10.0f, 10.0f };
float yR = 1.25f;
//float pi = PI;
float xi, zj;
float k = 0.0f;
float z = 0.0f;

GLfloat MyVertices[8][3] = {{-1000,-2000,-1000}, {1000,-2000,-1000}, {1000,2000,-1000}, {-1000,2000,-1000}, 
							{-1000,-2000,1000}, {1000,-2000,1000}, {1000,2000,1000}, {-1000,2000,1000}};
/*GLfloat MyVertices[8][3] = { 
	{ -1.0f, -1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f },
	{  1.0f,  1.0f, -1.0f }, { -1.0f,  1.0f, -1.0f }, 
	{ -1.0f, -1.0f,  1.0f }, {  1.0f, -1.0f,  1.0f }, 
	{  1.0f,  1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f } }; 	
	*/
					
float cube_normal[8][3] = {
	{ -0.57735f, -0.57735f, -0.57735f }, {  0.57735f, -0.57735f, -0.57735f }, 
	{  0.57735f,  0.57735f, -0.57735f }, { -0.57735f,  0.57735f, -0.57735f }, 
	{ -0.57735f, -0.57735f,  0.57735f }, {  0.57735f, -0.57735f,  0.57735f }, 
	{  0.57735f,  0.57735f,  0.57735f }, { -0.57735f,  0.57735f,  0.57735f } };
	
GLubyte cube_index[] = { 0, 3, 2, 1, 1, 2, 6, 5, 2, 3, 7, 6,
			 3, 0, 4, 7, 4, 5, 6, 7, 5, 4, 0, 1 };
float cube_texture[8][2] = {
	{0,  1}, {1,  1},
	{1,  0}, {0,  0},
	{1,  1}, {0,  1},
	{0,  0}, {1,  0}};
	

GLUquadricObj *pObj = NULL;

static int MouseX,MouseY,MouseX_pivot,MouseY_pivot; //마우스 위치

#include <sys/timeb.h>
timeb startTime, endTime;

float lastx, lasty;

/*
 *	WinMain
 */
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int nShowCmd )
{
 	// Registers the window class
	WNDCLASS wc;
	
	wc.style	 = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	 = (WNDPROC)WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon	 = LoadIcon( hInstance, IDI_APPLICATION );
	wc.hCursor	 = LoadCursor( 0, IDC_ARROW );
	wc.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
	wc.lpszMenuName	 = NULL;
	wc.lpszClassName = szClassName;

	RegisterClass(&wc);

	// Create the main window
	MyMWindowHandle = CreateWindow( szClassName, 
					"Simple OpenGL Example", 
					WS_OVERLAPPEDWINDOW, 
					CW_USEDEFAULT, 
					CW_USEDEFAULT, 
					CW_USEDEFAULT, 
					CW_USEDEFAULT, 
					0, 
					0, 
					hInstance, 
					0 );

	// Make sure that window was created 
	if( !MyMWindowHandle )
		return false;
	
	ShowWindow( MyMWindowHandle, nShowCmd );
	UpdateWindow( MyMWindowHandle );

	// Main message loop
	MSG msg;
	msg.message = WM_NULL;
	while( msg.message != WM_QUIT )
	{
		// If there are Window messages then process them.
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		else
		{	
			UpdateScene();
			DrawScene();
		}
	}

	UnregisterClass( szClassName, wc.hInstance );
	
	return (int)msg.wParam;
}


/*
 * WndProc: to process messages for the main window
 */
LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	RECT rect;


	switch( msg ) 
	{
	case WM_CREATE: 
		// Initialize for the OpenGL rendering
		MyDC = GetDC( hWnd ); 
		if( !bSetupPixelFormat() ) 
			DestroyWindow( MyMWindowHandle ); 

		MyRC = wglCreateContext( MyDC ); 
		wglMakeCurrent( MyDC, MyRC ); 

		InitObjects();
		InitScene();

		break; 
	
	case WM_SIZE: 
		GetClientRect( hWnd, &rect ); 
		Resize( rect.right, rect.bottom );
		
		break; 

	case WM_DESTROY:
		// Destroy all about OpenGL
		if( MyRC ) 
			wglDeleteContext( MyRC ); 
		
		if( MyDC ) 
			ReleaseDC( hWnd, MyDC ); 
		
		FinishScene();

		PostQuitMessage( 0 );
		break;
	
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE ) {
			if( IDYES == MessageBox( NULL, "Are you sure to exit?", "Exit", MB_YESNO ) )
				DestroyWindow( MyMWindowHandle );
		}
		break;

	case WM_LBUTTONUP:
		fireBullet();
		break;

	case WM_MOUSEMOVE:
		point.x=LOWORD(lParam) * 0.8f;
		point.y=HIWORD(lParam) * 0.8f;
		break;
	
	default:
		return DefWindowProc( hWnd, msg, wParam, lParam );
	}

	return 0;
}

/*
 * bSetupPixelFormat : to setup the format of pixel for OpenGL
 */
bool bSetupPixelFormat( void ) 
{ 
    PIXELFORMATDESCRIPTOR pfd; 
	int pixelformat; 

	pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR ); 
	pfd.nVersion = 1; 
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
	pfd.dwLayerMask = PFD_MAIN_PLANE; 
	pfd.iPixelType = PFD_TYPE_RGBA; 
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16; 
	pfd.cAccumBits = 0; 
	pfd.cStencilBits = 0;

	if( (pixelformat = ChoosePixelFormat( MyDC, &pfd )) == 0 ) { 
		MessageBox( NULL, "ChoosePixelFormat() failed!!!", "Error", MB_OK|MB_ICONERROR ); 
		return false; 
	} 

	if( SetPixelFormat( MyDC, pixelformat, &pfd ) == false ) { 
		MessageBox( NULL, "SetPixelFormat() failed!!!", "Error", MB_OK|MB_ICONERROR ); 
		return false; 
	}
	
	return true; 
}

/*
 * Resize : to re-initialize a scene
 */
void Resize( const int cx, const int cy ) 
{ 
	glViewport( 0, 0, cx, cy );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	gluPerspective( 60.0, (double)cx/cy, 0.001, 30000);

	glMatrixMode( GL_MODELVIEW );

	return;
}

void InitObjects( void ) {
	int i;
	cam.x = 0;
	cam.y = 0;
	cam.z = 0;
	cam.xrot = 0;
	cam.yrot = 0;
	cam.zrot = 0;
	
	for(i=0;i<BULLET_NUM;i++)
	{
		bullets[i].fired = FALSE;
	}

	// init target
	for(i=0;i<TARGET_NUM;i++)
	{
		targets[i].hited = FALSE;
		targets[i].radius = 3;
		targets[i].angle = 0;
		if(i%3 == 0)
		{
			targets[i].x = -50;
		}
		else if(i%3 == 1)
		{
			targets[i].x = 0;
		}
		else
		{
			targets[i].x = 50;
		}
		if(i < 3)
		{
			targets[i].z = 50;
		}
		else if(i < 6)
		{
			targets[i].z = 100;
		}
		else
		{
			targets[i].z = 150;
		}
		targets[i].y = 6;
	}
}

/*
 * InitScene : to initialize OpenGL rendering parameters
 */
void InitScene( void )
{
	glEnable( GL_DEPTH_TEST );
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

	ftime( &startTime );
						

	glPolygonOffset( 1.0f, 1.0f);
	glEnable( GL_POLYGON_OFFSET_FILL);
	glLineWidth(2);

	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//	SetLighting();
	glEnable(GL_NORMALIZE);

	glEnable( GL_TEXTURE_2D);
	LoaderTexture();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT,0, MyVertices);
	glNormalPointer(GL_FLOAT, 0, cube_normal);
	glTexCoordPointer(2, GL_FLOAT, 0, cube_texture);

	return;
}
/*
 * FinishScene : to free memory
 */
void FinishScene( void )
{
	gluDeleteQuadric(pObj);
	
	return;
}

/*
 * UpdateScene: to update a scene
 */
void UpdateScene( void )
{
	ftime( &endTime );

	double elapsedTime = (double)(endTime.time-startTime.time) 
						+ (double)(endTime.millitm-startTime.millitm)/1000.0;
	
	startTime = endTime;

	mouseMovement(point.x, point.y);

    if (GetAsyncKeyState( 'W' ) && 0x8000)
    {
		float xrotrad, yrotrad;
		yrotrad = (cam.yrot / 180 * PI);
		xrotrad = (cam.xrot / 180 * PI); 
		cam.x += float(sin(yrotrad));
		cam.z -= float(cos(yrotrad));
    }

    if (GetAsyncKeyState( 'S' ) && 0x8000)
    {
		float xrotrad, yrotrad;
		yrotrad = (cam.yrot / 180 * PI);
		xrotrad = (cam.xrot / 180 * PI); 
		cam.x -= float(sin(yrotrad));
		cam.z += float(cos(yrotrad));
    }

    if (GetAsyncKeyState( 'D' ) && 0x8000)
    {
		float yrotrad;
		yrotrad = (cam.yrot / 180 * PI);
		cam.x += float(cos(yrotrad));
		cam.z += float(sin(yrotrad));
    }

    if (GetAsyncKeyState( 'A' ) && 0x8000)
    {
		float yrotrad;
		yrotrad = (cam.yrot / 180 * PI);
		cam.x -= float(cos(yrotrad));
		cam.z -= float(sin(yrotrad));
    }

	return;
}

void fireBullet()
{
	int i;
	for(i=0;i<BULLET_NUM;i++)
	{
		if(!bullets[i].fired)
		{
			bullets[i].fired = true;
			bullets[i].x = cam.x / 2.5 ;
			bullets[i].y = 4;
			bullets[i].z = cam.z / 2.5;
			bullets[i].fromX = bullets[i].x;
			bullets[i].fromY = bullets[i].y;
			bullets[i].fromZ = bullets[i].z;
			bullets[i].xrot = cam.xrot;
			bullets[i].yrot = cam.yrot;
			bullets[i].zrot = cam.zrot;
			break;
		}
	}
}

float distance(float x1, float y1, float z1, float x2, float y2, float z2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));
}

bool doSpheresIntersect( bullet *target1, targetObject *target2 )
{
    float fDistance =0.5 + target2->radius;
	float fRadius;

    fRadius = sqrt( ((target2->x - target1->x) * (target2->x - target1->x)) +
                    ((target2->y - target1->y) * (target2->y - target1->y)) +
                    ((target2->z - target1->z) * (target2->z - target1->z)) );

    if( fRadius < fDistance )
        return true;

    else
        return false;
}

/*
 * DrawScene : to draw a scene
 */
void DrawScene( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();
	interfaceview();

	camera();
	//glEnable( GL_DEPTH_TEST );
	DrawGround();

	glPushMatrix();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glLineWidth(3);
		glColor3f(0,0,0);

		int i, j;
		for(i=0;i<BULLET_NUM;i++)
		{
			if(bullets[i].fired)
			{
				glPushMatrix();
				float yrotrad = (bullets[i].yrot / 180 * PI);
				float xrotrad = (bullets[i].xrot / 180 * PI);
				glTranslatef(bullets[i].x += sinf(yrotrad), bullets[i].y -= sinf(xrotrad), bullets[i].z -= cosf(yrotrad));

				if(bullets[i].y <= 0 || distance(bullets[i].x, bullets[i].y, bullets[i].z, bullets[i].fromX, bullets[i].fromY, bullets[i].fromZ) > SHOT_DISTANCE)
					bullets[i].fired = false;
				glColor3f(0,0,0);
				glutWireSphere(0.5,4,4);
				glColor3f(1,0,1);
				glutSolidSphere(0.5,4,4);

				glPopMatrix();
			}

			for(j=0;j<TARGET_NUM;j++)
			{
				if(!targets[j].hited && bullets[i].fired && doSpheresIntersect(&bullets[i], &targets[j]))
				{
					bullets[i].fired = false;
					targets[j].hited = true;
				}
			}
		}

		for(i=0;i<TARGET_NUM;i++)
		{
			glPushMatrix();
			glTranslatef(targets[i].x, 0, targets[i].z);
			if(targets[i].hited)
			{
				if(targets[i].angle < 90)
					targets[i].angle += 0.8f;
				else
					targets[i].hited = false;
			} else if(!targets[i].hited && targets[i].angle > 0)
				targets[i].angle -= 0.8f;
			
			glRotatef(targets[i].angle, 1, 0, 0);
			target();
			glPopMatrix();
		}
	glPopMatrix();

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, cube_index);
	SwapBuffers(MyDC);

	return;
}

void DrawGround( void )
{
	glPushMatrix();

	glTranslatef( 0.0f, -0.10f, 0.0f );
	glScalef( 20.0f, 0.0f, 20.0f);

	glColor3f( 0.9f, 0.9f, 0.9f);
	glBegin( GL_POLYGON );
		glVertex3f( 50.0f, 0.0f, 50.0f);
		glVertex3f( 50.0f, 0.0f, -50.0f);
		glVertex3f( -50.0f, 0.0f, -50.0f);
		glVertex3f( -50.0f, 0.0f, 50.0f);
	glEnd();

	glColor3f( 0.0f, 0.0f, 0.0f);
	glBegin( GL_LINES );
		for( float x=-50.0f; x<=50.0f; x+=1.0f)
		{
			glVertex3f( x, 0.0f, -50.0f);
			glVertex3f( x, 0.0f, 50.0f);
		}
		for( float z=-50.0f; z<=50.0f; z+=1.0f)
		{
			glVertex3f( -50.0f, 0.0f, z);
			glVertex3f( 50.0f, 0.0f, z);
		}
	glEnd();

	glPopMatrix();

	return;
}

void DrawShadow( void )
{
	float proj[16];
	for( int i = 0; i <16; i++)proj[i] = 0.0f;
	proj[0] = proj[5] = proj[10] = 1.0f;
	proj[7] = -1.0f / (lightPos[1] + 0.999f);

	glPushMatrix();

	glColor3f( 0.5f, 0.5f, 0.5f );

	glTranslatef( lightPos[0], lightPos[1], lightPos[2] );
	glMultMatrixf( proj );
	glTranslatef( -lightPos[0], -lightPos[1], -lightPos[2] );


	glPushMatrix();
	glTranslatef( 0.0f, -1.0f, 0.0f);
	glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
	glutSolidCone( 0.5, 1.0, 24, 3 );
	glPopMatrix();


	glPushMatrix();
	glTranslatef( 0.0f, 0.5f, 0.0f);
	glScalef( 0.5f, 0.5f, 0.5f);
	glutSolidIcosahedron();
	glPopMatrix();

	glPopMatrix();

	return;
}

void target()
{
	glPushMatrix();
		glTranslatef(0, 0, -1);

		glColor3f(0.0,0.0,0.0);
		glBegin(GL_LINE_LOOP);
			glVertex3f(0,0,0);
			glVertex3f(0,4,0);
			glVertex3f(2,4,0);
			glVertex3f(2,0,0);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(2,0,0);
			glVertex3f(2,4,0);
			glVertex3f(2,4,1);
			glVertex3f(2,0,1);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(0,0,1);
			glVertex3f(0,4,1);
			glVertex3f(2,4,1);
			glVertex3f(2,0,1);
		glEnd();
		glEnd();
			glBegin(GL_LINE_LOOP);
			glVertex3f(0,0,1);
			glVertex3f(0,4,1);
			glVertex3f(0,4,0);
			glVertex3f(0,0,0);
		glEnd();
		//
		glBegin(GL_LINE_LOOP);
			glVertex3f(-2,4,0);
			glVertex3f(-2,8,0);
			glVertex3f(4,8,0);
			glVertex3f(4,4,0);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(4,4,0);
			glVertex3f(4,8,0);
			glVertex3f(4,8,1);
			glVertex3f(4,4,1);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(4,4,1);
			glVertex3f(4,8,1);
			glVertex3f(-2,8,1);
			glVertex3f(-2,4,1);
		glEnd();
			glBegin(GL_LINE_LOOP);
			glVertex3f(-2,4,1);
			glVertex3f(-2,8,1);
			glVertex3f(-2,8,0);
			glVertex3f(-2,4,0);
		glEnd();
			glBegin(GL_LINE_LOOP);
			glVertex3f(-2,4,0);
			glVertex3f(4,4,0);
			glVertex3f(4,4,1);
			glVertex3f(-2,4,1);
		glEnd();
			glBegin(GL_LINE_LOOP);
			glVertex3f(-2,8,1);
			glVertex3f(4,8,1);
			glVertex3f(4,8,1);
			glVertex3f(-2,8,1);
		glEnd();
		//
		glBegin(GL_LINE_LOOP);
			glVertex3f(-2,8,0);
			glVertex3f(4,8,0);
			glVertex3f(3,9,0);
			glVertex3f(-1,9,0);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(4,8,0);
			glVertex3f(4,8,1);
			glVertex3f(3,9,1);
			glVertex3f(3,9,0);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(4,8,1);
			glVertex3f(3,9,1);
			glVertex3f(-1,9,1);
			glVertex3f(-2,8,1);
		glEnd();
		glEnd();
			glBegin(GL_LINE_LOOP);
			glVertex3f(-2,8,1);
			glVertex3f(-1,9,1);
			glVertex3f(-1,9,0);
			glVertex3f(-2,8,0);
		glEnd();
		//머리


		glBegin(GL_LINE_LOOP);
			glVertex3f(2.5,9,0);
			glVertex3f(2.5,9,1);
			glVertex3f(2.0,10,1);
			glVertex3f(2,10,0);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(2.0,10,0);
			glVertex3f(2,10,1);
			glVertex3f(1.5,10.5,1);
			glVertex3f(1.5,10.5,0);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(1.5,10.5,0);
			glVertex3f(1.5,10.5,1);
			glVertex3f(0.5,10.5,1);
			glVertex3f(0.5,10.5,0);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(0.5,10.5,0);
			glVertex3f(0.5,10.5,1);
			glVertex3f(0,10,1);
			glVertex3f(0,10,0);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(0.0,10.0,0);
			glVertex3f(0.0,10.0,1);
			glVertex3f(-0.5,9,1);
			glVertex3f(-0.5,9,0);
		glEnd();


		glColor3f(0.8,0.8,0.8);
		glBegin(GL_QUADS);
			glVertex3f(0,0,0);
			glVertex3f(0,4,0);
			glVertex3f(2,4,0);
			glVertex3f(2,0,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(2,0,0);
			glVertex3f(2,4,0);
			glVertex3f(2,4,1);
			glVertex3f(2,0,1);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(0,0,1);
			glVertex3f(0,4,1);
			glVertex3f(2,4,1);
			glVertex3f(2,0,1);
		glEnd();
		glEnd();
			glBegin(GL_QUADS);
			glVertex3f(0,0,1);
			glVertex3f(0,4,1);
			glVertex3f(0,4,0);
			glVertex3f(0,0,0);
		glEnd();
		//
		glBegin(GL_QUADS);
			glVertex3f(-2,4,0);
			glVertex3f(-2,8,0);
			glVertex3f(4,8,0);
			glVertex3f(4,4,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(4,4,0);
			glVertex3f(4,8,0);
			glVertex3f(4,8,1);
			glVertex3f(4,4,1);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(4,4,1);
			glVertex3f(4,8,1);
			glVertex3f(-2,8,1);
			glVertex3f(-2,4,1);
		glEnd();
			glBegin(GL_QUADS);
			glVertex3f(-2,4,1);
			glVertex3f(-2,8,1);
			glVertex3f(-2,8,0);
			glVertex3f(-2,4,0);
		glEnd();
			glBegin(GL_QUADS);
			glVertex3f(-2,4,0);
			glVertex3f(4,4,0);
			glVertex3f(4,4,1);
			glVertex3f(-2,4,1);
		glEnd();
			glBegin(GL_QUADS);
			glVertex3f(-2,8,1);
			glVertex3f(4,8,1);
			glVertex3f(4,8,1);
			glVertex3f(-2,8,1);
		glEnd();
		//
		glBegin(GL_QUADS);
			glVertex3f(-2,8,0);
			glVertex3f(4,8,0);
			glVertex3f(3,9,0);
			glVertex3f(-1,9,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(4,8,0);
			glVertex3f(4,8,1);
			glVertex3f(3,9,1);
			glVertex3f(3,9,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(4,8,1);
			glVertex3f(3,9,1);
			glVertex3f(-1,9,1);
			glVertex3f(-2,8,1);
		glEnd();
		glEnd();
			glBegin(GL_QUADS);
			glVertex3f(-2,8,1);
			glVertex3f(-1,9,1);
			glVertex3f(-1,9,0);
			glVertex3f(-2,8,0);
		glEnd();
		//머리
		glBegin(GL_QUADS);
			glVertex3f(-0.5,9,0);
			glVertex3f(0.5,9,0);
			glVertex3f(0.5,10.5,0);
			glVertex3f(0,10,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(0.5,9,0);
			glVertex3f(1.5,9,0);
			glVertex3f(1.5,10.5,0);
			glVertex3f(0.5,10.5,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(1.5,9,0);
			glVertex3f(2.5,9,0);
			glVertex3f(2.0,10,0);
			glVertex3f(1.5,10.5,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(-0.5,9,1);
			glVertex3f(0.5,9,1);
			glVertex3f(0.5,10.5,1);
			glVertex3f(0,10,1);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(0.5,9,1);
			glVertex3f(1.5,9,1);
			glVertex3f(1.5,10.5,1);
			glVertex3f(0.5,10.5,1);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(1.5,9,1);
			glVertex3f(2.5,9,1);
			glVertex3f(2.0,10,1);
			glVertex3f(1.5,10.5,1);
		glEnd();

		glBegin(GL_QUADS);
			glVertex3f(2.5,9,0);
			glVertex3f(2.5,9,1);
			glVertex3f(2.0,10,1);
			glVertex3f(2,10,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(2.0,10,0);
			glVertex3f(2,10,1);
			glVertex3f(1.5,10.5,1);
			glVertex3f(1.5,10.5,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(1.5,10.5,0);
			glVertex3f(1.5,10.5,1);
			glVertex3f(0.5,10.5,1);
			glVertex3f(0.5,10.5,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(0.5,10.5,0);
			glVertex3f(0.5,10.5,1);
			glVertex3f(0,10,1);
			glVertex3f(0,10,0);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f(0.0,10.0,0);
			glVertex3f(0.0,10.0,1);
			glVertex3f(-0.5,9,1);
			glVertex3f(-0.5,9,0);
		glEnd();
		glPopMatrix();
}

void interfaceview( void )	//사용자 인터페이스
{

	//glDisable(GL_DEPTH_TEST);
	glColor3f(0,0.7,0.2);
	glPushMatrix();
	glTranslatef(0, 0, -5);
	//조준점
	glBegin(GL_LINES);
		glVertex3f(0,0.7,0);
		glVertex3f(0,0.3,0);
		glVertex3f(0.7,0,0);
		glVertex3f(0.3,0,0);
		glVertex3f(0,-0.3,0);
		glVertex3f(0,-0.7,0);
		glVertex3f(-0.3,0,0);
		glVertex3f(-0.7,0,0);
	glEnd();

	//총
	glRotatef(180, 0,0,1);
	glRotatef(270, 1, 0, 0);
	glTranslatef(-10,-7,1);
	glColor3f(0.2,0.3,0.1);

	glBegin(GL_QUADS);
		glVertex3f(0,0,0);
		glVertex3f(6,0,0);
		glVertex3f(4,15,2);
		glVertex3f(2,15,2);
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(6,0,0);
		glVertex3f(6,0,6);
		glVertex3f(4,15,4);
		glVertex3f(4,15,2);
	glEnd();

	glColor3f(0,0,0);
	glBegin(GL_LINE_LOOP);
		glVertex3f(0,0,0);
		glVertex3f(6,0,0);
		glVertex3f(4,15,2);
		glVertex3f(2,15,2);
	glEnd();
	glBegin(GL_LINE_LOOP);
		glVertex3f(6,0,0);
		glVertex3f(6,0,6);
		glVertex3f(4,15,4);
		glVertex3f(4,15,2);
	glEnd();
	glPushMatrix();
	glTranslatef(3,18,3);
		glScalef(0.5,5,0.5);
	glColor3f(0.1,0.1,0.1);
	glutSolidSphere(1,5,5);
	glColor3f(0,0,0);
	glutWireSphere(1,5,5);
	glPopMatrix();

	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

}

void LoaderTexture(void)
{
	AUX_RGBImageRec* texRect = auxDIBImageLoad("sky.bmp");
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D,texID);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, texRect->sizeX, texRect->sizeY,
			0, GL_RGB, GL_UNSIGNED_BYTE, texRect->data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


		delete [] texRect->data;
		delete [] texRect;
}

void camera(void)
{	
	glRotatef(cam.xrot,1.0,0.0,0.0);  //rotate our camera on teh x-axis (left and right)
    glRotatef(cam.yrot,0.0,1.0,0.0);  //rotate our camera on the y-axis (up and down)
    glTranslated(-cam.x * 0.4,-5,-cam.z * 0.4); //translate the screento the position of our camera
}

void mouseMovement(int x, int y) {
    int diffx=x-lastx; //check the difference between the current x and the last x position
    int diffy=y-lasty; //check the difference between the current y and the last y position
    lastx=x; //set lastx to the current x position
    lasty=y; //set lasty to the current y position
    cam.xrot += (float) diffy; //set the xrot to xrot with the additionof the difference in the y position
    cam.yrot += (float) diffx;    //set the xrot to yrot with the additionof the difference in the x position
}

void SetLighting(){

	// Color Material 사용
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	float mat_specular[4] = {100.0f, 100.0f, 100.0f, 1.0f};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);
}