/********************************************************************/
/*          main.cpp                                                */
/*            Source of EZ3Loader                                   */
/*                                                                  */
/*          Copyright (C) 2003-2004            USTC Inc.            */
/********************************************************************/
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"

#include "agb.h"
#include "string.h"
#include "keypad.h"
#include "BGfunc.h"
#include "screenmode.h"
#include "cmddefine.h"
#include "global.h"

#include "shell.h"
#include "inram.h"
#include "viewtext.h"
#include "icons_obj.h"
#include "savermanage.h"
#include "lang.h"
#include "nandop.h"
#include "fat16.h"
#include "hard.h"  
/*-------------------- Global Variables ----------------------------*/
///**************�жϴ����ֵ�ȫ�ֱ���**********
u32     IntrMainBuf[0x200];         // Buffer for interrupt main routine
extern "C" void 	intr_main(void);
typedef void (*IntrFuncp)(void);
//const char ezload_ver[] = "Ver1.21A";
extern BYTE  SDorNand ;
extern u32 version ;
res_struct res;  
vu8* HZKLINE;

extern int gameMine();
//int appleBrick();
extern u8* DICTLINE;
//extern u32	 glTotalsize ;
//extern u32  glCursize ;
extern u16  glLoadsaver ;
extern int VblankCount;
extern char CurrentDisk[32];	//��ǰ���ڴ���
extern char CurrentPath[128];		//��ǰ·��
extern char CurrentFileName[128]; //��ǰѡ���ļ���
extern u16  glExecFileTxt ;
extern u16	 CurrentIndexSelect ;		//��ѡ�ļ�������
extern u16	 CurrentIndexView ;			//��ҳ���ļ���ʼ������
u32  		gl_currentpage ;
u32 		gl_norOffset;				//Nor���п�ʼ�ĵط�
u32 		gl_norsize;					//NOR Flash �Ĵ�С
int 		gl_norgamecounter=0;
EZ4_CARTTYPE g_EZ4CardType;

//*==================================================================*/
//*                      timer1 �жϴ�������                                */
//*==================================================================*/
void time1Intr(void)
{
   *(vu16 *)REG_IF	   = TIMER1_INTR_FLAG ;
//   DrawPic(progress[g_time1],40,56,160,48,0,0,1);
	
//	sprintf(tt,"��ǰ������ʾ  %08x/%x8x",glCursize,glTotalsize);
//	Clear(60+54,40+30,62,40,0x7FBB,1);
//	DrawHZTextRect(tt,0,60+54,40+30,62,40,14,0,2,1);	//д��Ϣ
/*
   g_time1 ++;
   if(g_time1 >= 11)
   	g_time1 = 0 ;
*/
}

