#pragma once
#include "includes.h"
#include "Render.h"
#include "Physic_object.h"
class main
{
public:
	static HWND hwnd;
public:
	main(){};
	~main(){};
    void Initialize();
	void RunMessageLoop();
	static void recreate();
private:
	static LRESULT CALLBACK WndProc(
        HWND hWnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam
        );
};

HWND main::hwnd;

int GenRandNum(int MaxModule)
{
    int ret = rand()%MaxModule;
    ret *= int(pow(-1.0f,rand()%(MaxModule*10)));
    while(ret < -MaxModule || MaxModule < ret)
    {
        ret = rand()%MaxModule;                            //taken from the internet
        ret *= int(pow(-1.0f,rand()%(MaxModule*10)));
    }
    return ret+50;
}

