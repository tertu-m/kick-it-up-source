/*
	1999-2000 Copyright (c) mahalo, the nogada worker.. (Amigo~*). All rights reserved.
	          Copyright (c) Hyung-seok Choi, the Graphics Designer. All rights reserved.

  	2000/07/22 'Work for Opening Source'
			- Removed Motion-Video Support (for improve safety and speed)
			- Removed Xaudio player mp3 play (this required their own dlls)

			- Added Audio-Media play using DirectShow.
*/

#include <stdio.h>
#include <windows.h>
#include <direct.h>
#include <string.h>
#include <io.h>
#include <time.h>
#include <dsound.h>

#include "Media.h"
#include "Main.h"
#include "Input.h"
#include "Select.h"
#include "Result.h"

#include "Song.h"
//#include "sound.h"
#include "DSUtil.h"

#define DISCSIZE_X	300
#define DISCSIZE_Y	200

#define	STILL_DRAWING	100

extern SONG					CSONG[512];
extern LPDIRECTDRAWSURFACE	 SelectBack;
extern LPDIRECTDRAWSURFACE	SelectFont;
extern LPDIRECTDRAWSURFACE	SongTitle;
extern	LPDIRECTDRAWSURFACE	NumberFont;

extern LPDIRECTDRAWSURFACE	SongBack;
extern LPDIRECTDRAWSURFACE	Background;
extern LPDIRECTDRAWSURFACE	ShiftLeft;
extern LPDIRECTDRAWSURFACE	ShiftRight;
extern	LPDIRECTDRAWSURFACE	ModeIcon;
extern	LPDIRECTDRAWSURFACE	CFont;
extern	LPDIRECTDRAWSURFACE	Diff;
extern	LPDIRECTDRAWSURFACE	DoubleIcon;
extern	LPDIRECTDRAWSURFACE	CrazyIcon;
extern	LPDIRECTDRAWSURFACE	HardIcon;
extern	LPDIRECTDRAWSURFACE	EasyIcon;

extern double				bpm,bpm2,bpm3;
extern int					start,start2,start3;
extern	int					tick;
extern	DWORD					bunki,bunki2;

extern char					SongName[MAX_PATH+1];
extern char					SongName2[MAX_PATH+1];
extern char					SongName3[MAX_PATH+1];
extern char					Title[MAX_PATH+1];
extern	int					start1;

extern	char				GameMode;

extern	char				Couple;
extern	char				Double;
// 커플 데이터 끝


//extern CWAVE				*WavePrimary;
extern	LPDIRECTSOUND		lpds;

extern LPDIRECTSOUNDBUFFER			g_dsMode;
extern LPDIRECTSOUNDBUFFER			g_dsCancel;
extern LPDIRECTSOUNDBUFFER			g_dsMove;
extern LPDIRECTSOUNDBUFFER			g_dsSelectSong;

extern char First;

DWORD	startTimer, curTimer;

extern BOOL	IntroFlag;

HRESULT	ClpBlt3(int x ,int y ,LPDIRECTDRAWSURFACE ds,LPRECT srect,DWORD mode)
{
	static RECT sRect;
	HRESULT	hRet;

	memcpy(&sRect,srect,sizeof(sRect));
	
	if(x>640 || y>480) return DD_OK;

	if(y+(srect->bottom-srect->top)>480)srect->bottom=srect->bottom-(y+(srect->bottom-srect->top)-480);
	if(y<0)
	{
		srect->top-=y;
		y=0;
	}

	if(x+(srect->right-srect->left)>640)srect->right=srect->right-(x+(srect->right-srect->left)-640);
	if(x<0)
	{
		srect->left-=x;
		x=0;
	}
	
	hRet=g_pDDSBack->BltFast(x,y,ds,srect,mode);

	memcpy(srect,&sRect,sizeof(sRect));
	
	return hRet;

}

void Read()
{
	HANDLE	hFind;
	DWORD	Count=0;
	WIN32_FIND_DATA lpData;

	char* cPathStr;

	hFind=(HANDLE)FindFirstFile("*.*",&lpData);
	cPathStr=_strupr(_strdup(lpData.cFileName));
	if(strcmp(cPathStr,"SONG")!=0)
	{
		for(;;)
		{
			if(FindNextFile(hFind,&lpData)==FALSE)break;
			cPathStr=_strupr(_strdup(lpData.cFileName));
			if(strcmp(cPathStr,"SONG")==0)break;
		}
	}
	if(lpData.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY)
	{
		SetCurrentDirectory(cPathStr);
		hFind=FindFirstFile("*.*",&lpData);
		if(lpData.cFileName[0]!='.' && lpData.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY)
		{
			SetCurrentDirectory(lpData.cFileName);

			if(access("Crazy_2.stf",04)==0)CSONG[Count].ReadCrazy_2_STF("Crazy_2.stf");
			else if(access("Crazy_2.ksf",04)==0)CSONG[Count].ReadCrazy_2_KSF("Crazy_2.ksf");

			if(access("Crazy_1.stf",04)==0)CSONG[Count].ReadCrazy_1_STF("Crazy_1.stf"),Count++;
			else if(access("Crazy_1.ksf",04)==0)CSONG[Count].ReadCrazy_1_KSF("Crazy_1.ksf"),Count++;

			if(access("Hard_2.stf",04)==0)CSONG[Count].ReadHard_2_STF("Hard_2.stf");
			else if(access("Hard_2.ksf",04)==0)CSONG[Count].ReadHard_2_KSF("Hard_2.ksf");

			if(access("Hard_1.stf",04)==0)CSONG[Count].ReadHard_1_STF("Hard_1.stf"),Count++;
			else if(access("Hard_1.ksf",04)==0)CSONG[Count].ReadHard_1_KSF("Hard_1.ksf"),Count++;

			if(access("Easy_2.stf",04)==0)CSONG[Count].ReadEasy_2_STF("Easy_2.stf");
			else if(access("Easy_2.ksf",04)==0)CSONG[Count].ReadEasy_2_KSF("Easy_2.ksf");

			if(access("Easy_1.stf",04)==0)CSONG[Count].ReadEasy_1_STF("Easy_1.stf"),Count++;
			else if(access("Easy_1.ksf",04)==0)CSONG[Count].ReadEasy_1_KSF("Easy_1.ksf"),Count++;

			if(access("Double.stf",04)==0)CSONG[Count].ReadDouble_STF("Double.stf"),Count++;
			else if(access("Double.ksf",04)==0)CSONG[Count].ReadDouble_KSF("Double.ksf");

			//if(CSONG[Count].bpm!=0)Count++;
			SetCurrentDirectory("..\\");
		}

		for(;;)
		{
			if(FindNextFile(hFind,&lpData)==FALSE)break;
			else
			{
				if(lpData.cFileName[0]!='.' && lpData.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY)
				{
					SetCurrentDirectory(lpData.cFileName);

					if(access("Crazy_2.stf",04)==0)CSONG[Count].ReadCrazy_2_STF("Crazy_2.stf");
					else if(access("Crazy_2.ksf",04)==0)CSONG[Count].ReadCrazy_2_KSF("Crazy_2.ksf");

					if(access("Crazy_1.stf",04)==0)CSONG[Count].ReadCrazy_1_STF("Crazy_1.stf"),Count++;
					else if(access("Crazy_1.ksf",04)==0)CSONG[Count].ReadCrazy_1_KSF("Crazy_1.ksf"),Count++;

					if(access("Hard_2.stf",04)==0)CSONG[Count].ReadHard_2_STF("Hard_2.stf");
					else if(access("Hard_2.ksf",04)==0)CSONG[Count].ReadHard_2_KSF("Hard_2.ksf");

					if(access("Hard_1.stf",04)==0)CSONG[Count].ReadHard_1_STF("Hard_1.stf"),Count++;
					else if(access("Hard_1.ksf",04)==0)CSONG[Count].ReadHard_1_KSF("Hard_1.ksf"),Count++;

					if(access("Easy_2.stf",04)==0)CSONG[Count].ReadEasy_2_STF("Easy_2.stf");
					else if(access("Easy_2.ksf",04)==0)CSONG[Count].ReadEasy_2_KSF("Easy_2.ksf");

					if(access("Easy_1.stf",04)==0)CSONG[Count].ReadEasy_1_STF("Easy_1.stf"),Count++;
					else if(access("Easy_1.ksf",04)==0)CSONG[Count].ReadEasy_1_KSF("Easy_1.ksf"),Count++;

					if(access("Double.stf",04)==0)CSONG[Count].ReadDouble_STF("Double.stf"),Count++;
					else if(access("Double.ksf",04)==0)CSONG[Count].ReadDouble_KSF("Double.ksf"),Count++;
				
					//if(CSONG[Count].bpm!=0)Count++;
					SetCurrentDirectory("..\\");
				}
			}
		}
		SetCurrentDirectory("..\\");
	}
	FindClose(hFind);

	if(CSONG[0].bpm==0)
	{
		MessageBox(hWnd,"Song directory not found or No song data.","KICKITUP ERROR",MB_OK);
//		ReleaseAllObjects();

		PostQuitMessage(0);
	}
}

