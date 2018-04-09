#include "stdio.h"
#include "stdarg.h"
#include "string.h"
//#include "math.h"
#include "stdlib.h"

#include "agb.h"
#include "global.h"
#include "BGfunc.h"
#include "keypad.h"
#include "screenmode.h"
#include "jpeg.h"
#include "asc1616.h"
#include "yinbiao.h"
#include "shell.h"
#include "savermanage.h"

extern res_struct res;

//diffrent from agb official 
#define REG_TM3D       *(u16*)0x400010C
#define REG_BLDMOD     *(u16*)0x4000050
#define REG_COLEV      *(u16*)0x4000052
#define REG_COLEY      *(u16*)0x4000054
#define OAMmem         (u32*)0x7000000
#define VideoBuffer    (u16*)0x6000000
#define OAMdata		   (u16*)0x6100000
#define BGPaletteMem   (u16*)0x5000000
#define OBJPaletteMem  (u16*)0x5000200
#define GAMEPAKMem	   (u8*) 0x8000000
#define Vcache         (u16*)0x2001000
#define IWRAM		   (u16*)0x3000000

const unsigned short picExtnum=11;	//ʶ���ͼ���ļ�����
const char picExt[11][4]={			//ʶ���ͼ���ļ���չ���б�
	"BMP",
	"GIF",
	"JPG",
	"ICO",
	"JPE", 
	"LWF",
	"PIC", 
	"PCX",
	"PSD",
	"TIF",
	"PNG"
};



//******************************************************************************
//**** ��ʱ����(���뼶) 
void sleep(u16 ms)
{
	//Start the timer
	*(vu16*)REG_TM3CNT_H  = 1|2|128|0x80;

    //zero the timer
	REG_TM3D = 0; 

	while(ms--)
	{
		while(REG_TM3D <= 16){}                //wait
		REG_TM3D = 0;          //reset the timmer
	} 
	//Stop the timer
	*(vu16*)REG_TM3CNT_H  = 0;
	
} 


//******************************************************************************
//**** �ڵ�����ʾ���У���ӡ������Ϣ(Ҫ��MODE_3)
void dbgPrint(char *str,...)			  
{
	va_list ptr;char *strr;
	strr=(char*)malloc(strlen(str)+2);
	va_start(ptr,str);
	vsprintf(strr,str,ptr);
	va_end(ptr);
	Clear(0,130,240,30,RGB(30,30,2),1);
	DrawHZTextRect(strr,120,1,131,238,29,14,0,RGB(3,3,31),1);
	free(strr);
}


//******************************************************************************
//**** ��ʼ����Դ����ϵͳ��Դ�ṹ����и�ֵ
void initResource()
{
	res.res_ZKDATA = GetRomFileByName("\\.shell\\","hzk.dat")+sizeof(RomFile);
	res.res_ASCDATA = GetRomFileByName("\\.shell\\","asc.dat")+sizeof(RomFile);

	res.res_DESKBG =	(u16*)(GetRomFileByName("\\.shell\\bmp\\","desktop.bmp")+sizeof(RomFile)+8);
	res.res_DESKICON =	(u16*)(GetRomFileByName("\\.shell\\bmp\\","deskicon.bmp")+sizeof(RomFile)+8);
	res.res_FILEICON =	(u16*)(GetRomFileByName("\\.shell\\bmp\\","icons.bmp")+sizeof(RomFile)+8);
	res.res_FILEBG =	(u16*)(GetRomFileByName("\\.shell\\bmp\\","filemng.bmp")+sizeof(RomFile)+8);
	res.res_FILEBGHEAD =(u16*)(GetRomFileByName("\\.shell\\bmp\\","filemnghead.bmp")+sizeof(RomFile)+8);

	res.res_TXTBG =		(u16*)(GetRomFileByName("\\.shell\\bmp\\","txtbg.bmp")+sizeof(RomFile)+8);
	res.res_TXTSCUP   = (u16*)(GetRomFileByName("\\.shell\\bmp\\","TextScrollUp.BMP")+sizeof(RomFile)+8);
	res.res_TXTSCDOWN = (u16*)(GetRomFileByName("\\.shell\\bmp\\","TextScrollDown.BMP")+sizeof(RomFile)+8);
	res.res_TXTSCSIGN = (u16*)(GetRomFileByName("\\.shell\\bmp\\","TextScrollButtonSign.BMP")+sizeof(RomFile)+8);
	res.res_TXTSCBACK = (u16*)(GetRomFileByName("\\.shell\\bmp\\","TextScrollBack.BMP")+sizeof(RomFile)+8);
}


//******************************************************************************
//**** �趨��ǰ���뵭��״̬
//**** status=1������Ч����Ϊ0����ΪЧ�����
//**** colorWhite=1�����ɫЧ����Ϊ0�����ɫЧ��
void FadeStatus(u8 status, u8 colorWhite)
{
	if(colorWhite==0)
		REG_BLDMOD = 0xFFFF;
	else
		REG_BLDMOD = 0xFFBF;
	if(status)
		REG_COLEY = 0x0800+16;
	else
		REG_COLEY = 0x0800;
}


