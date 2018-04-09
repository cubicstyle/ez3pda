#include "agb.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include "ctype.h"

#include "string.h"
#include "keypad.h"
#include "BGfunc.h"
//#include "spritemgr256.h"
#include "screenmode.h"
#include "cmddefine.h"
#include "global.h"
#include "wordglobal.h"
#include "shell.h"


softhead SOFTHEAD;
extern u16 WORDNUM;
extern u16 CIKUNUM;
extern u8* DICTLINE;
extern u8* HZKLINE;


//******************************************************************************
//**** ���ָ���ʿ��еĵ�����Ŀ
int GetWordNum(u8 cikui)
{
	u8 *HeadBase = DICTLINE + sizeof(softhead) + 4; //ͷ�ṹ��ַ
	headsect *hs;
	wordsect *ws;


	//���ƫ��ֱ���ƶ��Ĵʿ�λ��
	for(int i=0;i<cikui;i++)
	{
		hs = (headsect*)HeadBase;
		HeadBase += sizeof(headsect) + sizeof(wordsect)*hs->wordnum + hs->translen; //�ļ�ϵͳ��ʼ��ַ
	}

	hs = (headsect*)HeadBase;
	return hs->wordnum;
}


//******************************************************************************
//**** �ж������ַ���������Ȩ������-1,0,1
int stropt(char *word1, char *word2)
{
	u8 wl1,wl2,min;
	//char word1[32],word2[32];

	wl1=strlen(word1);
	wl2=strlen(word2);
	min = (wl1<wl2)?wl1:wl2;
	for(u8 i=0;i<min;i++)
	{
		if(tolower(word1[i])>tolower(word2[i]))
			return 1;
		else if(tolower(word1[i])<tolower(word2[i]))
			return -1;
	}
	if(wl1>wl2)
		return 1;
	else if(wl1<wl2)
		return -1;
	else
		return 0;
}


//******************************************************************************
//**** ����һ���ַ�����������֮��Ϊ����ĵ���(����2�ַ�)
int GetNearWordIndex(u8 cikui, char* wordstr)	
{
	u8 *HeadBase = DICTLINE + sizeof(softhead) + 4; //ͷ�ṹ��ַ
	u8 *wdBase;
	char word2[WORDLEN];
	wordsect *ws;
	headsect *hs;
	int i;
	u16  bar1,bar2,bar3;


	// WORDNUM = GetWordNum(cikui);

	//���ƫ��ֱ��ָ���Ĵʿ���λ��
	for(int i=0;i<cikui;i++)
	{
		hs = (headsect*)HeadBase;
		HeadBase += sizeof(headsect) + sizeof(wordsect)*hs->wordnum + hs->translen; //�ļ�ϵͳ��ʼ��ַ
	}
	HeadBase += sizeof(headsect);
	


	bar1 = 0; bar2 = WORDNUM/2 ; bar3 = WORDNUM-1;

	while(bar3-bar1>1)
	{
		wdBase = HeadBase + sizeof(wordsect)*bar2; //�ļ�ϵͳ��ʼ��ַ
		ws = (wordsect*)wdBase;
		strcpy(word2,ws->word);
		if(stropt(wordstr,word2)>0)
		{
			bar1 = bar2; 
			bar2 = bar1 + (bar3 - bar1 + 1)/2;
		}
		else if(stropt(wordstr,word2)<0)
		{
			bar3 = bar2; 
			bar2 = bar1 + (bar3 - bar1 + 1)/2;
		}
		else
			return bar2;
	}
	return bar2-1;
}


//******************************************************************************
//**** �õ��ʿ���Ŀ
int GetCikuNum()	
{
	int *num;
	num = (int*)DICTLINE;
	return *num;
}


//******************************************************************************
//**** �õ��ʿ��ļ���ͷ���ݽṹ
void GetSoftHead()
{
	u8 *HeadBase = DICTLINE; //ͷ�ṹ��ַ
	softhead *t = (softhead*)HeadBase;
	int len;
	Dec((unsigned char*)t->data1,256,(unsigned char*)SOFTHEAD.data1,&len);
	Dec((unsigned char*)t->data2,256,(unsigned char*)SOFTHEAD.data2,&len);
	Dec((unsigned char*)t->data3,256,(unsigned char*)SOFTHEAD.data3,&len);
	Dec((unsigned char*)t->data4,256,(unsigned char*)SOFTHEAD.data4,&len);
}