void SelectSong(void)
{
	DWORD count,i;
	static DWORD current;
	static DWORD SelectCurrent;
	static int Selected, zoom,toggle,speed;

	RECT	lRect;
	int ModeTemp1p, ModeTemp2p;
	
	static	time_t t;
	
	static	int a,b,c;

	static	int iMove;

	RECT DiscSize,Screen;

	char s[50];

	if(First==0)
	{
		startTimer=timeGetTime();
		if(Start1p==FALSE)
		{
			HighSpeed1p=1;
			bModeMirror1p=FALSE;
			bModeNonstep1p=FALSE;
			bModeSynchro=FALSE;
			bModeUnion1p=FALSE;
			bModeRandom1p=FALSE;
			b4dMix1p=FALSE;
				HighSpeed1p_1=1;
				HighSpeed1p_3=1;
				HighSpeed1p_5=1;
				HighSpeed1p_7=1;
				HighSpeed1p_9=1;
			bModeVanish1p=FALSE;
			bModeSuddenR1p=FALSE;
			bModeRandomS1p=FALSE;

		}
		if(Start2p==FALSE)
		{
			HighSpeed2p=1;
			bModeMirror2p=FALSE;
			bModeNonstep2p=FALSE;
			bModeUnion2p=FALSE;
			bModeRandom2p=FALSE;
			b4dMix2p=FALSE;
				HighSpeed2p_1=1;
				HighSpeed2p_3=1;
				HighSpeed2p_5=1;
				HighSpeed2p_7=1;
				HighSpeed2p_9=1;
			bModeVanish2p=FALSE;
			bModeSuddenR1p=FALSE;
			bModeRandomS1p=FALSE;
		}
		DDFillSurface(g_pDDSPrimary,0);
		DDFillSurface(g_pDDSBack,0);
		
		//FadeToSurface(SelectBack);
		g_pDDSBack->BltFast(0,0, SelectBack, NULL, DDBLTFAST_NOCOLORKEY);
		
		a=Start1p;b=Start2p;
		First++;

		if(g_dsSelectSong)g_dsSelectSong->Play(0,0,DSBPLAY_LOOPING);
	}

	DiscSize.top=0;
	DiscSize.left=0;
	DiscSize.right=300;
	DiscSize.bottom=200;

	for(count=0;;count++)
	{
		if(count!=0)CSONG[count].Prev=count-1;
		
		CSONG[count].Next=count+1;
		
		if(CSONG[count].bpm==0)
		{
			CSONG[count].Prev=0;
			count--;
			CSONG[count].Next=0;
			CSONG[0].Prev=count;
			break;
		}
	}

	if(speed==1) //일단은 변수를 이용합니다. 곧 타이머 형식으로 바꾸도록 합시다. 
	{
		speed=0;
		if(toggle==0)
		{
			if(zoom==10)toggle=1;
			else zoom++;
		}
		else if(toggle==1)
		{
			if(zoom==0)toggle=0;
			else zoom--;
		}
	}
	else speed++; 

	ReadGameInput();

	if(PressedKey1p[5]==TRUE)
	{
		if(Start1p==FALSE)
		{
			Start1p=TRUE;
		}
	}
	if(PressedKey2p[5]==TRUE)
	{
		if(Start2p==FALSE)
		{
			Start2p=TRUE;
		}
	}

	ModeTemp1p=ScanHiddenMode1p();

	if(ModeTemp1p)if(IntroFlag){intro->OnMediaStop();delete intro;IntroFlag=FALSE;}

	switch(ModeTemp1p)
	{
		case HMODE_SUDDENR:
			bModeSuddenR1p=TRUE;
			bModeVanish1p=FALSE;

			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_RANDOMS:
			bModeRandomS1p=TRUE;
			HighSpeed1p=1;

			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_2X:
			HighSpeed1p=2;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_4X:
			HighSpeed1p=4;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_8X:
			HighSpeed1p=8;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_MIRROR:
			bModeMirror1p=TRUE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_NONSTEP:
			bModeNonstep1p=TRUE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_SYNCHRO:
			bModeSynchro=TRUE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_UNION:
			bModeUnion1p=TRUE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_RANDOM:
			bModeRandom1p=TRUE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_4DMIX:
			srand((unsigned) time(&t));

			HighSpeed1p_1=1+rand()%8;
			HighSpeed1p_3=1+rand()%8;
			HighSpeed1p_5=1+rand()%8;
			HighSpeed1p_7=1+rand()%8;
			HighSpeed1p_9=1+rand()%8;

			b4dMix1p=TRUE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_VANISH:
			bModeVanish1p=TRUE;
			bModeSuddenR2p=FALSE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
/*		case HMODE_NONSTOPDOUBLE:
			if(Start1p&&Start2p)break;
			Double=TRUE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;*/
		case HMODE_CANCEL:
			if(g_dsCancel)g_dsCancel->Play(0,0,0);
			HighSpeed1p=1;
			bModeMirror1p=FALSE;
			bModeNonstep1p=FALSE;
			bModeSynchro=FALSE;
			bModeUnion1p=FALSE;
			bModeRandom1p=FALSE;
			b4dMix1p=FALSE;
				HighSpeed1p_1=1;
				HighSpeed1p_3=1;
				HighSpeed1p_5=1;
				HighSpeed1p_7=1;
				HighSpeed1p_9=1;
			bModeVanish1p=FALSE;
			bModeSuddenR1p=FALSE;
			bModeRandomS1p=FALSE;
			Double=FALSE;
			break;
	}

	ModeTemp2p=ScanHiddenMode2p();
	if(ModeTemp2p)if(IntroFlag){intro->OnMediaStop();delete intro;IntroFlag=FALSE;}
	
	switch(ModeTemp2p)
	{
		case HMODE_SUDDENR:
			bModeSuddenR2p=TRUE;
			bModeVanish2p=FALSE;

			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_RANDOMS:
			bModeRandomS2p=TRUE;
			HighSpeed2p=1;

			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_2X:
			HighSpeed2p=2;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_4X:
			HighSpeed2p=4;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_8X:
			HighSpeed2p=8;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_MIRROR:
			bModeMirror2p=TRUE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_NONSTEP:
			bModeNonstep2p=TRUE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_SYNCHRO:
			bModeSynchro=TRUE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_UNION:
			bModeUnion2p=TRUE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_RANDOM:
			bModeRandom2p=TRUE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_4DMIX:

			srand((unsigned) time(&t));

			HighSpeed2p_1=1+rand()%8;
			HighSpeed2p_3=1+rand()%8;
			HighSpeed2p_5=1+rand()%8;
			HighSpeed2p_7=1+rand()%8;
			HighSpeed2p_9=1+rand()%8;

			b4dMix2p=TRUE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
		case HMODE_VANISH:
			bModeVanish2p=TRUE;
			bModeSuddenR2p=FALSE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
			break;
/*		case HMODE_NONSTOPDOUBLE:
			if(Start1p&&Start2p)break;
			Double=TRUE;
			if(g_dsMode){g_dsMode->Stop();g_dsMode->SetCurrentPosition(0);g_dsMode->Play(0,0,0);}
*/			break;

		case HMODE_CANCEL:
			if(g_dsCancel)g_dsCancel->Play(0,0,0);
			HighSpeed2p=1;
			bModeMirror2p=FALSE;
			bModeNonstep2p=FALSE;
			bModeUnion2p=FALSE;
			bModeRandom2p=FALSE;
			b4dMix2p=FALSE;
				HighSpeed2p_1=1;
				HighSpeed2p_3=1;
				HighSpeed2p_5=1;
				HighSpeed2p_7=1;
				HighSpeed2p_9=1;
			bModeVanish2p=FALSE;
			bModeSuddenR2p=FALSE;
			bModeRandomS2p=FALSE;
			Double=FALSE;
			break;
	}

	if((Start1p && PressedKey1p[1]) || (Start2p && PressedKey2p[1]))
	{
		if(IntroFlag){intro->OnMediaStop();delete intro;IntroFlag=FALSE;}
		if(g_dsMove){g_dsMove->Stop();g_dsMove->SetCurrentPosition(0);g_dsMove->Play(0,0,0);}

		Selected=0;

		iMove=1;
		current=CSONG[CSONG[current].Prev].Prev;
	}

	if((Start1p && PressedKey1p[3]) || (Start2p && PressedKey2p[3]))
	{
		if(IntroFlag){intro->OnMediaStop();delete intro;IntroFlag=FALSE;}
		if(g_dsMove){g_dsMove->Stop();g_dsMove->SetCurrentPosition(0);g_dsMove->Play(0,0,0);}

		Selected=0;

		iMove=-1;
		current=CSONG[CSONG[current].Next].Next;
	}
	
	if((Start1p && PressedKey1p[7]) || (Start2p && PressedKey2p[7]))
	{
		if(Selected==7)
		{
			SelectCurrent=current;
			PressedKey1p[0]=3;
		}
		else
		{
			if(IntroFlag){intro->OnMediaStop();delete intro;IntroFlag=FALSE;}
			if(g_dsMove){g_dsMove->Stop();g_dsMove->SetCurrentPosition(0);g_dsMove->Play(0,0,0);}
/*			if(CSONG[current].Int_Song)
			{
				if(g_dsSelectSong)g_dsSelectSong->Stop();
				g_Cur_Song=CSONG[current].Int_Song;
				g_Cur_Song->SetCurrentPosition(0);
				g_Cur_Song->Play(0,0,0);
			}
			else*/ if(access(CSONG[current].WavsName,04)==0)
			{
				IntroFlag=TRUE;
				intro= new CMedia;
				intro->OpenMediaFile(CSONG[current].WavsName);
				intro->OnMediaPlay();
			}
			else if(access(CSONG[current].Wavs3Name,04)==0)
			{
				IntroFlag=TRUE;
				intro= new CMedia;
				intro->OpenMediaFile(CSONG[current].Wavs3Name);
				intro->OnMediaPlay();
			}

			Selected=7;
		}
	}
	
	if((Start1p && PressedKey1p[9]) || (Start2p && PressedKey2p[9]))
	{
		if(Selected==9)
		{
			SelectCurrent=CSONG[current].Next;
			PressedKey1p[0]=3;
		}
		else
		{
			if(IntroFlag){intro->OnMediaStop();delete intro;IntroFlag=FALSE;}
			if(g_dsMove){g_dsMove->Stop();g_dsMove->SetCurrentPosition(0);g_dsMove->Play(0,0,0);}
/*			if(CSONG[CSONG[current].Next].Int_Song)
			{
				if(g_dsSelectSong)g_dsSelectSong->Stop();
				g_Cur_Song=CSONG[CSONG[current].Next].Int_Song;
				g_Cur_Song->SetCurrentPosition(0);
				g_Cur_Song->Play(0,0,0);
			}
			else */if(access(CSONG[CSONG[current].Next].WavsName,04)==0)
			{
				IntroFlag=TRUE;
				intro= new CMedia;
				intro->OpenMediaFile(CSONG[CSONG[current].Next].WavsName);
				intro->OnMediaPlay();
			}
			else if(access(CSONG[CSONG[current].Next].Wavs3Name,04)==0)
			{
				IntroFlag=TRUE;
				intro= new CMedia;
				intro->OpenMediaFile(CSONG[CSONG[current].Next].Wavs3Name);
				intro->OnMediaPlay();
			}

			Selected=9;
		}
	}

	if(PressedKey1p[0]==3)
	{
		PressedKey1p[0]=0;

		if(CSONG[SelectCurrent].HaveCrazy==TRUE)GameMode=MODE_CRAZY,Double=FALSE;
	   	else if(CSONG[SelectCurrent].HaveDouble==TRUE)GameMode=MODE_DOUBLE,Double=TRUE;
	   	else if(CSONG[SelectCurrent].HaveEasy==TRUE)GameMode=MODE_EASY,Double=FALSE;
	   	else if(CSONG[SelectCurrent].HaveHard==TRUE)GameMode=MODE_HARD,Double=FALSE;
		else return;

		if(Start1p && Start2p)
		{
			Couple=TRUE;
			if(CSONG[SelectCurrent].HaveCouple==FALSE)bModeSynchro=TRUE;
		} else Couple=FALSE;

		if(SongBack)
		{
			SongBack->Release();
			SongBack=NULL;
		}
		
		if(g_dsCancel)
		{
			g_dsCancel->Stop();
			g_dsCancel->Play(0,0,0);
		}

		if(SongTitle)
		{
			SongTitle->Release();
			SongTitle=NULL;
		}
		SongTitle = DDLoadBitmap(g_pDD,CSONG[SelectCurrent].TitlePath,0,0);
		SongBack = DDLoadBitmap(g_pDD,CSONG[SelectCurrent].BMPPath,0,0);
		if(SongBack == NULL)
		{
			if(SongTitle)SongBack=DDLoadBitmap(g_pDD,CSONG[SelectCurrent].TitlePath,0,0);
			else SongBack=DDLoadBitmap(g_pDD, "Images\\Back.bmp",0,0);
		}
		if(SongTitle == NULL)
		{
			SongTitle=DDLoadBitmap(g_pDD,"Images\\NoTitle.bmp",0,0);
			if(SongTitle)g_pDDSPrimary->BltFast(0,0,SongTitle,NULL,DDBLTFAST_NOCOLORKEY);
		}
		else g_pDDSPrimary->BltFast(0,0,SongTitle,NULL,DDBLTFAST_NOCOLORKEY);

		if(GameMode == MODE_DOUBLE)
		{
			memcpy(&Data_Double,&CSONG[SelectCurrent].Data_Double,sizeof(CSONG[SelectCurrent].Data_Double));
		}
		else if(Couple==TRUE)
		{
			switch(GameMode)
			{
				case MODE_CRAZY:
					if(bModeSynchro)
					{
						memcpy(&Data,&CSONG[SelectCurrent].Data_Crazy,sizeof(CSONG[SelectCurrent].Data_Crazy));
						memcpy(&Data1,&CSONG[SelectCurrent].Data_Crazy,sizeof(CSONG[SelectCurrent].Data_Crazy));

						for(i=0;i<MAX_DATA;i++)
						{
							Data1[i][5]=Data1[i][0];
							Data1[i][6]=Data1[i][1];
							Data1[i][7]=Data1[i][2];
							Data1[i][8]=Data1[i][3];
							Data1[i][9]=Data1[i][4];
						}
					}
					else
					{
						memcpy(&Data,&CSONG[SelectCurrent].Data_Crazy1,sizeof(CSONG[SelectCurrent].Data_Crazy1));
						memcpy(&Data1,&CSONG[SelectCurrent].Data_Crazy1,sizeof(CSONG[SelectCurrent].Data_Crazy1));
					}
					break;
				case MODE_EASY:
					if(bModeSynchro)
					{
						memcpy(&Data,&CSONG[SelectCurrent].Data_Easy,sizeof(CSONG[SelectCurrent].Data_Easy));
						memcpy(&Data1,&CSONG[SelectCurrent].Data_Easy,sizeof(CSONG[SelectCurrent].Data_Easy));
						
						for(i=0;i<MAX_DATA;i++)
						{
							Data1[i][5]=Data1[i][0];
							Data1[i][6]=Data1[i][1];
							Data1[i][7]=Data1[i][2];
							Data1[i][8]=Data1[i][3];
							Data1[i][9]=Data1[i][4];
						}
					}
					else
					{
						memcpy(&Data,&CSONG[SelectCurrent].Data_Easy1,sizeof(CSONG[SelectCurrent].Data_Easy1));
						memcpy(&Data1,&CSONG[SelectCurrent].Data_Easy1,sizeof(CSONG[SelectCurrent].Data_Easy1));
					}
					break;
				case MODE_HARD:
					if(bModeSynchro)
					{
						memcpy(&Data,&CSONG[SelectCurrent].Data_Hard,sizeof(CSONG[SelectCurrent].Data_Hard));
						memcpy(&Data1,&CSONG[SelectCurrent].Data_Hard,sizeof(CSONG[SelectCurrent].Data_Hard));
						
						for(i=0;i<MAX_DATA;i++)
						{
							Data1[i][5]=Data1[i][0];
							Data1[i][6]=Data1[i][1];
							Data1[i][7]=Data1[i][2];
							Data1[i][8]=Data1[i][3];
							Data1[i][9]=Data1[i][4];
						}
					}
					else
					{
						memcpy(&Data,&CSONG[SelectCurrent].Data_Hard1,sizeof(CSONG[SelectCurrent].Data_Hard1));
						memcpy(&Data1,&CSONG[SelectCurrent].Data_Hard1,sizeof(CSONG[SelectCurrent].Data_Hard1));
					}
					break;
			}
		}
		else 
		{
			switch(GameMode)
			{
				case MODE_CRAZY:
					if(Start1p)memcpy(&Data,CSONG[SelectCurrent].Data_Crazy,sizeof(Data));
					else
					{
						memcpy(&Data,CSONG[SelectCurrent].Data_Crazy,sizeof(Data));
						memcpy(&Data1,CSONG[SelectCurrent].Data_Crazy,sizeof(Data));
						for(i=0;i<MAX_DATA;i++)
						{
							Data1[i][5]=Data[i][0];
							Data1[i][6]=Data[i][1];
							Data1[i][7]=Data[i][2];
							Data1[i][8]=Data[i][3];
							Data1[i][9]=Data[i][4];
						}
					}
					break;
				case MODE_EASY:
					if(Start1p)memcpy(&Data,CSONG[SelectCurrent].Data_Easy,sizeof(Data));
					else
					{
						memcpy(&Data,CSONG[SelectCurrent].Data_Easy,sizeof(Data));
						memcpy(&Data1,CSONG[SelectCurrent].Data_Easy,sizeof(Data));
						for(i=0;i<MAX_DATA;i++)
						{
							Data1[i][5]=Data[i][0];
							Data1[i][6]=Data[i][1];
							Data1[i][7]=Data[i][2];
							Data1[i][8]=Data[i][3];
							Data1[i][9]=Data[i][4];
						}
					}
					break;
				case MODE_HARD:
					if(Start1p)memcpy(&Data,CSONG[SelectCurrent].Data_Hard,sizeof(Data));
					else
					{
						memcpy(&Data,CSONG[SelectCurrent].Data_Hard,sizeof(Data));
						memcpy(&Data1,CSONG[SelectCurrent].Data_Hard,sizeof(Data));
						for(i=0;i<MAX_DATA;i++)
						{
							Data1[i][5]=Data[i][0];
							Data1[i][6]=Data[i][1];
							Data1[i][7]=Data[i][2];
							Data1[i][8]=Data[i][3];
							Data1[i][9]=Data[i][4];
						}
					}
					break;
			}
		}

		bpm=CSONG[SelectCurrent].bpm;
		bpm2=CSONG[SelectCurrent].bpm2;
		bpm3=CSONG[SelectCurrent].bpm3;
		bunki=CSONG[SelectCurrent].Bunki;
		bunki2=CSONG[SelectCurrent].Bunki2;
		
		switch(GameMode)
		{
			case MODE_CRAZY:
				start=CSONG[SelectCurrent].Crazy_Start;
				start2=CSONG[SelectCurrent].Crazy_Start2;
				start3=CSONG[SelectCurrent].Crazy_Start3;
				tick=CSONG[SelectCurrent].Crazy_Tick;
				break;
			case MODE_HARD:
				start=CSONG[SelectCurrent].Hard_Start;
				start2=CSONG[SelectCurrent].Hard_Start2;
				start3=CSONG[SelectCurrent].Hard_Start3;
				tick=CSONG[SelectCurrent].Hard_Tick;
				break;
			case MODE_EASY:
				start=CSONG[SelectCurrent].Easy_Start;
				start2=CSONG[SelectCurrent].Easy_Start2;
				start3=CSONG[SelectCurrent].Easy_Start3;
				tick=CSONG[SelectCurrent].Easy_Tick;
				break;
			case MODE_DOUBLE:
				start=CSONG[SelectCurrent].Double_Start;
				start2=CSONG[SelectCurrent].Double_Start2;
				start3=CSONG[SelectCurrent].Double_Start3;
				tick=CSONG[SelectCurrent].Double_Tick;
				break;
		}

		strcpy(SongName,CSONG[SelectCurrent].WavName);
		strcpy(SongName2,CSONG[SelectCurrent].MP3Name);
		strcpy(SongName3,CSONG[SelectCurrent].MPGName);
		strcpy(Title,CSONG[SelectCurrent].SongTitle);

		Judgement1p=NONE;
		Judgement2p=NONE;

		if(GameMode==MODE_DOUBLE)ProgramState=DOUBLE;
//		else if(Couple)ProgramState=COUPLE;
		else ProgramState=STAGE1;
		
		if(GameMode==MODE_DOUBLE)
		{
			if(Start1p && Start2p) Start2p=FALSE;
		}
		Selected=0;
		
		if(g_dsSelectSong)g_dsSelectSong->Stop();
		if(IntroFlag){intro->OnMediaStop();delete intro;IntroFlag=FALSE;}
		
		if(access(SongName,04)==0)
		{
			SongFlag=TRUE;
			song=new CMedia;
			song->OpenMediaFile(SongName);
		}
		else if(access(SongName2,04)==0)
		{
			SongFlag=TRUE;
			song=new CMedia;
			song->OpenMediaFile(SongName2);
		}
		else SongFlag=FALSE;

		DDFillSurface(g_pDDSBack, 0); // ????

		First=0;
		Combo1p=0;
		Combo2p=0;

		cPerfect1p=0;
		cGreat1p=0;
		cGood1p=0;
		cBad1p=0;
		cMiss1p=0;
		cMaxCombo1p=0;

		cPerfect2p=0;
		cGreat2p=0;
		cGood2p=0;
		cBad2p=0;
		cMiss2p=0;
		cMaxCombo2p=0;

		dwState=0;
		dwState2=0;

		start1=0;

		PressedKey1p[0]=0;
		PressedKey2p[0]=0;
		SelectCurrent=0;

		return;
		
	}

	g_pDDSBack->BltFast(0,0,SelectBack,NULL,DDBLTFAST_NOCOLORKEY);

	curTimer=timeGetTime();

	int i2;

	i2=(int)(curTimer-startTimer)/1000;
	sprintf(s,"%02d", (40 - i2));
	//sprintf(s, "99");

	if(0>=(40-i2))
	{
		if(Selected==7)SelectCurrent=current,PressedKey1p[0]=3;
		else if(Selected==9)SelectCurrent=CSONG[current].Next, PressedKey1p[0]=3;
		else SelectCurrent=current,PressedKey1p[0]=3;
	}
		
	DisplayNumber(560,8,s);
	
	if(Selected==7)
	{
			Screen.top=50-zoom;
			Screen.bottom=50+DISCSIZE_Y+zoom;
			Screen.left=10-zoom;
			Screen.right=10+DISCSIZE_X+zoom;
	}
	else
	{
		Screen.top=50;
		Screen.bottom=50+DISCSIZE_Y;
		Screen.left=10;
		Screen.right=10+DISCSIZE_X;
	}
	
	if(iMove<0)
	{
		if(iMove<=-640)iMove=0;
		else 
		{
			iMove-=8;

			ClpBlt3(10+iMove,50,CSONG[CSONG[CSONG[current].Prev].Prev].DiskImage, &DiscSize,DDBLTFAST_SRCCOLORKEY);
			ClpBlt3(650+iMove,50,CSONG[current].DiskImage, &DiscSize,DDBLTFAST_SRCCOLORKEY);
		}
	}
	else if(iMove>0)
	{
		if(iMove>=640)iMove=0;
		else
		{
			iMove+=8;
		
			ClpBlt3(-630+iMove,50,CSONG[current].DiskImage, &DiscSize, DDBLTFAST_SRCCOLORKEY);
			ClpBlt3(10+iMove,50,CSONG[CSONG[CSONG[current].Next].Next].DiskImage, &DiscSize, DDBLTFAST_SRCCOLORKEY);
		}
	}
	else if(iMove==0)g_pDDSBack->Blt(&Screen,CSONG[current].DiskImage, &DiscSize, DDBLT_KEYSRC,NULL);
	
	if(Selected==9)
	{
			Screen.top=50-zoom;
			Screen.bottom=50+DISCSIZE_Y+zoom;
			Screen.left=330-zoom;
			Screen.right=330+DISCSIZE_X+zoom;
	}
	else
	{
		Screen.top=50;
		Screen.bottom=50+DISCSIZE_Y;
		Screen.left=330;
		Screen.right=330+DISCSIZE_X;
	}
	if(iMove<0)
	{
		if(iMove<=-640)iMove=0;
		else
		{
			iMove-=8;

			ClpBlt3(330+iMove,50,CSONG[CSONG[CSONG[CSONG[current].Prev].Prev].Next].DiskImage, &DiscSize,DDBLTFAST_SRCCOLORKEY);
			ClpBlt3(970+iMove,50,CSONG[CSONG[current].Next].DiskImage, &DiscSize,DDBLTFAST_SRCCOLORKEY);
		}
	}
	else if(iMove>0)
	{
		if(iMove>=640)iMove=0;
		else
		{
			iMove+=8;
		
			ClpBlt3(-310+iMove,50,CSONG[CSONG[current].Next].DiskImage, &DiscSize, DDBLTFAST_SRCCOLORKEY);
			ClpBlt3(330+iMove,50,CSONG[CSONG[CSONG[CSONG[current].Next].Next].Next].DiskImage, &DiscSize,DDBLTFAST_SRCCOLORKEY);
		}
	}
	else if(iMove==0)g_pDDSBack->Blt(&Screen,CSONG[CSONG[current].Next].DiskImage, &DiscSize, DDBLT_KEYSRC,NULL);

	g_pDDSBack->BltFast(0,250,ShiftLeft,NULL,DDBLTFAST_SRCCOLORKEY);
	g_pDDSBack->BltFast(320,250,ShiftRight,NULL,DDBLTFAST_SRCCOLORKEY);
	
/*	if(PressedKey2p[0]==6)
	{
		switch(GameMode)
		{
			case MODE_EASY:GameMode=MODE_HARD;
				break;
			case MODE_HARD:GameMode=MODE_EASY;
				break;
		}
		PressedKey2p[0]=0;
	}

	else if(PressedKey2p[0]==4)
	{
		switch(GameMode)
		{
			case MODE_EASY:GameMode=MODE_HARD;
				break;
			case MODE_HARD:GameMode=MODE_EASY;
				break;
		}
		PressedKey2p[0]=0;
	} */
/*	switch(GameMode)
	{
		case MODE_EASY:DisplayMessage(200,460,"EASY");break;
		case MODE_HARD:DisplayMessage(200,460,"HARD");break;
	}
	switch(Couple)
	{
		case TRUE:DisplayMessage(100,460,"COUPLE MODE");break;
		case FALSE:DisplayMessage(100,460,"SINGLE MODE");break;
	}
*/	
	if(Selected==7)DisplayMessage(200,300,CSONG[current].SongTitle);
	else if(Selected==9)DisplayMessage(200,300,CSONG[CSONG[current].Next].SongTitle);

	if(bModeMirror1p)DrawMode(0,200,HMODE_MIRROR);
	if(bModeNonstep1p)DrawMode(0,240,HMODE_NONSTEP);
	if(bModeSynchro)DrawMode(0,280,HMODE_SYNCHRO);
	if(bModeUnion1p)DrawMode(0,320,HMODE_UNION);
	if(bModeRandom1p)DrawMode(0,360,HMODE_RANDOM);
	if(bModeVanish1p)DrawMode(0,400,HMODE_VANISH);

	if(HighSpeed1p>1)DrawMode(0,160,HMODE_2X);

	if(bModeMirror2p)DrawMode(600,200,HMODE_MIRROR);
	if(bModeNonstep2p)DrawMode(600,240,HMODE_NONSTEP);
	if(bModeUnion2p)DrawMode(600,320,HMODE_UNION);
	if(bModeRandom2p)DrawMode(600,360,HMODE_RANDOM);
	if(bModeVanish2p)DrawMode(600,400,HMODE_VANISH);

	if(HighSpeed2p>1)DrawMode(600,160,HMODE_2X);

	if(CSONG[current].HaveDouble && iMove==0 )g_pDDSBack->BltFast(0,50, DoubleIcon,NULL, DDBLTFAST_SRCCOLORKEY);
	if(CSONG[CSONG[current].Next].HaveDouble && iMove==0 )g_pDDSBack->BltFast(320,50, DoubleIcon, NULL, DDBLTFAST_SRCCOLORKEY);

	if(CSONG[current].HaveCrazy && iMove==0 )g_pDDSBack->BltFast(0,50, CrazyIcon,NULL, DDBLTFAST_SRCCOLORKEY);
	if(CSONG[CSONG[current].Next].HaveCrazy && iMove==0 )g_pDDSBack->BltFast(320,50, CrazyIcon, NULL, DDBLTFAST_SRCCOLORKEY);

	if(CSONG[current].HaveHard && iMove==0 )g_pDDSBack->BltFast(0,50, HardIcon,NULL, DDBLTFAST_SRCCOLORKEY);
	if(CSONG[CSONG[current].Next].HaveHard && iMove==0 )g_pDDSBack->BltFast(320,50, HardIcon, NULL, DDBLTFAST_SRCCOLORKEY);

	if(CSONG[current].HaveEasy && iMove==0 )g_pDDSBack->BltFast(0,50, EasyIcon,NULL, DDBLTFAST_SRCCOLORKEY);
	if(CSONG[CSONG[current].Next].HaveEasy && iMove==0 )g_pDDSBack->BltFast(320,50, EasyIcon, NULL, DDBLTFAST_SRCCOLORKEY);

/*	int DiffL,DiffR;

	DiffL=0,DiffR=0;
	if(CSONG[current].Double_Diff)DiffL=CSONG[current].Double_Diff;
	else if(CSONG[current].Easy_Diff)DiffL=CSONG[current].Easy_Diff;
	else if(CSONG[current].Hard_Diff)DiffL=CSONG[current].Hard_Diff;
	else if(CSONG[current].Crazy_Diff)DiffL=CSONG[current].Crazy_Diff;

	if(CSONG[CSONG[current].Next].Double_Diff)DiffR=CSONG[CSONG[current].Next].Double_Diff;
	else if(CSONG[CSONG[current].Next].Easy_Diff)DiffR=CSONG[CSONG[current].Next].Easy_Diff;
	else if(CSONG[CSONG[current].Next].Hard_Diff)DiffR=CSONG[CSONG[current].Next].Hard_Diff;
	else if(CSONG[CSONG[current].Next].Crazy_Diff)DiffR=CSONG[CSONG[current].Next].Crazy_Diff;

	if(iMove==0)
	for(i=DiffL;;i--)
	{
		if(i==0)break;
		if(i==1)g_pDDSBack->BltFast(285,145,Diff,NULL,DDBLTFAST_SRCCOLORKEY);

		if(i==2)g_pDDSBack->BltFast(283,125,Diff,NULL,DDBLTFAST_SRCCOLORKEY);
		if(i==3)g_pDDSBack->BltFast(283,165,Diff,NULL,DDBLTFAST_SRCCOLORKEY);

		if(i==4)g_pDDSBack->BltFast(281,105,Diff,NULL,DDBLTFAST_SRCCOLORKEY);
		if(i==5)g_pDDSBack->BltFast(281,105,Diff,NULL,DDBLTFAST_SRCCOLORKEY);

		if(i==6)g_pDDSBack->BltFast(279,85,Diff,NULL,DDBLTFAST_SRCCOLORKEY);
		if(i==7)g_pDDSBack->BltFast(279,185,Diff,NULL,DDBLTFAST_SRCCOLORKEY);
	}

	if(iMove==0)
	for(i=DiffR;;i--)
	{
		if(i==0)break;
		if(i==1)g_pDDSBack->BltFast(330,145,Diff,NULL,DDBLTFAST_SRCCOLORKEY);

		if(i==2)g_pDDSBack->BltFast(332,125,Diff,NULL,DDBLTFAST_SRCCOLORKEY);
		if(i==3)g_pDDSBack->BltFast(332,165,Diff,NULL,DDBLTFAST_SRCCOLORKEY);

		if(i==4)g_pDDSBack->BltFast(334,105,Diff,NULL,DDBLTFAST_SRCCOLORKEY);
		if(i==5)g_pDDSBack->BltFast(334,105,Diff,NULL,DDBLTFAST_SRCCOLORKEY);

		if(i==6)g_pDDSBack->BltFast(336,85,Diff,NULL,DDBLTFAST_SRCCOLORKEY);
		if(i==7)g_pDDSBack->BltFast(336,185,Diff,NULL,DDBLTFAST_SRCCOLORKEY);
	}
*/
//	DrawMode(0,0,HMODE_UNION);


	g_pDDSBack->BltFast(210,450,CFont, &lRect, DDBLTFAST_SRCCOLORKEY);

	if(Start1p)
	{
		if(a==0)
		{
			a++;
			if(Start1p && Start2p)Couple=TRUE;
			else Couple=FALSE;
		}
	}
	if(Start2p)
	{
		if(b==0)
		{
			b++;
			if(Start1p && Start2p)Couple=TRUE;
			else Couple=FALSE;
		}
	}

	if(Start1p==FALSE)
	{
		lRect.top=0;
		lRect.left=0;
		lRect.right=220;
		lRect.bottom=23;

		TransAlphaImproved(CFont, g_pDDSBack, 10, 450, lRect, ALPHA, CKey_CFont, 16);
	}
	if(Start2p==FALSE)//DisplayMessageC(320,480-20,"PRESS CENTER STEP");
	{
		lRect.top=0;
		lRect.left=0;
		lRect.right=220;
		lRect.bottom=23;

		TransAlphaImproved(CFont, g_pDDSBack, 410, 450, lRect, ALPHA, CKey_CFont, 16);
	}

	ALPHA += inc;
	if (ALPHA > 256)
	{
		ALPHA = 256;
		inc = -10;
	}
	else if (ALPHA < 0)
	{
		ALPHA = 0;
		inc = 10;
	}

	Flipp();

}