//******************************************************************************
//**** ָ��ʱ��εĵ��뵭������Ч��
//**** fadeIN=0�������������أ��������루�����ᣩ
//**** colorWhite=1�����ɫЧ����Ϊ0�����ɫЧ��
//**** timeΪ������������У�ÿ֡������ʱ��
void FadeInOut(u8 fadeIN, u8 colorWhite, u32 time)
{
	if(colorWhite==0)
		REG_BLDMOD = 0xFFFF;
	else
		REG_BLDMOD = 0xFFBF;
	for(u32 i=0;i<16+1;i++)
	{
		if(fadeIN==0)
			REG_COLEY = 0x0800+i;
		else
			REG_COLEY = 0x0800+16-i;
		sleep(time);
	}
}


//******************************************************************************
//**** ALPHA���Ч����ָ������GB�����������Եı���(����֮��Ϊ16)
void AlphaBG(u8 bg1index, u8 bg2index,u8 bg1per, u8 bg2per)
{
	REG_BLDMOD = (1<<bg1index)|(1<<(8+bg2index))|(1<<6);//|(1<<4)|(1<<(8+4));
	REG_COLEV = bg1per|(bg2per<<8);
}


//******************************************************************************
//**** ����Ļ�Դ�/�����ϻ�����ͼ��(240x160)
//**** isDrawDirect=1,ֱ��д���Դ棻=0��д���������
void DrawBG(u16 *GFX, u8 isDrawDirect)
{
	u16 *p;
	if(isDrawDirect)
		p = VideoBuffer;
	else
		p = Vcache;

	DMA_Copy(3,p,GFX,38400,DMA_16NOW);
}


//******************************************************************************
//**** ����Ļ�Դ�/���� x,y����һ��WxH���ľ���ͼ��
//**** isTransΪ1�����Ƿ����͸��ɫ��tcolorΪ͸��ɫֵ
void DrawPic(u16 *GFX, u16 x, u16 y, u16 w, u16 h, u8 isTrans, u16 tcolor, u8 isDrawDirect)
{
	u16 *p,c;
	u16 xi,yi,ww,hh;

	if(isDrawDirect)
		p = VideoBuffer;
	else
		p = Vcache;
	
    hh = (y+h>159)?159:(y+h);
    ww  = (x+w>239)?(239-x):w;
	
	
	if(isTrans)//���͸����ʾ
	{
		for(yi=y; yi < hh; yi++)
			for(xi=x;xi<x+ww;xi++)
			{
				c = GFX[(yi-y)*w+(xi-x)];
				if(c!=tcolor)
					p[yi*240+xi] = c;
			}
	}
	else		//ʵ����ʾ
	{
		for(yi=y; yi < hh; yi++)
			DMA_Copy(3,p+yi*240+x,GFX+(yi-y)*w,w,DMA_16NOW);
	}
}


//******************************************************************************
//**** ָ��ͼ��·���ļ���������Ļ�Դ�/���� x,y����һ��WxH���ľ���ͼ��
//**** isTransΪ1�����Ƿ����͸��ɫ��tcolorΪ͸��ɫֵ
void DrawFilePic(char *filename, u16 x, u16 y, u16 w, u16 h, u8 isTrans, u16 tcolor, u8 isDrawDirect)
{
	//u16 *p,c;
	//u16 xi,yi,ww,hh;
	u16 *GFX;

	GFX = (u16*)(GetRomFileByName("\\.shell\\bmp\\",filename)+sizeof(RomFile)+8);

	DrawPic(GFX,x,y,w,h,isTrans,tcolor,isDrawDirect);
}


//******************************************************************************
//**** ��������������ͬ�����Դ�
void syncVVram()
{
    u16 *p = (u16*)Vcache;
    u16 *v = VideoBuffer;
    DMA_Copy(3,v,p,38400,DMA_16NOW);
}


//******************************************************************************
//**** ��ָ����ɫ����Դ�/�����һ������
void Clear(u16 x, u16 y, u16 w, u16 h, u16 c, u8 isDrawDirect)
{
	u16 *p;
	u16 yi,ww,hh;
    
	if(isDrawDirect)
		p = VideoBuffer;
	else
		p = Vcache;

    hh = (y+h>160)?160:(y+h);
    ww  = (x+w>240)?(240-x):w;

	u16 tmp[240];
	for(u8 i=0;i<ww;i++)
		tmp[i] = c;

	for(yi=y; yi < hh; yi++)
        DMA_Copy(3,p+yi*240+x,tmp,ww,DMA_16NOW);
}


//******************************************************************************
//**** ����ĳ����ı���ͼ��ʹ�ñ���ͼ����Ӧλ�����ĳ����
void ClearWithBG(u16 x, u16 y, u16 w, u16 h, u16* pbg, u8 isDrawDirect)
{
	u16 *p;
	u16 yi,ww,hh;
    
	if(isDrawDirect)
		p = VideoBuffer;
	else
		p = Vcache;

    hh = (y+h>160)?160:(y+h);
    ww  = (x+w>240)?(240-x):w;

	for(yi=y; yi < hh; yi++)
        DMA_Copy(3,p+yi*240+x,pbg+yi*240+x,ww,DMA_16NOW);
}


