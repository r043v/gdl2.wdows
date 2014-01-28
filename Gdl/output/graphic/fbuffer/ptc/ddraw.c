//
// TinyPTC by Gaffer
// www.gaffer.org/tinyptc
//

// it's my personal edition, it's not an official release by gaffer !

#include "../../../../config.h"
#ifdef usePtc
#include "tinyptc.h"
#ifdef __PTC_DDRAW__

#include "convert.h"
#define WIN32_LEAN_AND_MEAN
#include "ddraw.h"

#define u32 unsigned int

PTC_CONVERTER convert;
static HMODULE library = 0;
static LPDIRECTDRAW lpDD = 0;
static LPDIRECTDRAWSURFACE lpDDS = 0;
static LPDIRECTDRAWSURFACE lpDDS_back;
static WNDCLASS wc;
static int active;
static int dx;
static int dy;

void (*_onAfterZoom) (void*bf,u32 w,u32 h,u32 pitch)=0;

void ptc_setAfterZoomCallBack( void (*onAfterZoom)	(void*bf,u32 w,u32 h,u32 pitch) )
{ if(onAfterZoom) _onAfterZoom = onAfterZoom;
}

/*unsigned int letMeUseSomePower = 0;

void setAppPower(unsigned int value)
{	letMeUseSomePower = (value>0);
}*/

static LPDIRECTDRAWCLIPPER lpDDC = 0;
static LPDIRECTDRAWSURFACE lpDDS_secondary = 0;

#ifdef __PTC_SYSTEM_MENU__
    static int original_window_width;
    static int original_window_height;
    static HMENU system_menu;
#endif

typedef HRESULT (WINAPI * DIRECTDRAWCREATE) (GUID FAR *lpGUID,LPDIRECTDRAW FAR *lplpDD,IUnknown FAR *pUnkOuter);

void ptc_setTitle(const char * title)
{	if(!title) return ;	
	//strcpy(wTitle,title);
	//wTitle = title ;
	//wc.lpszClassName = (char*)title ;
}

static void ptc_paint_primary()
{
    RECT source;
    RECT destination;
    POINT point;

    // check
    if (lpDDS)
    {
        // setup source rectangle
        source.left = 0;
        source.top = 0;
        source.right = dx;
        source.bottom = dy;

        // get origin of client area
        point.x = 0;
        point.y = 0;
        ClientToScreen(wnd,&point);

        // get window client area
        GetClientRect(wnd,&destination);

        // offset destination rectangle
        destination.left += point.x;
        destination.top += point.y;
        destination.right += point.x;
        destination.bottom += point.y;

        // blt secondary to primary surface
       // IDirectDrawSurface_Blt(lpDDS,&destination,lpDDS_secondary,&source,DDBLT_WAIT,0);DDBLTFAST_WAIT
		 IDirectDrawSurface_Blt(lpDDS,&destination,lpDDS_secondary,&source,DDBLTFAST_WAIT,0);
    }
}

//#endif

// menu option identifier
#define SC_ZOOM_MSK 0x400
#define SC_ZOOM_12  0x405
#define SC_ZOOM_1x1 0x401
#define SC_ZOOM_2x2 0x402
#define SC_ZOOM_4x4 0x404

//extern void FMUSIC_fadeSound2Zero(void) ;  

void setInfo(char*title,int *h,int *w)
{	if(title)	wTitle = title ;
	if(h) { wHeight   = *h ;
			heightAdr = h  ;
	}
	if(w) { wWidth   = *w ;
			widthAdr = w  ;
	}	
}

void getWdsSize(int*w,int*h)
{	*h = wHeight ;
	*w = wWidth  ;
}

 // code to manage message callback..
u32 (*_onMsg)(u32 msg, u32 arg1, u32 arg2)=0;

void ptc_iniMsgCallBack(u32 (*onMsg)(u32 msg, u32 arg1, u32 arg2)) { _onMsg = onMsg; }

static LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{      // first, see if the msg is designed for the framebuffer itself..
       if(!inFullScreen)
		{   switch (message)
		    {  case WM_PAINT:
				ptc_paint_primary(); goto end;
			   break;
			   
               case WM_SYSCOMMAND:
				#ifdef __PTC_RESIZE_WINDOW__
			 	 #ifdef __PTC_SYSTEM_MENU__
				  if ((wParam&0xFFFFFFF0)==SC_ZOOM_MSK)
				  { int zoom = wParam & 0x7;
					if(zoom < 5)
					{	
						#ifdef __PTC_CENTER_WINDOW__
						int x = (GetSystemMetrics(SM_CXSCREEN) - original_window_width*zoom) >> 1;
						int y = (GetSystemMetrics(SM_CYSCREEN) - original_window_height*zoom) >> 1;
						SetWindowPos(hWnd, NULL, x, y,original_window_width*zoom, original_window_height*zoom, SWP_NOZORDER);
						#else
						  SetWindowPos(hWnd, NULL, 0, 0,original_window_width*zoom, original_window_height*zoom, SWP_NOMOVE | SWP_NOZORDER);
						#endif
					}
					else {
						// zoom 1/2
						int x = (GetSystemMetrics(SM_CXSCREEN) - original_window_width/2) >> 1;
						int y = (GetSystemMetrics(SM_CYSCREEN) - original_window_height/2) >> 1;
						SetWindowPos(hWnd, NULL, x, y,original_window_width/2, original_window_height/2, SWP_NOZORDER);

					}
				  }// else
                   goto end;
				 #endif
			    #endif
			   break;
            };
        } else { switch (message)
		         { case WM_ACTIVATEAPP: active = (BOOL) wParam; goto end; break;
    			   case WM_SETCURSOR:   SetCursor(0);			goto end; break; 
                 };
               }
       
       if(message == WM_KEYDOWN) // check for 'fin' and esc
       {/* if((wParam&0xFF) == 35) // 35 is "fin", switch between full screen and windowed..
         {	
			#ifdef keepOldSize // to keep windowed size
		    	if(!inFullScreen)
				{	oldH = wHeight ;
					oldW = wWidth  ;
				} else	{	wHeight = oldH ; *widthAdr = oldW ;
							wWidth  = oldW ; *heightAdr = oldH ;
						}
			#endif
				ptc_close();	// close curent window
				inFullScreen^=1;// invert full screen flag
				ptc_open(0,0,0);// and reopen a window ..
				message = 0x42424242; // inform user of the switch..
		 }*/
       }
        else
           /* window was resized */
		if(message == WM_SIZE)
			{	RECT destination;
				GetClientRect(wnd,&destination);
				wsizex = destination.right  ;
				wsizey = destination.bottom ;
				goto end;
    		}
       else
       	if(message == WM_MOUSEMOVE)
            {	wmousex = ((int)lParam)&0xffff ;	/* get pos in stretched output */
			    wmousey = ((int)lParam)>>16 ;
			    mousex = (wmousex*wWidth)/wsizex  ;	/* compute pos in buffer */
			    mousey = (wmousey*wHeight)/wsizey ;
			    //mouseLeftState = (((int)wParam) & MK_LBUTTON);
            }
       
       if(_onMsg) { if(_onMsg(message,wParam,lParam)) return 0; } // send msg to the callback, return ok if callback know the msg
        else if(message == WM_CLOSE) { ptc_close(); ExitProcess(0); };
      end: return(DefWindowProc(hWnd,message,wParam,lParam));
}