int	ScanHiddenMode1p(void)
{
	static char IntKey1p[10];
	int i;

	if(PressedKey1p[1] || PressedKey1p[3] || PressedKey1p[5] || PressedKey1p[7] || PressedKey1p[9])
		for(i=0;i<7;i++)IntKey1p[i]=IntKey1p[i+1];
	
	if(PressedKey1p[1])IntKey1p[7]='1';
	if(PressedKey1p[3])IntKey1p[7]='3';
	if(PressedKey1p[5])IntKey1p[7]='5';
	if(PressedKey1p[7])IntKey1p[7]='7';
	if(PressedKey1p[9])IntKey1p[7]='9';

	if(strcmp(IntKey1p,"55755595")==0)
	{
		IntKey1p[7]='0';
		return HMODE_2X;
	} // 2배속 입니다.
	else if(strcmp(IntKey1p,"55355755")==0)
	{
		IntKey1p[7]='0';
		return HMODE_4X;
	} // 4배속 입니다.
	else if(strcmp(IntKey1p,"55153555")==0)
	{
		IntKey1p[7]='0';
		return HMODE_8X;
	} // 8배속 입니다.
	else if(strcmp(IntKey1p,"55975315")==0)
	{
		IntKey1p[7]='0';
		return HMODE_VANISH;
	} // 배니쉬 입니다.
	else if(strcmp(IntKey1p,"55159357")==0)
	{
		IntKey1p[7]='0';
		return HMODE_MIRROR;
	} // 미러모드 입니다.
	else if(strcmp(IntKey1p,"51535957")==0)
	{
		IntKey1p[7]='0';
		return HMODE_RANDOM;
	} // 랜덤모드 입니다.
/*	else if(strcmp(IntKey1p,"55979755")==0)
	{
		IntKey1p[7]='0';
		return HMODE_COUPLE;
	} // 커플모드 입니다.
*/	else if(strcmp(IntKey1p,"55797955")==0)
	{
		IntKey1p[7]='0';
		return HMODE_SYNCHRO;
	} // 싱크로 모드입니다.
	else if(strcmp(IntKey1p,"79579579")==0)
	{
		IntKey1p[7]='0';
		return HMODE_NONSTEP;
	} // 논스텝모드 입니다.
/*	else if(strcmp(IntKey1p,"17159395")==0)
	{
		IntKey1p[7]='0';
		return HMODE_NONSTOPDOUBLE;
	} // 논스톱더블 입니다.*/
	else if(strcmp(IntKey1p,"13573159")==0)
	{
		IntKey1p[7]='0';
		return HMODE_UNION;
	} // 유니온모드 입니다.
/*	else if(strcmp(IntKey1p,"95197537")==0)
	{
		IntKey1p[7]='0';
		return HMODE_SELECTALL;
	} // 셀렉트올 입니다. */
	else if(strcmp(IntKey1p,"55555555")==0)
	{
		IntKey1p[7]='0';
		return HMODE_CANCEL;
	} // 캔슬 입니다.

	else if(strcmp(IntKey1p,"55955575")==0)
	{
		IntKey1p[7]='0';
		return HMODE_4DMIX;
	}
	else if(strcmp(IntKey1p,"79513579")==0)
	{
		IntKey1p[7]='0';
		return HMODE_SUDDENR;
	}
	else if(strcmp(IntKey1p,"17931793")==0)
	{
		IntKey1p[7]='0';
		return HMODE_RANDOMS;
	}
	else return HMODE_NONE;
}