//******************************************************************************
//**** �������滭�㺯��
void setPixel(int x, int y, u16 c)
{
	u16 *p = Vcache;
	p[x+y*240] = c;
}


//******************************************************************************
//**** ��Ļ�Դ�/���滭�㺯��
void setPixel2(u16 *v, int x, int y, u16 c)
{
	v[x+y*240] = c;
}


//******************************************************************************
//**** ��16X8��ASC�ִ�,��������Ϊlen����len��0���Զ��жϳ���
void DrawASCText16(char *str, u32 len, u16 x, u16 y,  u16 c, u8 isDrawDirect)
{
    u32 i,j,k, l;
    
    int location;
	u32 hi=0;
	u8 *p;
	u16 *v;
	u16 *p1 = Vcache;
	u16 *p2 = VideoBuffer;

	if(isDrawDirect)
		v = p2;
	else
		v = p1;

	if(len==0)
		l=strlen(str);
	else
	{
		if(len>strlen(str))
			l=strlen(str);
		else
			l=len;
	}

    while(hi<l)
    {
		if(str[hi]<0x7F+1)  //ASCII code
		{
			location = str[hi]*32;
			//���ֿ��ﶨλ
			p=(u8*)&ASC1616_DATA[location];

			if(str[hi]==' ')
			{
				for(i=0;i<16;i++)
				for(k=0;k<8;k++) 
					setPixel2(v,x+k,y+i,c);
					//DrawHZPixel(x+k,y+i,0);
			}
			else
			{
				for(i=0;i<16;i++)
				for(j=0;j<2;j++)
				for(k=0;k<8;k++) 
				if(((p[i*2+j])>>(7-k))&0x1) 
					setPixel2(v,x+j*8+k,y+i,c);
					//DrawHZPixel(x+j*8+k,y+i,c);
			}
			//x+=16;
			x+=ascwidth[str[hi]];
			hi++;
		}
    }
}


//******************************************************************************
//**** ��ӡӢ������(12x6��ʽ),��������Ϊlen����len=0���Զ��жϳ���
void DrawYinBiao12(char *str, u32 len, u16 x, u16 y, u16 c, u8 isDrawDirect)
{
    u32 i,j,k, l;
    
    int  location;
	u32 hi=0;
	u8 *p;
	u16 *v;
	u16 *p1 = Vcache;
	u16 *p2 = VideoBuffer;

	if(isDrawDirect)
		v = p2;
	else
		v = p1;


	if(len==0)
		l=strlen(str);
	else
	{
		if(len>strlen(str))
			l=strlen(str);
		else
			l=len;
	}

    while(hi<l)
    {
		if(str[hi]<0x7F+1)  //ASCII code
		{
			location = str[hi]*24;
			hi++;
			//���ֿ��ﶨλ
			p=(u8*)&YINBIAO_DATA[location];

			for(i=0;i<12;i++)	//��ͬ�ֿ�Ӧ�䶯
			for(j=0;j<2;j++)	//�̶�Ϊ2
			for(k=0;k<8;k++)	//�̶�Ϊ8 
			if(((p[i*2+j])>>(7-k))&0x1)			//�̶�Ϊ7-k
				setPixel2(v,x+j*8+k,y+i,c);
				//DrawHZPixel(x+j*8+k,y+i,c);		//�̶�Ϊj*8
			x+=10;
			continue;
		}
    }
}