#define TM_SELCONT 5 
void TimeSetting()
{
//������
	u8 bcreate = 1,brefresh=0,bchangenum=0;
	u8 x,y ;
	u32 timer = 0, rtimer = 0, ti = 0, holdt = 0;

	u16 *pCol =(u16*)(GetRomFileByName("\\.shell\\bmp\\","Time_title.bmp")+sizeof(RomFile)+8);
	CLOCK_TIME clocktime ;	
	char year[6],mon[4],day[4],hour[4],minute[4];
	u8  tm_sel = 0 , tm_selold = 99 ;
	
	keypad keys ;
	bcreate = 1 ;
	x=120;y=48 ;
	while(1)
	{
		VBlankIntrWait();                           // Complete V Blank interrupt
		ti++;
		rtimer=timer;
		if(bcreate)
		{//�������������Լ���ʾʱ��
			Clear(0,0,240,160,*pCol,0);
			DrawFilePic("Time_title.bmp",22,8,172,32,0,0x7FFF,0);
			DrawFilePic("Time_col.bmp",42,48,48,92,0,0x7FFF,0);
			DrawFilePic("ok.bmp",184,140,24,14,1,0x7FFF,0);	//��OK
			DrawFilePic("x.bmp",214,140,12,12,1,0x7FFF,0);	//��X
			GetTime(&clocktime);
			sprintf(year,"%4d",2000+clocktime.year);
			DrawASCText16(year,0, x, y ,0 , 0 );
			DrawASCText16(year,0, x+1, y+1 ,0 , 0 );
			sprintf(mon,"%02d",clocktime.month);
			DrawASCText16(mon,0, x+8, y+18 ,0 , 0 );
			DrawASCText16(mon,0, x+9, y+19 ,0 , 0 );
			sprintf(day,"%02d",clocktime.data);
			DrawASCText16(day,0, x+8, y+36 ,0 , 0 );
			DrawASCText16(day,0, x+9, y+37 ,0 , 0 );
			sprintf(hour,"%02d",clocktime.hour);
			DrawASCText16(hour,0, x+8, y+54 ,0 , 0 );
			DrawASCText16(hour,0, x+9, y+55 ,0 , 0 );
			sprintf(minute,"%02d",clocktime.minute);
			DrawASCText16(minute,0, x+8, y+72 ,0 , 0 );
			DrawASCText16(minute,0, x+9, y+73 ,0 , 0 );
			syncVVram();
			bcreate = 0 ;
			brefresh = 1 ;
			
		}
		if(brefresh) //����ϴ�ѡ���ػ�
		{
			if(tm_selold == tm_sel)
				continue ;
			else
			{
				if(tm_selold <TM_SELCONT)
				{//�����
					Clear(x-20,y+18*tm_selold,82,18,*pCol,1);
					switch(tm_selold)
					{
					case 0 :
						sprintf(year,"%4d",2000+clocktime.year);
						DrawASCText16(year,0, x, y ,0 , 1 );
						DrawASCText16(year,0, x+1, y+1 ,0 , 1 );
						break;
					case 1:
						sprintf(mon,"%02d",clocktime.month);
						DrawASCText16(mon,0, x+8, y+18 ,0 , 1 );
						DrawASCText16(mon,0, x+9, y+19 ,0 , 1 );
						break;
					case 2:
						sprintf(day,"%02d",clocktime.data);
						DrawASCText16(day,0, x+8, y+36 ,0 , 1 );
						DrawASCText16(day,0, x+9, y+37 ,0 , 1 );
						break;
					case 3:
						sprintf(hour,"%02d",clocktime.hour);
						DrawASCText16(hour,0, x+8, y+54 ,0 , 1 );
						DrawASCText16(hour,0, x+9, y+55 ,0 , 1 );
						break;
					case 4:
						sprintf(minute,"%02d",clocktime.minute);
						DrawASCText16(minute,0, x+8, y+72 ,0 , 1 );
						DrawASCText16(minute,0, x+9, y+73 ,0 , 1 );
						break;
					}
				}
				//������ָʾͼƬ
				DrawFilePic("arrowL.bmp",x-20,y+18*tm_sel,12,12,1,0x7FFF,1);	//��L
				DrawFilePic("arrowR.bmp",x+48,y+18*tm_sel,12,12,1,0x7FFF,1);	//��R
			}			
			brefresh = 0 ;
		}
		if(bchangenum)
		{
			Clear(x,y+18*tm_sel,40,18,*pCol,1);
			switch(tm_sel)
			{
			case 0 :
				sprintf(year,"%4d",2000+clocktime.year);
				DrawASCText16(year,0, x, y ,0 , 1 );
				DrawASCText16(year,0, x+1, y+1 ,0 , 1 );
				break;
			case 1:
				sprintf(mon,"%02d",clocktime.month);
				DrawASCText16(mon,0, x+8, y+18 ,0 , 1 );
				DrawASCText16(mon,0, x+9, y+19 ,0 , 1 );
				break;
			case 2:
				sprintf(day,"%02d",clocktime.data);
				DrawASCText16(day,0, x+8, y+36 ,0 , 1 );
				DrawASCText16(day,0, x+9, y+37 ,0 , 1 );
				break;
			case 3:
				sprintf(hour,"%02d",clocktime.hour);
				DrawASCText16(hour,0, x+8, y+54 ,0 , 1 );
				DrawASCText16(hour,0, x+9, y+55 ,0 , 1 );
				break;
			case 4:
				sprintf(minute,"%02d",clocktime.minute);
				DrawASCText16(minute,0, x+8, y+72 ,0 , 1 );
				DrawASCText16(minute,0, x+9, y+73 ,0 , 1 );
				break;
			}
			bchangenum = 0 ;
		}
		keys.update();
		if ( (keys.press(KEY_UP)&&(holdt=ti))|| (keys.hold(KEY_UP)&&(ti-holdt>50)) )//�����ƶ����
		{
			if((tm_sel%TM_SELCONT)==0)
				continue;
			if((tm_sel%TM_SELCONT)>0)
			{
				tm_selold = tm_sel;
				tm_sel--; 
				brefresh = 1;
			}
			
		}
		if ( (keys.press(KEY_DOWN)&&(holdt=ti))|| (keys.hold(KEY_DOWN)&&(ti-holdt>50)) )//�����ƶ����
		{
			if((tm_sel%TM_SELCONT)==TM_SELCONT-1)
				continue;
			if(tm_sel==TM_SELCONT-1)
				continue;
			if((tm_sel%TM_SELCONT)<TM_SELCONT-1)
			{
				tm_selold = tm_sel;
				tm_sel++; 
				brefresh = 1;
			}
		}		
		if ( (keys.press(KEY_LEFT)&&(holdt=ti))|| (keys.hold(KEY_LEFT)&&(ti-holdt>50)) )//�����ƶ����
		{
			int sel = (tm_sel%TM_SELCONT);
			if(sel==0)
			{
				if(clocktime.year<=0)
					continue ;
				clocktime.year -- ;		
			}		
			else if(sel == 1)//month
			{
				if(clocktime.month<=1)
					continue ;
				clocktime.month -- ;				
			}
			else if(sel == 2)//day
			{
				if(clocktime.data<=1)
					continue ;
				clocktime.data -- ;				
			}
			else if(sel == 3)//hour
			{
				if(clocktime.hour<=0)
					continue ;
				clocktime.hour -- ;				
			}
			else if(sel == 4)//minute
			{
				if(clocktime.minute<=0)
					continue ;
				clocktime.minute -- ;				
			}
			bchangenum = 1 ;
		}
		if ( (keys.press(KEY_RIGHT)&&(holdt=ti))|| (keys.hold(KEY_RIGHT)&&(ti-holdt>50)) )//�����ƶ����
		{
			int sel = (tm_sel%TM_SELCONT);
			if(sel==0)
			{
				if(clocktime.year>=99)
					continue ;
				clocktime.year ++ ;				
			}
			else if(sel==1)//month
			{
				if(clocktime.month>=12)
					continue;
				clocktime.month ++ ;				
			}
			else if(sel==2)//day
			{
				if(clocktime.data>=31)
					continue ;
				clocktime.data ++ ;				
			}
			else if(sel==3)//hour
			{
				if(clocktime.hour>=23)
					continue ;
				clocktime.hour ++ ;	
			}
			else if(sel==4)//minute
			{
				if(clocktime.minute>=59)
					continue ;
				clocktime.minute ++ ;
			}
			bchangenum = 1 ;
		}
		if(keys.release(KEY_B))
		{
			break;
		}
		if(keys.release(KEY_A))
		{
			Set24Hour();
			SetTime(&clocktime);
			break;
		}
	}
}