int	ScanHiddenMode2p(void)
{
	static char IntKey2p[10];
	int i;

	if(PressedKey2p[1] || PressedKey2p[3] || PressedKey2p[5] || PressedKey2p[7] || PressedKey2p[9])
		for(i=0;i<7;i++)IntKey2p[i]=IntKey2p[i+1];
	
	if(PressedKey2p[1])IntKey2p[7]='1';
	if(PressedKey2p[3])IntKey2p[7]='3';
	if(PressedKey2p[5])IntKey2p[7]='5';
	if(PressedKey2p[7])IntKey2p[7]='7';
	if(PressedKey2p[9])IntKey2p[7]='9';

	if(strcmp(IntKey2p,"55755595")==0)
	{
		IntKey2p[7]='0';
		return HMODE_2X;
	} // 2배속 입니다.
	else if(strcmp(IntKey2p,"55355755")==0)
	{
		IntKey2p[7]='0';
		return HMODE_4X;
	} // 4배속 입니다.
	else if(strcmp(IntKey2p,"55153555")==0)
	{
		IntKey2p[7]='0';
		return HMODE_8X;
	} // 8배속 입니다.
	else if(strcmp(IntKey2p,"55975315")==0)
	{
		IntKey2p[7]='0';
		return HMODE_VANISH;
	} // 배니쉬 입니다.
	else if(strcmp(IntKey2p,"55159357")==0)
	{
		IntKey2p[7]='0';
		return HMODE_MIRROR;
	} // 미러모드 입니다.
	else if(strcmp(IntKey2p,"51535957")==0)
	{
		IntKey2p[7]='0';
		return HMODE_RANDOM;
	} // 랜덤모드 입니다.
/*	else if(strcmp(IntKey2p,"55979755")==0)
	{
		IntKey2p[7]='0';
		return HMODE_COUPLE;
	} // 커플모드 입니다.
*/	else if(strcmp(IntKey2p,"55797955")==0)
	{
		IntKey2p[7]='0';
		return HMODE_SYNCHRO;
	} // 싱크로 모드입니다.
	else if(strcmp(IntKey2p,"79579579")==0)
	{
		IntKey2p[7]='0';
		return HMODE_NONSTEP;
	} // 논스텝모드 입니다.
/*	else if(strcmp(IntKey2p,"17159395")==0)
	{
		IntKey2p[7]='0';
		return HMODE_NONSTOPDOUBLE;
	} // 논스톱더블 입니다.*/
	else if(strcmp(IntKey2p,"13573159")==0)
	{
		IntKey2p[7]='0';
		return HMODE_UNION;
	} // 유니온모드 입니다.
/*	else if(strcmp(IntKey2p,"95197537")==0)
	{
		IntKey2p[7]='0';
		return HMODE_SELECTALL;
	} // 셀렉트올 입니다. */
	else if(strcmp(IntKey2p,"55555555")==0)
	{
		IntKey2p[7]='0';
		return HMODE_CANCEL;
	} // 캔슬 입니다.

	else if(strcmp(IntKey2p,"55955575")==0)
	{
		IntKey2p[7]='0';
		return HMODE_4DMIX;
	}
	else if(strcmp(IntKey2p,"79513579")==0)
	{
		IntKey2p[7]='0';
		return HMODE_SUDDENR;
	}
	else if(strcmp(IntKey2p,"17931793")==0)
	{
		IntKey2p[7]='0';
		return HMODE_RANDOMS;
	}
	else return HMODE_NONE;
}