int ptc_open(char *title,int *w,int *h)
{   int x,y,width,height ;
    RECT rect;
    DDSCAPS capabilities;
    DDPIXELFORMAT format;
    DDSURFACEDESC descriptor;
    DIRECTDRAWCREATE DirectDrawCreate;

    if(title) // save or update windows info (for the switch between windowed and full screen, also for resolution change)
		wTitle = title  ;
	else	title = wTitle ;	
		
	if(w&&h)
		{	width = *w ;	wWidth  = width  ;
			height = *h ;	wHeight = height ;
		} else	{	height = wHeight;
					width  = wWidth ;
				}
		if(!(w&&h)) {	w = widthAdr ;
						h = heightAdr ;
						//if(!(widthAdr&&heightAdr)) return;
					}
		else		{	widthAdr = w ;
						heightAdr= h ;
					}

	if(w&&h)
		if(inFullScreen){ // if need && possible, resize the output to a classic size..
			if((height*4)/3 != width) // is 4/3 ?
			{
				#ifdef canAutoResizeInFullScreen
					if(height<240 && width<320) { height = 240; width = 320; }
					else	if(height<480 && width<640) { height = 480; width = 640; }
							else	if(height<600 && width<800) { height = 600; width = 800; } 
									else	inFullScreen=0;
					if(minimumFullScreenHeight)
						if(height < minimumFullScreenHeight)
							height = minimumFullScreenHeight ;
					if(minimumFullScreenWidth)
						if(width < minimumFullScreenWidth)
							width = minimumFullScreenWidth ;

				#else
					
					if(minimumFullScreenHeight)
						if(height < minimumFullScreenHeight)
							height = minimumFullScreenHeight ;
					if(minimumFullScreenWidth)
						if(width < minimumFullScreenWidth)
							width = minimumFullScreenWidth ;
				#endif
						if((height*4)/3 != width)	inFullScreen=0; // if not fine resised, stay in windowed..
						*heightAdr = height ; wHeight = height ;
						*widthAdr = width  ; wWidth  = width  ;
			}
		}

    // setup data
    dx = width;
    dy = height;

    // load direct draw library
    library = (HMODULE) LoadLibrary("ddraw.dll");
    if(!library) return 0;
	//{ showMsg("  ddraw.dll not found !\ncannot render the backbuffer :| ","sorry.."); return 0; }

    // get directdraw create function address
    DirectDrawCreate = (DIRECTDRAWCREATE) GetProcAddress(library,"DirectDrawCreate");
    
    // create directdraw interface
    if( (!DirectDrawCreate) || (FAILED(DirectDrawCreate(0,&lpDD,0))) ) return 0;
	//{ showMsg(" DirectDrawCreate failed !\ncannot render the backbuffer :| ","sorry.."); return 0; }

if(inFullScreen){
    // register window class
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
	#ifdef __PTC_ICON__
		wc.hInstance = GetModuleHandle(0);
		wc.hIcon = LoadIcon(wc.hInstance,__PTC_ICON__);
	#else
		wc.hInstance = 0;
		wc.hIcon = 0;
	#endif
		wc.hCursor = 0;
		wc.hbrBackground = 0;
		wc.lpszMenuName = 0;
		wc.lpszClassName = title;
		RegisterClass(&wc);

		// create window
	#ifdef __PTC_ICON__
		/*        invoke CreateWindowEx,WS_EX_OVERLAPPEDWINDOW,
								  ADDR szClassName,
								  ADDR szDisplayName,
								  WS_POPUP or WS_THICKFRAME,
								  Wtx,Wty,Wwd,Wht,
								  NULL,NULL,
								  hInst,NULL*/
		wnd = CreateWindow(title,title,WS_POPUP | WS_SYSMENU,0,0,0,0,0,0,0,0);
	#else
		wnd = CreateWindow(title,title,WS_POPUP,0,0,0,0,0,0,0,0);
	#endif

    // enter exclusive mode
    if (FAILED(IDirectDraw_SetCooperativeLevel(lpDD,wnd,DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN))) return 0;

    // enter display mode
    if (FAILED(IDirectDraw_SetDisplayMode(lpDD,width,height,32)))
    {
        if (FAILED(IDirectDraw_SetDisplayMode(lpDD,width,height,24)))
        {
            if (FAILED(IDirectDraw_SetDisplayMode(lpDD,width,height,16)))
            {
                // failure
                return 0;
            }
        }
    }

    // primary with two back buffers
    descriptor.dwSize  = sizeof(descriptor);
    descriptor.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    descriptor.dwBackBufferCount = 2;
    descriptor.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY | DDSCAPS_COMPLEX | DDSCAPS_FLIP;
    if (FAILED(IDirectDraw_CreateSurface(lpDD,&descriptor,&lpDDS,0)))
    {
        // primary with one back buffer
        descriptor.dwBackBufferCount = 1;
        if (FAILED(IDirectDraw_CreateSurface(lpDD,&descriptor,&lpDDS,0)))
        {
            // primary with no back buffers
            descriptor.dwFlags = DDSD_CAPS;
            descriptor.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY;
            if (FAILED(IDirectDraw_CreateSurface(lpDD,&descriptor,&lpDDS,0)))
            {
                // failure
                return 0;
            }
        }
    }

    // get back surface
    capabilities.dwCaps = DDSCAPS_BACKBUFFER;
    if (FAILED(IDirectDrawSurface_GetAttachedSurface(lpDDS,&capabilities,&lpDDS_back))) return 0;

} else { // case a windowed output

    // register window class
    wc.style = CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
	#ifdef __PTC_ICON__
		wc.hInstance = GetModuleHandle(0);
		wc.hIcon = LoadIcon(wc.hInstance,__PTC_ICON__);
	#else
		wc.hInstance = 0;
		wc.hIcon = 0;
	#endif
	wc.hCursor = LoadCursor(0,IDC_ARROW);
    wc.hbrBackground = 0;
    wc.lpszMenuName = 0;
    wc.lpszClassName = title;
    RegisterClass(&wc);

    // calculate window size
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,0);
    rect.right -= rect.left;
    rect.bottom -= rect.top;

	#ifdef __PTC_CENTER_WINDOW__
		// center window
		x = (GetSystemMetrics(SM_CXSCREEN) - rect.right) >> 1;
		y = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) >> 1;
	#else
		// let windows decide
		x = CW_USEDEFAULT;
		y = CW_USEDEFAULT;
	#endif

	#ifdef __PTC_RESIZE_WINDOW__
		// create resizable window
		wnd = CreateWindow(title,title,WS_OVERLAPPEDWINDOW,x,y,rect.right,rect.bottom,0,0,0,0);
		//wnd = CreateWindowEx(WS_EX_TOPMOST,title,title,WS_OVERLAPPEDWINDOW,x,y,rect.right,rect.bottom,0,0,0,0);
	#else
		// create fixed window
		wnd = CreateWindow(title,title,WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,x,y,rect.right,rect.bottom,0,0,0,0);
	#endif

	#ifdef setx2zoom
		// set zoom to x2
		{ int x = (GetSystemMetrics(SM_CXSCREEN) - width*2) >> 1;
		  int y = (GetSystemMetrics(SM_CYSCREEN) - height*2) >> 1;
		  SetWindowPos(wnd, NULL, x, y, width*2,height*2, SWP_NOZORDER);
		}
	#endif

    // show window
    ShowWindow(wnd,SW_NORMAL);

	#ifdef __PTC_RESIZE_WINDOW__
		#ifdef __PTC_SYSTEM_MENU__
			// add entry to system menu to restore original window size
			system_menu = GetSystemMenu(wnd,FALSE);
			AppendMenu(system_menu, MF_STRING, SC_ZOOM_12, "Zoom 1 / 2");
			AppendMenu(system_menu, MF_STRING, SC_ZOOM_1x1, "Zoom 1 x 1");
				#ifdef allow2xZoom
					AppendMenu(system_menu, MF_STRING, SC_ZOOM_2x2, "Zoom 2 x 2");
					#ifdef allow4xZoom
						AppendMenu(system_menu, MF_STRING, SC_ZOOM_4x4, "Zoom 4 x 4");
					#endif
				#endif
			// save original window size
			original_window_width = rect.right;
			original_window_height = rect.bottom;
		#endif
	#endif

    // enter cooperative mode
    if (FAILED(IDirectDraw_SetCooperativeLevel(lpDD,wnd,DDSCL_NORMAL))) return 0;

    // primary with no back buffers
    descriptor.dwSize  = sizeof(descriptor);
    descriptor.dwFlags = DDSD_CAPS;
    descriptor.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY;
    if (FAILED(IDirectDraw_CreateSurface(lpDD,&descriptor,&lpDDS,0))) return 0;

    // create secondary surface
    descriptor.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    descriptor.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    descriptor.dwWidth = width;
    descriptor.dwHeight = height;
    if (FAILED(IDirectDraw_CreateSurface(lpDD,&descriptor,&lpDDS_secondary,0))) return 0;
    
    // create clipper
    if (FAILED(IDirectDraw_CreateClipper(lpDD,0,&lpDDC,0))) return 0;

    // set clipper to window
    if (FAILED(IDirectDrawClipper_SetHWnd(lpDDC,0,wnd))) return 0;

    // attach clipper object to primary surface
    if (FAILED(IDirectDrawSurface_SetClipper(lpDDS,lpDDC))) return 0;
    
    // set back to secondary
    lpDDS_back = lpDDS_secondary;

}

    // get pixel format
    format.dwSize = sizeof(format);
    if (FAILED(IDirectDrawSurface_GetPixelFormat(lpDDS,&format))) return 0;

    // check that format is direct color
    if (!(format.dwFlags & DDPF_RGB)) return 0;
    
    // request converter function
    convert = ptc_request_converter(format.dwRGBBitCount,format.dwRBitMask,format.dwGBitMask,format.dwBBitMask);
    if (!convert) return 0;

	#ifdef __PTC_DISABLE_SCREENSAVER__
		// disable screensaver while ptc is open
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0, 0, 0);
	#endif
	DragAcceptFiles( wnd, 1);

	// success
    return 1;
}