//******************************************************************************
//**** ��ӡ���һ�о���(���12x12����),��������Ϊlen����len=0���Զ��жϳ���
//**** (������ʵ����΢���ӣ���Ҫ�Ǿ������üӷ�ʵ�ֳ˷����ӿ������ٶ�)
void DrawHZText12(char *str, u16 len, u16 x, u16 y, u16 c, u8 isDrawDirect)
{
    u32 i,l,hi=0;
    u32 location;
	u8 cc,c1,c2;
	u16 *v;
	u16 *p1 = Vcache;
	u16 *p2 = VideoBuffer;
	u16 yy;

	if(isDrawDirect)
		v = p2;
	else
		v = p1;

	if(len==0)
		l=strlen(str);
	else
		if(len>strlen(str))
			l=strlen(str);
		else
			l=len;

	if((u16)(len*6)>(u16)(240-x))
		len=(240-x)/6;
    while(hi<l)
    {
		c1 = str[hi];
    	hi++;
    	if(c1<0x80)  //ASCII Ӣ�ĵ��ֽ��ַ�
    	{
			yy = 240*y;	//yy���Դ���ʼ����õ������ƫ����
    		location = c1*12;	//�ֿ���ƫ����
    		for(i=0;i<12;i++)
			{
				cc = res.res_ASCDATA[location+i];
				if(cc & 0x01)
					v[x+7+yy]=c;
				if(cc & 0x02)
					v[x+6+yy]=c;
				if(cc & 0x04)
					v[x+5+yy]=c;
				if(cc & 0x08)
					v[x+4+yy]=c;
				if(cc & 0x10)
					v[x+3+yy]=c;
				if(cc & 0x20)
					v[x+2+yy]=c;
				if(cc & 0x40)
					v[x+1+yy]=c;
				if(cc & 0x80)
					v[x+yy]=c;
				yy+=240;
			}		
    		x+=6;
    		continue;
    	}
		else	//˫�ֽں���
		{	
    		c2 = str[hi];
    		hi++;
    		if(c1<0xb0)										//������ֿ�ǰ���ֵķ���
    			location = ((c1-0xa1)*94+(c2-0xa1))*24;
    		else
    			location = (9*94+(c1-0xb0)*94+(c2-0xa1))*24;

			// �����ַ����ݻ����ַ�
			yy = 240*y;
			for(i=0;i<12;i++)	//��ͬ�ֿ�Ӧ�䶯
			{				
				cc = res.res_ZKDATA[location+i*2]; //ǰ�뺺��
				if(cc & 0x01)
					v[x+7+yy]=c;
				if(cc & 0x02)
					v[x+6+yy]=c;
				if(cc & 0x04)
					v[x+5+yy]=c;
				if(cc & 0x08)
					v[x+4+yy]=c;
				if(cc & 0x10)
					v[x+3+yy]=c;
				if(cc & 0x20)
					v[x+2+yy]=c;
				if(cc & 0x40)
					v[x+1+yy]=c;
				if(cc & 0x80)
					v[x+yy]=c;
								
				cc = res.res_ZKDATA[location+i*2+1];//��뺺��
				if(cc & 0x01)
					v[x+15+yy]=c;
				if(cc & 0x02)
					v[x+14+yy]=c;
				if(cc & 0x04)
					v[x+13+yy]=c;
				if(cc & 0x08)
					v[x+12+yy]=c;
				if(cc & 0x10)
					v[x+11+yy]=c;
				if(cc & 0x20)
					v[x+10+yy]=c;
				if(cc & 0x40)
					v[x+9+yy]=c;
				if(cc & 0x80)
					v[x+8+yy]=c;
				yy+=240;
			}
			x+=12;
		}
	}
}


//******************************************************************************
//**** ׼����ָ�������д�ӡ���ֶ��䣨�Զ����У����������õ��ı�����
//**** ��������Ϊlen����len=0���Զ��жϳ���
void DrawHZTextGetLineNum(char *str, u32 len, u16 w, u16 *totalLineNum)
{
    u32 i,hiden,intl, l, rownum,colmaxnum,colnum;
	u32 pi;
	    
	if(len==0)
		l=strlen(str);
	else
		l=len;

	colmaxnum=w/6;

	pi=0;
	rownum=0;
	while(pi<l)
	{
		hiden=0;
		colnum=colmaxnum;
		for(i=0;i<colmaxnum;i++)
		{
			if(pi+i>l-1)
			{
				colnum=i;
				break;
			}
			if(str[pi+i]==0x0D)	//DOS���� 0x0D,0x0A
			{
				if(pi+i<l-1)
					if(str[pi+i+1]==0x0A)
					{
						colnum=i;
						hiden=2;
						break;
					}
			}
			if(str[pi+i]==0x0A)	//DOS���� 0x0D,0x0A
			{
				colnum=i;
				hiden=1;
				break;
			}
		}

		intl=0;//�Ƿ�ضϺ���
		if(colnum==colmaxnum)//��Ȼ����ʱ���ж��Ƿ�ضϺ���
		{
			for(i=0;i<colnum;i++)
			{
				if(str[pi+i]<0x7F)
					intl=0;
				else
				{
					if(intl==0)	//����ϴ�������������ǰ�
						intl=1;
					else
						intl=0; //����ϴ��ǰ룬���������
				}
			}
		}
		if(intl==1)
			colnum--;

		rownum++;
		pi+=colnum+hiden;
	}//while
	*totalLineNum=rownum;
}


