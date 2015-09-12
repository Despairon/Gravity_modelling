#include "main.h"
FLOAT xx = 15.0f, yy = 0.0f, zz = 0.0f; // Position of the camera
FLOAT vx = 14.0f, vy = 0, vz = 0.0f; // look-at point
bool Fullscreen = false;
bool quit = false; 
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) 
{
	CoInitializeEx(0, COINIT_MULTITHREADED);
	main app;
	app.Initialize();	
	render->init(app.hwnd);
	render->init_geometry(app.hwnd);
	render->InitMatrixes(app.hwnd);	
	ShowCursor(0);
	SetTimer(app.hwnd, CAMERA_MOVE_TIMER, 10, (TIMERPROC) NULL);
	app.RunMessageLoop();
	render->~Render();
	CoUninitialize();
}
void main::Initialize()
{
//-------main---window------------------------//
	UINT xscreen = GetSystemMetrics(SM_CXSCREEN);
	UINT yscreen = GetSystemMetrics(SM_CYSCREEN);
	TCHAR szClassName[] = L"main_class"; 
    WNDCLASSEX wc; 
    wc.cbSize        = sizeof(wc); 
    wc.style         = CS_HREDRAW | CS_VREDRAW; 
    wc.lpfnWndProc   = main::WndProc; 
    wc.lpszMenuName  = NULL; 
    wc.lpszClassName = szClassName; 
    wc.cbWndExtra    = NULL; 
    wc.cbClsExtra    = NULL; 
    wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO); 
    wc.hIconSm       = LoadIcon(NULL, IDI_WINLOGO); 
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW); 
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
    wc.hInstance     = HINST_THISCOMPONENT; 
	RegisterClassEx(&wc);
	hwnd = CreateWindow(
    szClassName, 
    L"Gravity",
    WS_OVERLAPPEDWINDOW,
    (xscreen-xscreen/1.5f)-xscreen/10,
    (yscreen-yscreen/1.5f)-yscreen/10,
    xscreen/1.5f, 
    yscreen/1.5f, 
    NULL, 
    NULL,
    HINST_THISCOMPONENT, 
    NULL); 
    ShowWindow(hwnd, 1); 
    UpdateWindow(hwnd); 
}
void main::RunMessageLoop()
{
	MSG msg = {0};
	while( msg.message != WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
			render->SetViewMatrix(xx,yy,zz,vx,vy,vz);
			render->Draw();
        }
    }

}
LRESULT CALLBACK main::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;
	RECT rc,rec;
	GetClientRect(hWnd,&rc);
	FLOAT width = rc.right - rc.left;
	FLOAT height = rc.bottom - rc.top;
	static POINT pt;
	static FLOAT a, a2, da = XM_PIDIV2;
	static FLOAT x, y, z, dx, dy;
	static int accelerator = 1;
	static bool move_forward = false, move_backward = false, move_left = false, move_right = false;
	srand(time(NULL));
    switch( message )
    {
        case WM_PAINT:
                hdc = BeginPaint( hWnd, &ps );
                EndPaint( hWnd, &ps );
                break;
		case WM_KEYDOWN:
			switch(wParam)
			{
			    case 0x10:
					if (accelerator == 1)
						accelerator = 3;
					else
						accelerator = 1;
				    break;
			    case 0x31:
					if (!Gravity_center::g_c)
					    Gravity_center::g_c = new Gravity_center(pow(10,15));
					else
						MessageBox(hWnd, L"Gravity center has been created already!", L"wow!", MB_OK);
				    break;
				case 0x32:
					if (Gravity_center::g_c)
					{
						if (Gravity_center::gravity_on)
							MessageBox(hWnd, L"You can't create objects while gravity is on!", L"oops!", MB_OK);
						else
						{
							float x = GenRandNum(1000);
							float y = GenRandNum(1000);
							float z = GenRandNum(1000);
							new physic_object(x, y, z, 1000000);
						}
					}
					else MessageBox(hWnd, L"Create gravity center first!", L"oops!", MB_OK);
					break;
				case 0x33:
					if (Gravity_center::g_c)
					{
						Gravity_center::gravity_on = true;
					}
					else MessageBox(hWnd, L"Create gravity center first!", L"oops!", MB_OK);
					break;
			    case 0x0D:
					if (Fullscreen)
						Fullscreen = false;
					else
						Fullscreen = true;
					recreate();
					break;
				case 0x1B:
					quit = true;
					PostQuitMessage(0);
					break;
				default:
					break;
			}
		case WM_KEYUP:
			switch (wParam)
			{
			case 0x41:
				move_left = false;
				break;
			case 0x44:
				move_right = false;
				break;
			case 0x57:
				move_forward = false;
				break;
			case 0x53:
				move_backward = false;
				break;
			}
		case WM_SIZE:
			GetClientRect(hWnd, &rc);
			width = rc.right - rc.left;
			height = rc.bottom - rc.top;
			break;
		case WM_MOVE:
			GetClientRect(hWnd, &rc);
			width = rc.right - rc.left;
			height = rc.bottom - rc.top;
			break;
		case WM_MOUSEMOVE:
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam); 

			if (pt.x >= width - 2)
			{
				GetWindowRect(hWnd, &rec);
				ClientToScreen(hWnd, &pt);
				SetCursorPos(rec.left+11, pt.y);
				da *= -1.0f;
				ScreenToClient(hWnd, &pt);
			}
			if (pt.x <= 2)
			{
				GetWindowRect(hWnd, &rec);
				ClientToScreen(hWnd, &pt);
				SetCursorPos(rec.right-11, pt.y);
				da *= -1.0f;
				ScreenToClient(hWnd, &pt);
			}
			a = -XM_PI*(pt.x / width)+da;
			a2 = -XM_PI*(pt.y / height)+XM_PIDIV2;

		    vx = xx + cos(a)*cos(a2);
			vy = yy + sin(a2);
			vz = zz + cos(a2)*sin(a);

			dx = pt.x;
			dy = pt.y;
			break;
		case WM_TIMER:
	    	case CAMERA_MOVE_TIMER:
				if (GetAsyncKeyState(0x41) & 0x8000)
					move_left = true;
				if (GetAsyncKeyState(0x44) & 0x8000)
					move_right = true;
				if (GetAsyncKeyState(0x57) & 0x8000)
					move_forward = true;
				if (GetAsyncKeyState(0x53) & 0x8000)
					move_backward = true;
				if (move_forward)
				{
					x = xx;
					z = zz;
					y = yy;
					xx += (vx - xx)*accelerator;
					yy += (vy - yy)*accelerator;
					zz += (vz - zz)*accelerator;
					vx += (vx - x)*accelerator;
					vy += (vy - y)*accelerator;
					vz += (vz - z)*accelerator;
				}
				if (move_backward)
				{
					x = xx;
					z = zz;
					y = yy;
					xx -= (vx - xx)*accelerator;
					yy -= (vy - yy)*accelerator;
					zz -= (vz - zz)*accelerator;
					vx -= (vx - x)*accelerator;
					vy -= (vy - y)*accelerator;
					vz -= (vz - z)*accelerator;
				}
				if (move_left)
				{
					xx = (xx - cos(a - XM_PIDIV2));
					zz = (zz - sin(a - XM_PIDIV2));
					vx = (vx - cos(a - XM_PIDIV2));
					vz = (vz - sin(a - XM_PIDIV2));
				}
				if (move_right)
				{
					xx = (xx + cos(a - XM_PIDIV2));
					zz = (zz + sin(a - XM_PIDIV2));
					vx = (vx + cos(a - XM_PIDIV2));
					vz = (vz + sin(a - XM_PIDIV2));
				}
				break;
			break;
        case WM_DESTROY:
			if (quit)
			{
				KillTimer(hwnd, CAMERA_MOVE_TIMER);
				PostQuitMessage(0);
			}
            break;
		case WM_CLOSE:
			quit = true;
			PostQuitMessage( 0 );
			break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}

void main::recreate()
{
	UINT xscreen = GetSystemMetrics(SM_CXSCREEN);
	UINT yscreen = GetSystemMetrics(SM_CYSCREEN);
	if (Fullscreen)
	{
		SetWindowLong(hwnd,-16,WS_POPUP);
		SetWindowPos(hwnd,HWND_TOP,0,0,xscreen,yscreen, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		render->reinit(hwnd, Fullscreen);
	}
	else
	{
		SetWindowLong(hwnd,-16,WS_OVERLAPPEDWINDOW);
		SetWindowPos(hwnd,
			         HWND_TOP, 
			         (xscreen - xscreen / 1.5f) - xscreen / 10, 
			         (yscreen - yscreen / 1.5f) - yscreen / 10, 
			          xscreen / 1.5f, 
			          yscreen / 1.5f, 
			          SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		render->reinit(hwnd, Fullscreen);
	}
}