void ptc_getMsg(void)
{  MSG message ;
   while (PeekMessage(&message,wnd,0,0,PM_REMOVE))
	{	// translate and dispatch
		TranslateMessage(&message);
		DispatchMessage(&message);
	};
}

void showMsg(const char*msg,const char*title)
{	if(!title) title = "info" ;
	MessageBox(wnd,msg,title,MB_OK);
}

int ptc_update(void *buffer)
{   int y;
    char8 *src;
    char8 *dst;
    int src_pitch;
    int dst_pitch;
    MSG message;
    DDSURFACEDESC descriptor;
	static unsigned int oldTick = 0 ;
	extern unsigned int tick;

    // process messages
    while (PeekMessage(&message,wnd,0,0,PM_REMOVE))
    {	TranslateMessage(&message);
		DispatchMessage(&message);
    };

    if (!inFullScreen || active)
    {    // restore surfaces
		IDirectDrawSurface_Restore(lpDDS);
		if(!inFullScreen)	IDirectDrawSurface_Restore(lpDDS_secondary);

        // lock back surface
        descriptor.dwSize = sizeof descriptor;
        if (FAILED(IDirectDrawSurface_Lock(lpDDS_back,0,&descriptor,DDLOCK_WAIT,0))) return 0;
    
        // calculate pitches
        src_pitch = dx<<2;// * 4; // 32b
        dst_pitch = descriptor.lPitch;

        // copy pixels to back surface
        src = (char8*) buffer;
        dst = (char8*) descriptor.lpSurface;
        for (y=0; y<dy; y++)
        {    // convert line
            convert(src,dst,dx);
            src += src_pitch;
            dst += dst_pitch;
        }

		if(_onAfterZoom) _onAfterZoom(descriptor.lpSurface,dx,dy,descriptor.dwRefreshRate);

        // unlock back surface
        IDirectDrawSurface_Unlock(lpDDS_back,descriptor.lpSurface);

		if(inFullScreen){
            // flip primary surface
            IDirectDrawSurface_Flip(lpDDS,0,DDFLIP_WAIT);
        } else {
            // paint primary
            ptc_paint_primary();
        }
/*
		if(!letMeUseSomePower)
		{	// limit to 66 frames/second (66fps -> ~15ms between eatch screen refresh)
			do { Sleep(1); tick = GetTickCount(); } while(tick - oldTick < 13); oldTick = tick;
		} else Sleep(0);*/
    }

    return 1;
}

void ptc_close(void)//int exitProcess)
{
	if(!inFullScreen)
	{	// check secondary
		if (lpDDS_secondary)
		{	// release secondary
			IDirectDrawSurface_Release(lpDDS_secondary);
			lpDDS_secondary = 0;
		}
	}

    // check
    if (lpDDS)
    {   // release primary
        IDirectDrawSurface_Release(lpDDS);
        lpDDS = 0;
    }

    // check
    if (lpDD)
    {   // leave display mode
        IDirectDraw_RestoreDisplayMode(lpDD);

        // leave exclusive mode
        IDirectDraw_SetCooperativeLevel(lpDD,wnd,DDSCL_NORMAL);

        // free direct draw
        IDirectDraw_Release(lpDD);
        lpDD = 0;
    }

    // hide window
    DestroyWindow(wnd);

    // check
    if (library)
    {   // free library
        FreeLibrary(library);
        library = 0;
    }	
/*
	if(exitProcess) {	if(_onExitProcess) _onExitProcess();
						#ifdef __PTC_DISABLE_SCREENSAVER__
							// enable screensaver now that ptc is closed
							SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 1, 0, 0);
						#endif
						ExitProcess(0);
					}*/
}

#endif

#endif