//******************************************************************************
//**** �򵥵�TXT�ļ��Ķ���(��ͣʹ��)
void DrawTXTviewer(char *data, u32 size, u32 start, u16 spcLine, u16 scrH, u16 scrY , u16 mode)
{
	u32 i,hiden,intl, l,rownum,colmaxnum,colnum;
	u32 pi;

	//ˢ��ȫ��
	if(mode&FM_TXT_LINES)
		DrawBG((u16*)res.res_TXTBG,0);//����
			    
	l=size;
	colmaxnum=(240-FM_SCR_W)/6;

	ClearWithBG(240-FM_SCR_W,0,FM_SCR_W,160,res.res_TXTBG,0);
	Clear(240-FM_SCR_W,scrY,FM_SCR_W,scrH,FM_CL_GRAY,0);
	DrawRect(240-FM_SCR_W,0,240-1,160-1,0x4);

	pi=0;
	rownum=0;
	//for(j=0;j<TXT_MSN;j++)
	while(pi<l)
	{
		intl=0;
		hiden=0;
		colnum=colmaxnum;
		for(i=0;i<colmaxnum;i++)//Ϊ��ʾĳ�����֣�������׼��
		{
			if(pi+i>l-1)
			{
				colnum=i;
				break;
			}
			if(data[pi+i]==0x0D)	//DOS���� 0x0D,0x0A
			{
				if(pi+i<l-1)
					if(data[pi+i+1]==0x0A)
					{
						colnum=i;
						hiden=2;
						break;
					}
			}
			if(data[pi+i]==0x0A)	//DOS���� 0x0D,0x0A
			{
				colnum=i;
				hiden=1;
				break;
			}

			if(data[pi+i]<0x7F)
				intl=0;
			else
			{
				if(intl==0)	//����ϴ�������������ǰ�
					intl=1;
				else
					intl=0; //����ϴ��ǰ룬���������
			}
		}

		if(intl==1)
			colnum--;

		if(rownum+1>start)//��ָ��������ʼ��ȡ��ʾ
		{
			//ˢ��������
			if(mode&FM_TXT_SPCLINE)
			{	
				if(spcLine==rownum-start)
				{
					Clear(0,spcLine*TXT_ROW_H,240-FM_SCR_W,TXT_ROW_H,FM_CL_BG,0);
					if(colnum>0)
						DrawHZText12(data+pi,colnum,1,1+TXT_ROW_H*(rownum-start),FM_CL_BLACK,0);//��ӡ�ִ�
					return;
				}
			}
			else	//ˢ������
				if(colnum>0)
					DrawHZText12(data+pi,colnum,1,1+TXT_ROW_H*(rownum-start),FM_CL_BLACK,0);//��ӡ�ִ�
		}

		pi+=colnum+hiden;
		rownum++;
		if(rownum>start+TXT_MSN-1)
			break;
	}//while
}


//******************************************************************************
//**** ��ָ�������д�ӡ�ı����䣬�Զ�����
//**** ָ��ÿ�еĸ߶�lineH(�Ƽ�14-16)��liָ�������п�ʼ��ʾ���ı���ɫΪc
void DrawHZTextRect(char *str, u32 len, u16 x, u16 y, u16 w, u16 h, u16 lineH, u16 li, u16 c, u8 isDrawDirect)
{
    u16 i,hiden,intl, l,rowmaxnum,rownum,colmaxnum,colnum;
	u16 pi;
	    
	if(len==0)
		l=strlen(str);
	else
		l=len;

	rowmaxnum=h/lineH;
	colmaxnum=w/6;

	if(rowmaxnum<2)//���������ʾ���У�����ʼ��ʾ��Ϊ0
		li=0;


	pi=0;
	rownum=0;

	while(pi<l)
	{
		hiden=0;
		colnum=colmaxnum;
		for(i=0;i<colmaxnum;i++)
		{
			if(pi+i>l-1)
			{
				colnum=i;
				break;
			}
			if(str[pi+i]==0x0D)	//DOS���� 0x0D,0x0A
			{
				if(pi+i<l-1)
					if(str[pi+i+1]==0x0A)
					{
						colnum=i;
						hiden=2;
						break;
					}
			}
			if(str[pi+i]==0x0A)	//DOS���� 0x0D,0x0A
			{
				colnum=i;
				hiden=1;
				break;
			}
		}

		intl=0;//�Ƿ�ضϺ���
		if(colnum==colmaxnum)//��Ȼ����ʱ���ж��Ƿ�ضϺ���
		{
			for(i=0;i<colnum;i++)
			{
				if(str[pi+i]<0x7F)
					intl=0;
				else
				{
					if(intl==0)	//����ϴ�������������ǰ�
						intl=1;
					else
						intl=0; //����ϴ��ǰ룬���������
				}
			}
		}
		if(intl==1)
			colnum--;

		if(rownum+1>li)
			if(colnum>0)
				DrawHZText12(str+pi,colnum,x+1,y+1+lineH*(rownum-li),c,isDrawDirect);
		pi+=colnum+hiden;
		rownum++;
		if(rownum>TXT_MSN+li-1)
			break;
		if(rownum>h/TXT_ROW_H+li-1)
			break;
	}//for
}