void LangSetting()
{
	int x,y,sel;
	keypad key1 ;
	x = 60;
	y = 40;
	DrawFilePic("itemdisc.bmp",x,y,128,80,1,0x7FFF,1);	//������
    DrawIcon(x+13,y+26,7,1);					//��ͼ��
	DrawFilePic("ok.bmp",x+7,y+58,24,14,1,0x7FFF,1);	//��OK
	DrawFilePic("x.bmp",x+34,y+58,12,12,1,0x7FFF,1);	//��X
	
	DrawHZTextRect("����",0,x+64,y+26,62,56,14,0,2,1);	//д��Ϣ
	DrawHZTextRect("English",0,x+64,y+46,62,56,14,0,2,1);	//д��Ϣ
	//DrawHZTextRect("France",0,x+64,y+46,62,56,14,0,2,1);	//д��Ϣ
	
	DrawFilePic("arrowR.bmp",x+52,y+26,12,12,1,0x7FFF,1);	//�������ͷ
	sel=0;
	while(1)
	{
		VBlankIntrWait();                           // Complete V Blank interrupt
		key1.update();
		if (key1.press(KEY_UP))
		{
			DrawFilePic("arrowR.bmp",x+52,y+26,12,12,1,0x7FFF,1);	//�������ͷ
			Clear(x+52,y+46,12,12,0x7FBB,1);
			sel = 0 ;
		}
		if(key1.press(KEY_DOWN))
		{
			DrawFilePic("arrowR.bmp",x+52,y+46,12,12,1,0x7FFF,1);	//�������ͷ
			Clear(x+52,y+26,12,12,0x7FBB,1);
			sel =1 ;
		}
		if(key1.release(KEY_A))
		{//�趨
			if(sel)
			{//Ӣ��
				u16 ulan=1252 ;
				LoadEnglish();
				WriteSram(0x0E0081F0,(u8*)&ulan,2);
			}
			else
			{//����
				u16 ulan=936 ;
				LoadChinese();
				WriteSram(0x0E0081F0,(u8*)&ulan,2);
			}
			break;
		}
		if(key1.release(KEY_B))
		{
			break;
		}
	}
	
}