//******************************************************************************
//**** �õ�ĳ�ʿ��index�����ʵ�����
int GetWord(u8 cikui, u16 index, char* word, char* yinbiao, char* trans)	
{
	u8 *HeadBase = DICTLINE + sizeof(softhead) + 4; //ͷ�ṹ��ַ
	u8 *wdBase, *transline,ybover=0;
	char *str;
	wordsect *ws;
	headsect *hs;
	int i,p1,p2;

	//���ƫ��ֱ��ָ���Ĵʿ�λ��
	for(int i=0;i<cikui;i++)
	{
		hs = (headsect*)HeadBase;
		HeadBase += sizeof(headsect) + sizeof(wordsect)*hs->wordnum + hs->translen; 
	}
	HeadBase += sizeof(headsect);

	wdBase = HeadBase + sizeof(wordsect)*index;  
	ws = (wordsect*)wdBase;
	strcpy(word,ws->word);

	transline = HeadBase + sizeof(wordsect)*WORDNUM + ws->address;

	for(i=0;i<ws->size;i++)
	{
		if(transline[i]=='/')
		{
			if(ybover==1)
			{
				p2=i;
				ybover=2;
				break;
			}
			else
			{
				p1=i;
				ybover=1;
			}
		}
	}

	str = (char*)transline;
	if(ybover==2)
	{
		if(p2-p1==1)
			yinbiao[0]=0;
		else
			memcpy(yinbiao,str,i+1);
		yinbiao[i+1]=0;
		memcpy(trans,str+i+1,ws->size-i-1);
		trans[ws->size-i-1]=0;
	}
	else
	{
		yinbiao[0]=0;
		memcpy(trans,str,ws->size);
		trans[ws->size]=0;
	}
}


//******************************************************************************
//**** ����ĳ�ʵ��ĳλ�õ����Ƿ�Ϊ�ص㵥��
u8 getvip(u16 ciku, u16 wordi)
{
	unsigned char tmp,c;
	tmp = 1<<(wordi%8);
	load(WORDSAVEPOINT + 1024 + 1024*ciku + wordi/8, &c,1);
	return c&tmp;
}


//******************************************************************************
//**** ����ĳ�ʵ��ĳλ�õ���Ϊ�ص㵥��
void setvip(u16 ciku, u16 wordi,u8 isvip)
{
	unsigned char tmp,c;
	tmp = 1<<(wordi%8);
	load(WORDSAVEPOINT + 1024 + 1024*ciku + wordi/8, &c,1);
	if(isvip)	//set
		c = c|tmp;
	else	//unset
		c = c&(tmp^0xFF);
	save(WORDSAVEPOINT + 1024 + 1024*ciku + wordi/8, &c,1);
}


//******************************************************************************
//**** �õ���һ���������
int getnextword(u8 ciku, u16 wordi,u8 fanwei)
{
	if(fanwei==0)		//���е���
	{
		if(wordi<WORDNUM-1)
			return wordi+1;
		else
			return -1;
	}
	else if(fanwei==1)	//vip����
	{
		if(wordi<WORDNUM-1)
		{
			for(int i=wordi+1;i<WORDNUM;i++)
			{
				if(getvip(ciku,i))
					return i;
			}
			return -1;
		}
		else
			return -1;
	}
	else if((fanwei>'A'-1)&&(fanwei<'Z'+1)) //����ĸ��ʽ
	{
		if(wordi<WORDNUM-1)
		{
			for(int i=wordi+1;i<WORDNUM;i++)
			{
				char word[WORDLEN],trans[128],yinbiao[32];
				GetWord(ciku,i,word,yinbiao,trans);
				if((word[0]==(char)fanwei)||(word[0]==(char)(fanwei+'a'-'A')))
					return i;
			}
			return -1;
		}
		else
			return -1;
	}
}


//******************************************************************************
//**** �õ�ǰһ���������
int getpreword(u8 ciku, u16 wordi,u8 fanwei)
{
	if(fanwei==0)		//���е���
	{
		if(wordi>0)
			return wordi-1;
		else
			return -1;
	}
	else if(fanwei==1)	//vip����
	{
		if(wordi>0)
		{
			for(int i=wordi-1;i>=0;i--)
			{
				if(getvip(ciku,i))
					return i;
			}
			return -1;
		}
		else
			return -1;
	}
	else if((fanwei>'A'-1)&&(fanwei<'Z'+1))
	{
		if(wordi>0)
		{
			for(int i=wordi-1;i>=0;i--)
			{
				char word[WORDLEN],trans[128],yinbiao[32];
				GetWord(ciku,i,word,yinbiao,trans);
				if((word[0]==(char)fanwei)||(word[0]==(char)(fanwei+'a'-'A')))
					return i;
			}
			return -1;
		}
		else
			return -1;
	}
}