//******************************************************************************
//**** �����ļ���,���ظ��ļ�����
u32 GetFileType(char *rfname)
{
	int i,pos=1000,el;
	char ext[MAX_PATH];
	char ep[4];
	int l=(int)strlen(rfname);

	for(i=0;i<l;i++)
		if(rfname[i]=='.')
			pos=i;
	if(pos==1000)
		return 0;

	el = l-pos-1;	//��չ������
	memcpy(ext,rfname+pos+1,el);
	ext[el]=0;
	for(i=0;i<el;i++)
	{
		ext[i]=(ext[i]>'a'-1)?ext[i]-('a'-'A'):ext[i];
	}
	if(strcmp("TXT",ext)==0)
		return PROC_TXT_VIEW;
	if(strcmp("INI",ext)==0)
		return PROC_TXT_VIEW;
	if(strcmp("INF",ext)==0)
		return PROC_TXT_VIEW;
	if(strcmp("C",ext)==0)
		return PROC_C_VIEW;
	if(strcmp("CPP",ext)==0)
		return PROC_C_VIEW;
	if(strcmp("H",ext)==0)
		return PROC_H_VIEW;
	if(strcmp("HTM",ext)==0)
		return PROC_HTM_VIEW;
	if(strcmp("HTML",ext)==0)
		return PROC_HTM_VIEW;
	if(strcmp("GBA",ext)==0)
		return PROC_GBA_VIEW;
	if(strcmp("BIN",ext)==0)
		return PROC_GBA_VIEW;
	if(strcmp("LZ7",ext)==0)
		return PROC_ZIPGBA_VIEW;
	if(strcmp("SAV",ext)==0)
		return PROC_SAVER_VIEW;
	for(i=0;i<100;i++)
	{
		sprintf(ep,"S%02d",i);
		if(strcmp(ep,ext)==0)
			return PROC_SAVER_VIEW;
	
	}
	for(i=0;i<picExtnum;i++)
	{
		if(strcmp(picExt[i],ext)==0)
			return PROC_JPG_VIEW;		
	}

	return PROC_UNKNOWN;
}


//******************************************************************************
//**** ���ߺ�������ɫΪc
void DrawLine(u16 x, u16 y, u16 x2, u16 y2, u16 c)
{
	u16 i;

    if(x2==x)//����
	{
		if(y==y2)
		{
			setPixel(x,y,c);
			return;
		}
    	for(i=0;i<abs(y2-y)+1;i++)
    		setPixel(x,y+i*(abs(y2-y)/(y2-y)),c);

		return;
	}
	if(y2==y)//����
	{
    	for(i=0;i<abs(x2-x)+1;i++)
    		setPixel(x+i*((x2-x)/abs(x2-x)),y,c);
		return;
	}
	if(abs(x2-x)>abs(y2-y)) //����б��
	{
		if(x2>x)
    		for(i=x;i<x2+1;i++)
    			setPixel(i,y+((i-x)*(y2-y))/(x2-x),c);
		else
    		for(i=x;i>x2+1;i--)
    			setPixel(i,y+((i-x)*(y2-y))/(x2-x),c);
	}
	else				//����б��
	{
		if(y2>y)
    		for(i=y;i<y2+1;i++)
    			setPixel(x+((i-y)*(x2-x))/(y2-y),i,c);
		else
    		for(i=y;i>y2+1;i--)
    			setPixel(x+((i-y)*(x2-x))/(y2-y),i,c);
	}
}


//******************************************************************************
//**** ��һ������ı���
void DrawRect(u16 x, u16 y, u16 x2, u16 y2, u16 c)
{
	DrawLine(x,y,x2,y,c);
	DrawLine(x,y,x,y2,c);
	DrawLine(x2,y2,x2,y,c);
	DrawLine(x2,y2,x,y2,c);
}


//******************************************************************************
//**** ������ͼ�����Ϣ���ڣ������ֱ�Ϊ���⡢��Ϣ���ݡ�X��Y��ͼ�꼯������
void CreateWindow(char* caption, char* msg, u16 x, u16 y, u8 iconIndex)
{
	DrawFilePic("itemdisc.bmp",x,y,128,80,1,0x7FFF,1);	//������
    DrawIcon(x+13,y+26,iconIndex,1);					//��ͼ��
	DrawFilePic("ok.bmp",x+7,y+58,24,14,1,0x7FFF,1);	//��OK
	DrawFilePic("x.bmp",x+34,y+58,12,12,1,0x7FFF,1);	//��X
	if(strcmp(caption,"")==0)//�ձ��⣬��������Ϣ���ݣ�������Ϣ��������ʾ
		DrawHZTextRect(msg,0,x+54,y+16,62,56,14,0,2,1);	//д��Ϣ
	else
	{
		DrawHZText12(caption,0,x+54,y+16, 3,1);	//�������
		DrawHZText12(caption,0,x+55,y+16, 3,1);
		DrawHZTextRect(msg,0,x+54,y+30,62,40,14,0,2,1);	//д��Ϣ
	}
}


//******************************************************************************
//**** ������(��ͣʹ��)
void DrawFrame(char *caption)
{
	//����
	Clear(0,0,240,FM_TITLE_H,FM_CL_TITLE,0);
	DrawHZText12(caption,240/6, 1,1, FM_CL_BG,0);
	DrawHZText12(caption,240/6, 2,1, FM_CL_BG,0);

	//�հ�����
	Clear(0,FM_TITLE_H,240,160-FM_TITLE_H-FM_FOOT_H,FM_CL_BG,0);

	//״̬��
	Clear(0,160-FM_FOOT_H,240,FM_FOOT_H,FM_CL_GRAY,0);
}