void SaverSetting()
{
	int x,y,sel;
	keypad key1 ;
	SAVEFILEINFO saverinfo ;
	u16 lan = CheckLanguage();

	SetRampage(0);	
	ReadSram(_FileInfoSaver,(u8*)&saverinfo,sizeof(SAVEFILEINFO));

	x = 60;
	y = 40;
	DrawFilePic("itemdisc.bmp",x,y,128,80,1,0x7FFF,1);	//������
    	DrawIcon(x+13,y+26,7,1);					//��ͼ��
	DrawFilePic("ok.bmp",x+7,y+58,24,14,1,0x7FFF,1);	//��OK
	DrawFilePic("x.bmp",x+34,y+58,12,12,1,0x7FFF,1);	//��X
	if(lan == 936)
	{
		DrawHZTextRect("�Զ��浵",0,x+64,y+26,62,56,14,0,2,1);	//д��Ϣ
		DrawHZTextRect("ǿ�ƴ浵",0,x+64,y+46,62,56,14,0,2,1);	//д��Ϣ
	}
	else
	{
		DrawHZTextRect("Auto Saving",0,x+64,y+26,62,56,14,0,2,1);	//д��Ϣ
		DrawHZTextRect("Obliged Saving",0,x+64,y+46,62,56,14,0,2,1);	//д��Ϣ
	}
	if(saverinfo.saverforce)
		sel=saverinfo.saverforce -1;
	else
	{
		sel = 0 ;
		saverinfo.saverforce = 1 ;
	}
	DrawFilePic("arrowR.bmp",x+52,y+26+20*sel,12,12,1,0x7FFF,1);	//�������ͷ
	while(1)
	{
		VBlankIntrWait();                           // Complete V Blank interrupt
		key1.update();
		if (key1.press(KEY_UP))
		{
			DrawFilePic("arrowR.bmp",x+52,y+26,12,12,1,0x7FFF,1);	//�������ͷ
			Clear(x+52,y+46,12,12,0x7FBB,1);
			sel = 0 ;
		}
		if(key1.press(KEY_DOWN))
		{
			DrawFilePic("arrowR.bmp",x+52,y+46,12,12,1,0x7FFF,1);	//�������ͷ
			Clear(x+52,y+26,12,12,0x7FBB,1);
			sel =1 ;
		}
		if(key1.release(KEY_A))
		{//�趨
			saverinfo.saverforce = sel +1  ;
			WriteSram(_FileInfoSaver,(u8*)&saverinfo,sizeof(SAVEFILEINFO));
			
			break;
		}
		if(key1.release(KEY_B))
		{
			break;
		}
	}
	
}
//*==================================================================*/
//*                      Main Routine                                */
//*==================================================================*/
void InitialGBA(void)
{
	SetMode (MODE_3 | BG2_ENABLE | OBJ_MAP_1D | OBJ_ENABLE);
//�����ж�������
    DmaCopy(3, intr_main, IntrMainBuf, sizeof(IntrMainBuf), 32);// Interrupt main routine set
    *(vu32 *)INTR_VECTOR_BUF = (vu32 )IntrMainBuf;
//������ʾ�ж�
    *(vu16 *)REG_IME   = 1;                           // Set IME
    *(vu16 *)REG_IE    = V_BLANK_INTR_FLAG            // Permit V blank interrupt
                       | CASSETTE_INTR_FLAG	          // Permit cassette interrupt
                       | TIMER1_INTR_FLAG ;
                       
   	*(vu16 *)REG_IF	   = TIMER1_INTR_FLAG ;                    
    *(vu16 *)REG_STAT  = STAT_V_BLANK_IF_ENABLE;
    //*(vu16 *)REG_WAITCNT = CST_ROM0_1ST_3WAIT|CST_ROM0_2ND_1WAIT;
    //*(vu16 *)REG_WAITCNT = CST_ROM0_1ST_3WAIT|CST_ROM0_2ND_1WAIT|CST_PREFETCH_ENABLE|CST_SRAM_2WAIT;
    *(vu16 *)REG_WAITCNT = CST_PREFETCH_ENABLE|CST_SRAM_2WAIT;
    
    //��ԭpage
    SetRampage(0);
	
}