void DrawMode(int x, int y, int Mode)
{
	RECT	modeRect;

	switch(Mode)
	{
		case HMODE_2X:
		case HMODE_4X:
		case HMODE_8X:
			modeRect.top=0;
			modeRect.left=0;
			modeRect.right=37;
			modeRect.bottom=37;
			break;
		case HMODE_VANISH:
			modeRect.top=0;
			modeRect.left=37;
			modeRect.right=37*2;
			modeRect.bottom=37;
			break;
		case HMODE_MIRROR:
			modeRect.top=0;
			modeRect.left=37*2;
			modeRect.right=37*3;
			modeRect.bottom=37;
			break;
		case HMODE_RANDOM:
			modeRect.top=0;
			modeRect.left=37*3;
			modeRect.right=37*4;
			modeRect.bottom=37;
			break;
		case HMODE_COUPLE:
			modeRect.top=0;
			modeRect.left=37*4;
			modeRect.right=37*5;
			modeRect.bottom=37;
			break;
		case HMODE_SYNCHRO:
			modeRect.top=0;
			modeRect.left=37*5;
			modeRect.right=37*6;
			modeRect.bottom=37;
			break;
		case HMODE_NONSTEP:
			modeRect.top=0;
			modeRect.left=37*6;
			modeRect.right=37*7;
			modeRect.bottom=37;
			break;
		case HMODE_UNION:
			modeRect.top=0;
			modeRect.left=37*7;
			modeRect.right=37*8;
			modeRect.bottom=37;
			break;
		default:
			modeRect.top=0;
			modeRect.left=0;
			modeRect.right=0;
			modeRect.bottom=0;
			break;
	}

	g_pDDSBack->BltFast(x,y, ModeIcon, &modeRect, DDBLTFAST_SRCCOLORKEY);
}