//******************************************************************************
//**** ���ļ�����������(��ͣʹ��)
void DrawFileMngr(char *caption,char* footmsg, FM_MD_FILE* fmf, u16 oldFi, u16 newFi, u16 scrH, u16 scrY , u16 mode)
{
	u16 i,num;

	//ˢ�������ͷ
	if(mode&FM_MD_HEAD)
	{
		ClearWithBG(0,0,240,14,res.res_FILEBG,0);
		DrawHZText12(caption,240/6, 1,1, FM_CL_BG,0);
		DrawHZText12(caption,240/6, 2,1, FM_CL_BG,0);
	}

	//ˢ�������β
	if(mode&FM_MD_FOOT)
	{
		ClearWithBG(0,160-FM_FOOT_H,240,FM_FOOT_H,res.res_FILEBG,0);
		DrawHZText12(footmsg,240/6, 1,160-FM_FOOT_H+1, FM_CL_BLACK,0);
	}

	//ˢ���ļ���
	if(mode&FM_MD_FILES)
	{
		char FileStr[128];
		ClearWithBG(0,FM_TITLE_H,240-FM_SCR_W,160-FM_TITLE_H-FM_FOOT_H,res.res_FILEBG,0);
		num=(fmf->sectnum>FM_MSN)?FM_MSN:fmf->sectnum;
		for(i=0;i<num;i++)
		{
			DrawHZText12(fmf->sect[i].filename,FM_F_NAME_W/6, FM_F_NAME_X, FM_TITLE_H+2+i*FM_ROW_H, FM_CL_BLACK,0);
			if(fmf->sect[i].isDir)
				DrawFileIcon(3, FM_TITLE_H+2+i*FM_ROW_H,PROC_DIR_VIEW,0);
			else
				DrawFileIcon(3, FM_TITLE_H+2+i*FM_ROW_H,GetFileType(fmf->sect[i].filename),0);

			if(fmf->sect[i].isDir==0)
			{
				if(fmf->sect[i].filesize>999*1024)
					sprintf(FileStr,"%d,%03d KB",(fmf->sect[i].filesize/1024)/1000,(fmf->sect[i].filesize/1024)%1000);
				else if(fmf->sect[i].filesize>9999)
					sprintf(FileStr,"%d KB",fmf->sect[i].filesize/1024);
				else if(fmf->sect[i].filesize>999)
					sprintf(FileStr,"%d,%03d",fmf->sect[i].filesize/1000,fmf->sect[i].filesize%1000);
				else
					sprintf(FileStr,"%d",fmf->sect[i].filesize);
				DrawHZText12(FileStr,FM_F_SIZE_W/6, FM_F_SIZE_X2-6*strlen(FileStr), FM_TITLE_H+2+i*FM_ROW_H, FM_CL_BLACK,0);
			}
		}
	}

	//ˢ��ѡ���ļ�
	if(mode&FM_MD_FOCUS)
	{
		////ȡ�������ļ�
		char FileStr[128];
		u16 w = strlen(fmf->sect[oldFi].filename)*6+2;
		ClearWithBG(FM_F_NAME_X,FM_TITLE_H+1+oldFi*FM_ROW_H,240-FM_SCR_W-FM_F_NAME_X,FM_ROW_H-2,res.res_FILEBG,0);

		DrawHZText12(fmf->sect[oldFi].filename,FM_F_NAME_W/6, FM_F_NAME_X,FM_TITLE_H+2+oldFi*FM_ROW_H, FM_CL_BLACK,0);
		if(fmf->sect[oldFi].isDir==0)
		{
			if(fmf->sect[oldFi].filesize>999*1024)
				sprintf(FileStr,"%d,%03d KB",(fmf->sect[oldFi].filesize/1024)/1000,(fmf->sect[oldFi].filesize/1024)%1000);
			else if(fmf->sect[oldFi].filesize>9999)
				sprintf(FileStr,"%d KB",fmf->sect[oldFi].filesize/1024);
			else if(fmf->sect[oldFi].filesize>999)
				sprintf(FileStr,"%d,%03d",fmf->sect[oldFi].filesize/1000,fmf->sect[oldFi].filesize%1000);
			else
				sprintf(FileStr,"%d",fmf->sect[oldFi].filesize);
			DrawHZText12(FileStr,FM_F_SIZE_W/6, FM_F_SIZE_X2-6*strlen(FileStr),FM_TITLE_H+2+oldFi*FM_ROW_H, FM_CL_BLACK,0);
		}

		////������ʾѡ���ļ�
		w = strlen(fmf->sect[newFi].filename)*6+2;
		if(fmf->sect[newFi].isDir)
			DrawFileIcon(3, FM_TITLE_H+2+newFi*FM_ROW_H,PROC_DIR_VIEW,0);
		else
			DrawFileIcon(3, FM_TITLE_H+2+newFi*FM_ROW_H,GetFileType(fmf->sect[newFi].filename),0);
		Clear(FM_F_NAME_X,FM_TITLE_H+1+newFi*FM_ROW_H,w,FM_ROW_H-2,RGB(3,20,20),0);

		DrawHZText12(fmf->sect[newFi].filename,FM_F_NAME_W/6, FM_F_NAME_X,FM_TITLE_H+2+newFi*FM_ROW_H, FM_CL_BG,0);
		if(fmf->sect[newFi].isDir==0)
		{
			if(fmf->sect[newFi].filesize>999*1024)
				sprintf(FileStr,"%d,%03d KB",(fmf->sect[newFi].filesize/1024)/1000,(fmf->sect[newFi].filesize/1024)%1000);
			else if(fmf->sect[newFi].filesize>9999)
				sprintf(FileStr,"%d KB",fmf->sect[newFi].filesize/1024);
			else if(fmf->sect[newFi].filesize>999)
				sprintf(FileStr,"%d,%03d",fmf->sect[newFi].filesize/1000,fmf->sect[newFi].filesize%1000);
			else
				sprintf(FileStr,"%d",fmf->sect[newFi].filesize);
			DrawHZText12(FileStr,FM_F_SIZE_W/6, FM_F_SIZE_X2-6*strlen(FileStr),FM_TITLE_H+2+newFi*FM_ROW_H, FM_CL_BLACK,0);
		}
	}

	//ˢ���϶���
	if(mode&FM_MD_SCR)
	{
		ClearWithBG(240-FM_SCR_W,FM_TITLE_H,FM_SCR_W,160-FM_TITLE_H-FM_FOOT_H,res.res_FILEBG,0);
		ClearWithBG(240-FM_SCR_W,scrY,FM_SCR_W,scrH,res.res_FILEBG,0);
		DrawRect(240-FM_SCR_W,FM_TITLE_H,240-1,160-FM_FOOT_H-1,0x4);
	}	
}