void ShowTimeRightbotom(u8 direct)
{
	char time[32];
	CLOCK_TIME clocktime ;	
	if(SDorNand)
		sprintf(time,"[SD]V%x.%03x",(version&0xFFFF)>>12,(version&0xFFF));
	else
		sprintf(time,"[ND]V%x.%03x",(version&0xFFFF)>>12,(version&0xFFF));
	
	DrawHZText12(time,0,167,3,0,direct);	//д��Ϣ
	DrawHZText12(time,0,166,2,0x7FFF,direct);	//д��Ϣ

/*
	ClearWithBG(178,135,62,25,res.res_DESKBG,direct);
	GetTime_Orignal(&clocktime);
	sprintf(time,"20%01d%01d-%01d%01d-%01d%01d",
				(clocktime.year>>4),(clocktime.year&0xF),
				(clocktime.month>>4),(clocktime.month&0xF),
				(clocktime.data>>4),(clocktime.data&0xF));
	DrawHZText12(time,0,180,136,0,direct);	//д��Ϣ
	DrawHZText12(time,0,179,135,0x7FFF,direct);	//д��Ϣ
	sprintf(time,"%01d%01d:%01d%01d:%01d%01d",						
				(clocktime.hour>>4),(clocktime.hour&0xF),
				(clocktime.minute>>4),(clocktime.minute&0xF),
				(clocktime.second>>4),(clocktime.second&0xF));
	DrawHZText12(time,0,186,148,0,direct);	//д��Ϣ
	DrawHZText12(time,0,185,147,0x7FFF,direct);	//д��Ϣ
	*/
}
//
int ShowDesktop()
{
	u32 timer = 0, rtimer = 0, ti = 0, holdt = 0;
	u32 desktop_flag = DESKTOP_FLAG_CREAT;
	u8 desktop_icon_i=0, desktop_icon_i_old=99;
	u8 needRefresh = 1, showhint = 0, needfade=1;
	u16 desktop_act_t=1;//���ڿ������涯̬Ч����ʱ�����
	u16 ax=0,ay=0;
	
	keypad keys ;
	
	FadeInOut(1,0,90);//	��������
	
	while(1) 
	{
		VBlankIntrWait();                           // Complete V Blank interrupt
		ti++;
		rtimer=timer;
		if(desktop_flag == DESKTOP_FLAG_CREAT)
		{
			//��ʾ����
			DrawBG((u16*)res.res_DESKBG,0);

			for(u8 i=0;i<ICON_COUNT;i++)
    			DrawIcon(DESK_ICON_X0+(i/ICON_FLAG_YCOUNT)*DESK_ICON_DX,DESK_ICON_Y0+DESK_ICON_DY*(i%ICON_FLAG_YCOUNT),desk_icon_list[i],0);
            desktop_flag = DESKTOP_FLAG_UPD;
			needRefresh = 1;
		}
        if(needRefresh)
        {
   			ShowTimeRightbotom(0);
            syncVVram();
            needRefresh = 0;
        }
		if(desktop_flag == DESKTOP_FLAG_UPD)		//������������ʾ������ˢ��
		{
			if(showhint == 0)//���û����ʾ���ڣ���ʾ���ͷ
			{
					DrawFilePic("arrowL.bmp",DESK_ICON_X0 + 23 + (desktop_icon_i/ICON_FLAG_YCOUNT)*DESK_ICON_DX,DESK_ICON_Y0+5+(desktop_icon_i%ICON_FLAG_YCOUNT)*DESK_ICON_DY,12,12,1,0x7FFF,1);	//�������ͷ
			}
			else			//�����������ʾ���ڣ���ʾ�Ҽ�ͷ
			{
				desktop_act_t = 1;
				desktop_icon_i_old = desktop_icon_i;
				ax = DESK_ICON_X0+24+16 + (desktop_icon_i/ICON_FLAG_YCOUNT)*DESK_ICON_DX;	//��ʾ����x
				ay = DESK_ICON_Y0+2+(desktop_icon_i%ICON_FLAG_YCOUNT)*DESK_ICON_DY;			//��ʾ����y
				if(ay>78)
					ay = 78;

				DrawFilePic("arrowR.bmp",ax-17,DESK_ICON_Y0+5+(desktop_icon_i%ICON_FLAG_YCOUNT)*DESK_ICON_DY,12,12,1,0x7FFF,1);	//�������ͷ
				CreateWindow((char*)gl_hint_title[desktop_icon_i],(char*)gl_hint_content[desktop_icon_i],ax,ay,desk_icon_list[desktop_icon_i]);
				desktop_act_t = 1;		
			}
			desktop_act_t = 0;
			desktop_flag = DESKTOP_FLAG_NULL;
		}
		if(needfade)
		{
			FadeInOut(1,0,50);
			needfade=0;
		}
		keys.update();
		if ( (keys.press(KEY_UP)&&(holdt=ti))|| (keys.hold(KEY_UP)&&(ti-holdt>50)) )//�����ƶ����
		{
			if((desktop_icon_i%ICON_FLAG_YCOUNT)==0)
				continue;
			if((desktop_icon_i%ICON_FLAG_YCOUNT)>0)
			{
				desktop_icon_i_old = desktop_icon_i;
				desktop_icon_i--; 
				desktop_flag = DESKTOP_FLAG_UPD;
				needRefresh = 1;
			}
			
		}
		if ( (keys.press(KEY_DOWN)&&(holdt=ti))|| (keys.hold(KEY_DOWN)&&(ti-holdt>50)) )//�����ƶ����
		{
			if((desktop_icon_i%ICON_FLAG_YCOUNT)==ICON_FLAG_YCOUNT-1)
				continue;
			if(desktop_icon_i==ICON_COUNT-1)
				continue;
			if((desktop_icon_i%ICON_FLAG_YCOUNT)<ICON_FLAG_YCOUNT-1)
			{
				desktop_icon_i_old = desktop_icon_i;
				desktop_icon_i++; 
				desktop_flag = DESKTOP_FLAG_UPD;
				needRefresh = 1;
			}
		}
		if ( (keys.press(KEY_LEFT)&&(holdt=ti))|| (keys.hold(KEY_LEFT)&&(ti-holdt>50)) )//�����ƶ����
		{
			if(desktop_icon_i>ICON_FLAG_YCOUNT-1)
			{
				desktop_icon_i_old = desktop_icon_i;
				desktop_icon_i-=ICON_FLAG_YCOUNT; 
				desktop_flag = DESKTOP_FLAG_UPD;
				needRefresh = 1;
			}
			
		}
		if ( (keys.press(KEY_RIGHT)&&(holdt=ti))|| (keys.hold(KEY_RIGHT)&&(ti-holdt>50)) )//�����ƶ����
		{
			if(desktop_icon_i+ICON_FLAG_YCOUNT<ICON_COUNT)
			{
				desktop_icon_i_old = desktop_icon_i;
				desktop_icon_i+=ICON_FLAG_YCOUNT; 
				desktop_flag = DESKTOP_FLAG_UPD;
				needRefresh = 1;
			}
			
		}
		if ( keys.hold(KEY_L) && keys.hold(KEY_R)) 
		{
			ScreenSaver("ezpdalogo.bmp");
			desktop_flag = DESKTOP_FLAG_CREAT;
			needfade = 1;
		}
		if (keys.release(KEY_A))
		{	
			if(showhint == 1)
			{
				//////////////////////////////////////////////������Ϣ����////////////////
				//////////////////////////////////////////////������Ϣ����////////////////
				if(desktop_icon_i == 0)//��Դ������
				{
					return PROC_ENTERDISKSHELL;
				}
				if(desktop_icon_i == 1)//���̵����
				{
					char tmp[64];
					int isset=0,selsaver=0,get=0;
					FILE_SAVE_INFO fsi;
					u8 *p;
					RomFile* rp;
					//�������ؽ���
					
stillinsaver:
					isset = EnterSaver(0,isset) ;
					switch(isset)
					{
					case 0:
						selsaver = SAVETXTPOS ;
						break;
					case 1:
						selsaver = _txtSaver1 ;
						break;
					case 2:
						selsaver = _txtSaver2 ;
						break;
					case 3:
						selsaver = _txtSaver3 ;
						break;
					case 4:
						selsaver = _txtSaver4 ;
						break;
					default :
						selsaver = SAVETXTPOS ;						
					}
					if(isset>=0)
					{
						load(selsaver,(u8*)&fsi,sizeof(FILE_SAVE_INFO));
						if(fsi.flg!='eztx')
							goto stillinsaver ;
					}
					else
					{						
						needfade = 1;
						desktop_flag = DESKTOP_FLAG_CREAT;
						needRefresh = 1;
						continue ;
					}
					
					load(selsaver,(u8*)&fsi,sizeof(FILE_SAVE_INFO));
					//dbgPrint("pos:%d; pi:%d",fsi.pos,fsi.pos2);
					//sleep(1500);	
					sprintf(CurrentDisk,"EZ-Disk");	//��ǰ���ڴ���
					sprintf(CurrentPath,"%s",fsi.path);		//��ǰ·��
					sprintf(CurrentFileName,"%s",fsi.fname); //��ǰѡ���ļ���
					//�����Ƿ��е�ǰ���ļ�������м�����û�г��ִ���
					sprintf(tmp,"%s%s",fsi.path,fsi.fname);
					fat_init(1);
					get = fat_open(tmp);
					if(get<0)
					{
						showhint = 0;
						desktop_flag = DESKTOP_FLAG_CREAT;
						needRefresh = 1;
						continue ;
					}
					fat_close_withouwrit(get);
					
					CurrentIndexSelect = fsi.fi ;
					CurrentIndexView = fsi.viewfi ;
					glExecFileTxt = EXEC_TXT ;
					if((CurrentPath[0]==0)||(CurrentFileName[0]==0))
					{
						showhint = 0;
						needRefresh = 1;
						continue ;
					}
					
					return PROC_ENTERDISKSHELL ;
				}
/*				if(desktop_icon_i == 2)	//Ӣ��ѧϰ
				{
					return PROC_EZWORD ;
				}*/
				if(desktop_icon_i == 2)	//ɨ����Ϸ
				{
					u8 gameover=0;
					DrawFilePic("mineBG.bmp",0,0,240,160,0,0,1);	//������
					while(gameover==0)
					{
						while(1){
							sleep(15);	keys.update();

							if(keys.press(KEY_START))
								break;
							if(keys.release(KEY_B))
							{
								gameover=1;			
								break;
							}
						}
						if(gameover)
						{
							needfade = 1;
							desktop_flag = DESKTOP_FLAG_CREAT;
							needRefresh = 1;
							FadeInOut(0,0,50);	//����
							break;
						}
						gameMine();
						FadeStatus(1,0);
						SetMode (MODE_3 | BG2_ENABLE | OBJ_MAP_1D | OBJ_ENABLE);
						DrawFilePic("mineBG.bmp",0,0,240,160,0,0,1);	//������
						FadeInOut(1,0,50);
					}
				}
				/*
				if(desktop_icon_i == 4)	//ש����Ϸ
				{
					FadeInOut(0,0,50);	//����
					appleBrick();
					SetMode (MODE_3 | BG2_ENABLE | OBJ_MAP_1D | OBJ_ENABLE);
					needfade = 1;
					desktop_flag = DESKTOP_FLAG_CREAT;
					needRefresh = 1;
				}
				*/
				if(desktop_icon_i == 3)	//����
				{
					int func_i ;
					while(1)
					{
						func_i = ShowWin_Menu("ezoption.bmp","","",(char**)gl_option_set,2,0);
						if(func_i<0)//
						{
							FadeInOut(0,1,30);//	�������
							break;
						}
						else if(func_i==0)
						{//��������
							LangSetting();
						}
						else if(func_i>0)
						{//�浵��ʽ����
							SaverSetting();
						}
					}
					needfade = 1;
					desktop_flag = DESKTOP_FLAG_CREAT;
					needRefresh = 1;
				}
				if(desktop_icon_i == 4)	//����
				{
					char path[MAX_PATH],fname[MAX_PATH];
					u8 *p;
					RomFile* rp;
					
					strcpy(path,"\\.shell\\");
					strcpy(fname,"help.txt");
					p = GetRomFileByName(path,fname);
					rp = (RomFile*)p;
					viewText(p+sizeof(RomFile),rp->size,0,0);
					FadeInOut(0,0,50);	//����
					needfade = 1;
					desktop_flag = DESKTOP_FLAG_CREAT;
				}
			}
			else
			{
				showhint = 1;
				desktop_flag = DESKTOP_FLAG_UPD;
				needRefresh = 1;
			}
		}
		if (keys.press(KEY_B))
		{
			showhint = 0;
			desktop_flag = DESKTOP_FLAG_UPD;
			needRefresh = 1;
		}

		timer++;
		if(!(timer%20))
		{//��ʾʱ��
			ShowTimeRightbotom(1);
		}
	}
}

