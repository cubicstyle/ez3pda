// Donald Peeke-Vout
// bg.cpp
//  My first adventure with tile backgrounds! Yay!  Looks like a bad
//  case of parallax scrolling to me...


#include "agb.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"

#include "string.h"
#include "keypad.h"
#include "BGfunc.h"
//#include "spritemgr256.h"
#include "screenmode.h"
#include "cmddefine.h"
#include "shell.h"
#include "global.h"
#include "wordglobal.h"
#include "wordfunc.h"
#include "wordmenu.h"


#ifdef _MAIN_ROM_
res_struct res;
#endif


u8* DICTLINE;
extern u8* HZKLINE;
u16 WORDNUM,CIKUNUM;



///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef _MAIN_ROM_
int main(void) 
#else
int ezWord()
#endif
{
	u32 ti = 0, holdt = 0;
	int rv;
	int func_i=0, ciku_i=0, part_i=0;
	u8 refresh = 1;
	u16 wordi=0;

	FadeInOut(0,1,0);//	˲��ת��
	//initResource();
	//HZKLINE =  GetRomFileByName("\\.shell\\","hzk.dat")+sizeof(RomFile);
	DICTLINE =  GetRomFileByName("\\.shell\\","AppleDict.dat")+sizeof(RomFile);
	
	SetMode (MODE_3 | BG2_ENABLE | OBJ_MAP_1D | OBJ_ENABLE);
	keypad keyy ;
	keypad *keys = &keyy ;

welcome:	//��ӭ����
	sleep(1000);
	DrawFilePic("EZWordBG.bmp",0,0,240,160,0,0,1);
	FadeInOut(1,1,50);//	��������
	while(1){
		sleep(15);	keys->update();
		if(keys->press(KEY_START))	break;				
#ifndef _MAIN_ROM_
		if(keys->release(KEY_B))	return PROC_DESKTOP;				
#endif
	}

func:		//ѡ���ܲ˵�
	if(func_i<0)
		func_i=0;
	func_i = ShowWin_Menu("wordmenu.bmp","EW���ܲ˵�.bmp","",(char**)mnu_select_function,4,func_i);
	if(func_i<0)
	{
		FadeInOut(0,1,30);//	�������
		goto welcome;
	}
	if(func_i==3)	//ѡ����Ӣ���ʵ�
	{
		sleep(300);
dict:
		short btnx=40,btny=46, menui=0,  refresh=1, isreload=1;
		char words[32];
		u8 wordsLen=0;

		DrawFilePic("worddict.bmp",0,0,240,160,0,0,0);	//����
		WORDNUM = GetWordNum(0);
		btnx = 0;
		btny = 0;
		while(1){
			sleep(15);	ti++;
       		VBlankIntrWait();                           // Complete V Blank interrupt
			
			if(refresh)
			{
				btnx=(btnx+9)%9;
				btny=(btny+3)%3;
				syncVVram();
				if(wordsLen>0)
				{
					//��ӡӢ�ĵ���
					DrawASCText16(words,wordsLen,66,20,0x12F,1);
				}
				DrawFilePic("underFlag.bmp",78+btnx*17,65+btny*20,16,8,1,0x7FFF,1);	//�����
				refresh=0;				
				//dbgPrint("btnx: %d, btny: %d",btnx,btny);
				//sleep(1000);
			}

			keys->update();
			if ( (keys->press(KEY_LEFT)&&(holdt=ti))|| (keys->hold(KEY_LEFT)&&(ti-holdt>50)) ) 
			{
				btnx-=1;
				refresh=1;
			}
			if ( (keys->press(KEY_RIGHT)&&(holdt=ti))|| (keys->hold(KEY_RIGHT)&&(ti-holdt>50)) ) 
			{
				btnx+=1;
				refresh=1;
			}

			if ( (keys->press(KEY_UP)&&(holdt=ti))|| (keys->hold(KEY_UP)&&(ti-holdt>50)) ) 
			{
				btny-=1;
				refresh=1;
			}
			if ( (keys->press(KEY_DOWN)&&(holdt=ti))|| (keys->hold(KEY_DOWN)&&(ti-holdt>50)) ) 
			{
				btny+=1;
				refresh=1;
			}
			if (keys->release(KEY_B))
			{
				refresh=1;
				if(wordsLen==0) //�����ϼ���Ļ
					goto func;
				if(wordsLen>0)
					wordsLen--;
			}
			if (keys->press(KEY_A))
			{
				refresh=1;
				if(wordsLen<25)
				{
					char c;
					c = (btnx + btny*9) + 'a'; //�ڵ���ĩ�����һ����ĸ
					if(c>'z')
						c = ' ';
					words[wordsLen] = c;
					words[wordsLen+1]=0;
					wordsLen++;
					refresh = 1;
				}
			}
			if(keys->press(KEY_START))
			{
				refresh = 1;
				if(wordsLen==0) //�����ϼ���Ļ
					goto func;
				u16 mnushownum=5;
				int select_i;
				char trans[128],yinbiao[32];
				u16 wi = GetNearWordIndex(0,words);

				if(wi>WORDNUM-4)
					wi = WORDNUM - 3;
				else if(wi<3)
					wi = 2;

				for(u8 i=0;i<mnushownum;i++)
				{
					char www[32];
					GetWord(0, wi+i-2, www, yinbiao, trans); 
					memcpy(mnu_select_words[i],www,32);
				}
				select_i = ShowWin_Menu("wordmenu.bmp","EWѡ�񵥴�.bmp","",(char**)mnu_select_words,5,2);
				if(select_i<0)
				{
					DrawFilePic("worddict.bmp",0,0,240,160,0,0,0);	//����
					continue;
				}
				ciku_i = 0;//ȷ��Ϊ��ѯ�ʿ�
				func_i = 0;//ȫ����ʾ
				part_i = 0;
				wordi = wi-2+select_i;
				goto study;
			}
		}

	}


ciku:		//ѡ��ʿ�˵�
	if(ciku_i<1)
		ciku_i=1;
	ciku_i = ShowWin_Menu("wordmenu.bmp","EWѡ��ʿ�.bmp","",(char**)mnu_select_ciku,4,ciku_i-1);
	if(ciku_i<0)
		goto func;
	ciku_i++;

part:		//ѡ��Χ�˵�
	if(part_i<0)
		part_i=0;
	part_i = ShowWin_Menu("wordmenu.bmp","EWѡ��Χ.bmp","",(char**)mnu_select_fanwei,28,part_i);
	if(part_i<0)
		goto ciku;

	
info:
	DrawFilePic("wordpre.bmp",0,0,240,160,0,0,1); 
	//��ӡ����ȷ����Ϣ
	DrawHZText12((char*)mnu_select_function[func_i], 0,20,46,0,1);
	DrawHZText12((char*)mnu_select_ciku[ciku_i-1], 0,20,66,0,1);
	DrawHZText12((char*)mnu_select_fanwei[part_i], 0,20,86,0,1);
	DrawHZText12("�� START ��ʼ", 0,20,136,0,1);
	while(1){
		sleep(15);	keys->update();
		if(keys->press(KEY_START))	break;				
		if(keys->release(KEY_B))	goto part;				
	}
	wordi = 0;


	FadeInOut(0,0,50);

	DrawFilePic("wordstudy.bmp",0,0,240,160,0,0,1); 
	FadeInOut(1,0,50);
	sleep(1000);
study:
	DrawFilePic("wordstudy.bmp",0,0,240,160,0,0,1); 
	refresh = 1;
	WORDNUM = GetWordNum(ciku_i); 



	u16 vip=0;
	u8 bWord,bPinyin,bTrans,studyfanwei=0;
	char msg[128],word[WORDLEN],trans[128],yinbiao[32];

	
	//�����ֻѧϰ�ص㵥��
	if(part_i==1)
	{
		studyfanwei=1;
		for(u16 i=0;i<WORDNUM;i++)
		{
			if(getvip(ciku_i,i))
			{
				wordi=i;
				break;
			}
		}
	}
	else if((part_i>=2)&&(part_i<28))	//����ĸѧϰģʽ
	{
		studyfanwei=part_i-2+'A';
		for(int i=0;i<WORDNUM;i++)
		{
			GetWord(ciku_i,i,word,yinbiao,trans);
			if((word[0]==(char)studyfanwei)||(word[0]==(char)(studyfanwei+'a'-'A')))
			{
				wordi=i;
				break;
			}
		}
	}

	for(u8 i=0;i<5;i++)
	{
		u8 tmpword[32];
		load(WORDSAVEPOINT + 16+i*32,(u8*)tmpword,24);
		if(tmpword[0]!=0)
			memcpy(mnu_saveload[i],tmpword,32);
	}

		
	switch(func_i)
	{
	case 0:
		bWord=1;
		bPinyin=1;
		bTrans=1;
		break;
	case 1:
		bWord=1;
		bPinyin=0;
		bTrans=0;
		break;
	case 2:
		bWord=0;
		bPinyin=0;
		bTrans=1;
		break;
	}

	while(1) {
		sleep(5);	keys->update();
        VBlankIntrWait();                           // Complete V Blank interrupt

		ti++;
        
		if(refresh)
		{
			DrawFilePic("wordstudy_client.bmp",0,0,240,160,0,0,0); 

			GetWord(ciku_i,wordi,word,yinbiao,trans);
			vip=getvip(ciku_i,wordi);

			if(bWord)
			{
				//��ӡ��ӰӢ�����嵥��
				DrawASCText16(word,0,21,20,RGB(23,23,23),0);
				DrawASCText16(word,0,20,20,0x12F,0);
			}
			if(bPinyin)
			{
				//��ӡ����
				DrawYinBiao12(yinbiao,0,20,60,0x2,0);
			}
			if(bTrans)
			{
				//��ӡ���ķ���
				DrawHZTextRect(trans,0,20,85,140,70,16,0,FM_CL_TITLE,0);
			}
			if(vip)
				DrawFilePic("wordvip.bmp",124,136,44,16,1,0x7FFF,0); 
				;//����VIP��ͼ��

			syncVVram();
			refresh = 0;
		}


		//------------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------
		if ( (keys->press(KEY_LEFT)&&(holdt=ti))|| (keys->hold(KEY_LEFT)&&(ti-holdt>150)) )  
		{
			int rv = getpreword(ciku_i,wordi,studyfanwei);
			if(rv>=0)
			{
				if(func_i!=0)
					bPinyin=0;
				if(func_i==1)
					bTrans=0;
				if(func_i==2)
					bWord=0;
				wordi=rv;
				refresh=1;
			}
		}
		if ( (keys->press(KEY_RIGHT)&&(holdt=ti))|| (keys->hold(KEY_RIGHT)&&(ti-holdt>150)) ) 
		{
			int rv = getnextword(ciku_i,wordi,studyfanwei);
			if(rv>=0)
			{
				if(func_i!=0)
					bPinyin=0;
				if(func_i==1)
					bTrans=0;
				if(func_i==2)
					bWord=0;
				wordi=rv;
				refresh=1;
			}
		}

		if (keys->press(KEY_A)|| (keys->press(KEY_DOWN)&&func_i==1) || (keys->press(KEY_UP)&&func_i==2) )//��ʾ��չ��Ϣ
		{
			refresh=1;
			if((func_i==1)&&(bTrans==0))
			{
				bTrans=1;
				bPinyin=1;
			}
			if((func_i==2)&&(bWord==0))
			{
				bWord=1;
				bPinyin=1;
			}
		}
		if (keys->press(KEY_B)|| (keys->press(KEY_DOWN)&&func_i==2) || (keys->press(KEY_UP)&&func_i==1) )//ȡ����չ��Ϣ
		{
			refresh=1;
			if((func_i==1)&&(bTrans==1))
			{
				bTrans=0;
				bPinyin=0;
			}
			if((func_i==2)&&(bWord==1))
			{
				bWord=0;
				bPinyin=0;
			}
		}
		if (keys->release(KEY_B))
			if(ciku_i == 0)	//���Ŀǰ�����ڴʵ��ѯ״̬
				goto dict;


		if (keys->press(KEY_SELECT))		//����/ȡ��VIP
		{
			vip = (vip)?0:1;
			setvip(ciku_i,wordi,vip);
			refresh=1;
		}
		if (keys->press(KEY_START))		//��ʾѡ���
		{
			int opti=0;
			refresh=1;
			if(ciku_i == 0)	//���Ŀǰ�����ڴʵ��ѯ״̬
				goto dict;

optionwin:
			opti = ShowWin_Menu("wordmenu.bmp","EWѡ��.bmp","",(char**)mnu_run_option,6,opti);
			if(opti < 1)
				continue;
			else if(opti == 1)//�浵
			{
				int rv = ShowWin_Menu("wordmenu.bmp","EW����ѧϰ����.bmp","",(char**)mnu_saveload,5,0);
				if(rv != -1)
				{
					strcpy(mnu_saveload[rv],word);
					save(WORDSAVEPOINT + 16+rv*32,(unsigned char*)word,strlen(word));
					save(WORDSAVEPOINT + 16+rv*32+24,(unsigned char*)&ciku_i,2);
					save(WORDSAVEPOINT + 16+rv*32+24+2,(unsigned char*)&wordi,2);
				}
				goto optionwin;
			}
			else if(opti == 2)//����
			{
				refresh=1;
				int rv = ShowWin_Menu("wordmenu.bmp","EW����ѧϰ����.bmp","",(char**)mnu_saveload,5,0);
				if(rv != -1)
				{
					char tmpword[32];
					load(WORDSAVEPOINT + 16+rv*32,(u8*)tmpword,24);
					if(tmpword[0]==0)	//����ǿս���,�����
						goto optionwin;
					load(WORDSAVEPOINT + 16+rv*32+24,(unsigned char*)&ciku_i,4);
					load(WORDSAVEPOINT + 16+rv*32+24+2,(unsigned char*)&wordi,4);
					WORDNUM = GetWordNum(ciku_i); 
					continue;
				}
				else
					goto optionwin;
			}
			else if(opti == 3)//ɱ��
			{
				u8 z[32];
				FadeInOut(0,1,11);
				memset(z,0,32);
				for(u8 i=0;i<5;i++)
				{
					memset(mnu_saveload[i],0,32);
					save(WORDSAVEPOINT + 16+i*32,z,32);
				}
				sleep(500);
				FadeInOut(1,1,11);
				goto optionwin;
			}
			else if(opti == 4)//ɱ�ص�
			{
				FadeInOut(0,1,11);
				for(u16 i=0;i<WORDNUM;i++)
					setvip(ciku_i,i,0);
				sleep(500);
				FadeInOut(1,1,11);
				goto optionwin;
			}
			else if(opti == 5)//�˳�
			{
				FadeInOut(0,0,30);//	�������
				DrawFilePic("wordpre.bmp",0,0,240,160,0,0,1); 
				FadeInOut(1,0,50);//	�������
				goto info;
			}
		}

	////////////////////////////////////////////////////////////////////////////////
	}//while
}//main