//******************************************************************************
//**** �ƶ�y1��һ����h����Ļ���ݣ���y2��(ע�������ƶ��е��ص�)
void moveScreen(u16 y1,u16 y2,u16 h,u8 isDrawDirect)
{
	u16 *p;
	if(isDrawDirect)
		p = VideoBuffer;
	else
		p = Vcache;
	DMA_Copy(3,p+240*y2,p+240*y1,240*h,DMA_16NOW);
}


//******************************************************************************
//**** ���ļ��������д�ӡһ���ļ�������С��Ϣ(��ͣʹ��)
void DrawFileRow(u16 rowIndex, char* name, u32 size)
{
	char FileStr[128];
	DrawHZText12(name,FM_F_NAME_W/6, FM_F_NAME_X, FM_TITLE_H+2+rowIndex*FM_ROW_H, 0,0);
	if(size>999*1024)
		sprintf(FileStr,"%d,%03d KB",(size/1024)/1000,(size/1024)%1000);
	else if(size>9999)
		sprintf(FileStr,"%d KB",size/1024);
	else if(size>999)
		sprintf(FileStr,"%d,%03d",size/1000,size%1000);
	else
		sprintf(FileStr,"%d",size);
	DrawHZText12(FileStr,FM_F_SIZE_W/6, FM_F_SIZE_X2-6*strlen(FileStr),FM_TITLE_H+2+rowIndex*FM_ROW_H, 0,0);
}


//******************************************************************************
//**** ����Ļ����ʾJPEG�ļ�(��MODE_3ģʽ)
void ShowJPEG(u16 x, u16 y, u8* data)
{
	u16 *p = Vcache;
	JPEG_DecompressImage(data,p,240,160);
}


//******************************************************************************
//**** ָ��ͼ����������������ָ��λ�û�ͼ��
void DrawIcon(u16 x, u16 y, u8 Ii, u8 isDrawDirect)
{
	if(Ii == PROC_ZIPGBA_VIEW)
		Ii = PROC_GBA_VIEW ;
	DrawPic((u16*)res.res_DESKICON+Ii*24*24,
		x , 
		y , 
		24,
		24,
		1,
		0x7FFF,
		isDrawDirect);
}


//******************************************************************************
//**** ָ��Сͼ�����������ļ�������ָ��λ�û�Сͼ��
void DrawFileIcon(u16 x, u16 y, u8 Ii, u8 isDrawDirect)
{
	if(Ii == PROC_ZIPGBA_VIEW)
		Ii = PROC_GBA_VIEW ;
	DrawPic((u16*)res.res_FILEICON+Ii*16*14,
		x, 
		y, 
		16,
		14,
		1,
		0x7FFF,
		isDrawDirect);
}


//******************************************************************************
//**** ��ָ��ƫ����λ��д��������
void save(u32 offset, u8 *data, u16 len)
{
	//���հ��ṩ
	return WriteSram(offset,data,len);
}


//******************************************************************************
//**** ��ָ��ƫ����λ�ö���������
void load(u32 offset, u8 *data, u16 len)
{
	//���հ��ṩ
	return ReadSram(offset,data,len);
}


//******************************************************************************
//**** �򵥽���
void Dec(unsigned char *Indata, int IndataLen, unsigned char *Outdata, int *OutdataLen)
{
	//���հ��ṩ
	return;
}


//******************************************************************************
//**** ��Ļ��������
void ScreenSaver(char *bg)
{
	//���հ��ṩ
	return;
}