#include "sdopera.h"
/*==================================================================*/
/*                      Main Routine                                */
/*==================================================================*/

int main(void)
{
	u32 timer = 0, rtimer = 0, ti = 0, holdt = 0;
	int shell_return = 0 ;
	int k = 0 , skip = 1 ;
	bool	  ret = 0 ;
	char *pLoader = (char*)0x08040000 ;
	gl_currentpage = 0x8002 ;	//temp lichuan old value 0x8002;


	//open the PSRAM write
	OpenRamWrite();

	InitialGBA();
	initResource();
	
	keypad keyy ;
	keypad *keys = &keyy ;
	
	
	glLoadsaver = 0 ; //����Ƿ��Ѿ������˴浵
	HZKLINE =  GetRomFileByName("\\.shell\\","hzk.dat")+sizeof(RomFile);
	//һ��Ҫ���ã���ȷ����ʲô����

	//SD_Test();
	//while(1);
//SD����
ezcartridge:
	OpenWrite();
	CheckEz4Card();
	EnableNand8bit();
	NAND_Reset();
	k = NAND_ReadID();
	ret = IsValidID(k);
	DisableNandbit();
	CloseWrite();
	SDorNand = 0 ;
	
	if(!ret)
	{
		SD_Enable();
		ret = SD_initial() ;
		SD_Disable();
		if(ret)
		{
			SDorNand = 1 ;
		}
	}
	if(!ret)
	{
		CreateWindow("info","can't find Disk",60,40,1);
		while(1);
	}
	
	//���������ļ�
	k = fat_init(1); //��ʼ�����ļ�ʧ��

	{
		//CreateWindow("info","fat_init OK",60,40,1);
	}
	
	if(k)	goto welcome ;
	k=fat_open("\\saver\\passme2.sav");	
	if(k>=0)
	{//�����ļ�,���ҵ������ļ���׼������ram,passme2
		_stat stat;
		int filesize ;
		u8* pcache = (u8*)0x02020000 ;
		fat_read(k,pcache,0x20000);
		fat_get_stat("\\saver\\passme2.sav",&stat);
		filesize =  stat.FileSize ;
		CreateWindow("info","add passme2.sav >>>",60,40,1);
		fat_read(k,pcache,filesize);	
		//д��浵
		SetRampage(0);
		WriteSram(_SRAMSaver,pcache,filesize);
		fat_close_withouwrit(k);
		CreateWindow("info","add passme2.sav OK",60,40,1);
	}
	
welcome:	//��ӭ����
	sleep(1000);
	DrawFilePic("ezpdalogo.bmp",0,0,240,160,0,0,1);

	FadeInOut(1,0,90);//	��������
	for(k=0;k<10;k++)
		VBlankIntrWait();                           // Complete V Blank interrupt
		
	
//����	
	skip = Checkaladdin();
	u16 lan = CheckLanguage();
	if(lan == 936)//����
	{
		LoadChinese();
	}
	else if(lan == 1252 )//Ӣ��
	{
		LoadEnglish();
	}
	else
	{//ѡ������
		LangSetting();
	}
	
	SAVEFILEINFO saverinfo ;
	SetRampage(0);	
	ReadSram(_FileInfoSaver,(u8*)&saverinfo,sizeof(SAVEFILEINFO));
	if(saverinfo.saverforce ==2)
	{
			CreateWindow("info",gl_checkingsaver,60,40,1);
			k = CheckSram(); //��Ҫ����
			if(k)
			{
				SaveCurrentSaverToFlash();
			}
	}
	else
	{
		CreateWindow("info","press L",60,40,1);
		
		if(!skip)
		{
			skip = 1 ;
			CreateWindow(gl_tip,gl_lskip,60,40,1);
			for(k=0 ; k<80 ; k++) 
			{
				VBlankIntrWait();                           // Complete V Blank interrupt
				keys->update();
				if(keys->hold(KEY_L))
				{
					skip = 0 ;
					break;
				}
			}
		}
		if(skip)
		{
			k = CheckSram(); //��Ҫ����
			if(k)
			{
				SaveCurrentSaverToFlash();
			}
		}
	}
	shell_return = PROC_DESKTOP ;
	while(1) 
	{
		sleep(15);
		ti++;
		rtimer=timer; 
		switch (shell_return)
		{
		case PROC_DESKTOP:
			shell_return = ShowDesktop() ;
			break;
		case PROC_ENTERDISKSHELL:
	        	shell_return = runEz3DiskShell();
	        	break;
	        case PROC_GOLDENSELECT :
	        	shell_return = EnterCheatCodeSelect();
	        	break;
	        case PROC_SOFTRESET:
	        	softrest();
	        	break;
	        case PROC_HARDRESET:
	        	hardrest();
	        	break;
	        case PROC_LOADSAVER:	
	        	shell_return = LoadSaver();
	        	break;
	        case PROC_WRITESAVEREX :
	        	shell_return = WriteSaverEx();
	        	break;
	        case PROC_WRITE2NORFLASH :
	        	shell_return = EnterWriteGBAtoNor() ;
	        	break;
	        case PROC_FORMATNORFLASH:
	        	shell_return = FormatNor() ;
	        	break;
	         default:
			;
		}//runShell();

		timer++;

	////////////////////////////////////////////////////////////////////////////////
	}//while
	
}

