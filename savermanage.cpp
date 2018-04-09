#include "stdio.h"
#include "string.h"
#include "savermanage.h"
#include "md5.h"
#include "inram.h"
#include "hard.h"
#include "fat16.h"
#include "bgfunc.h"
#include "keypad.h"
#include "shell.h"
#include "global.h"
#include "lang.h"

//
//******************************************************************************
//**** EZ3�ڴ��д
void ReadSram(u32 address, u8* data , u32 size )
{
	register int i ;
	u16* pData = (u16*)data;
	for(i=0;i<size;i+=2)
	{
		pData[i>>1]=*(u8*)(address+i)+(*(u8*)(address+i+1)*0x100);
	}
}

void WriteSram(u32 address, u8* data , u32 size )
{
	register int i ;
	for(i=0;i<size;i++)
		*(u8*)(address+i)=data[i];
}

void CopybackSaver(u8* pCpbuf,u32 size,u32 startpage)
{
	u32 left =0 , page = 0 ;

//---��ѭ������ȡsize��С	
	do
	{
		SetRampage(startpage+page);
		ReadSram(0xE000000,pCpbuf,64*1024);
		left += 64*1024 ;	
		page += 0x10 ;
		pCpbuf +=  64*1024 ;
	}
	while(left < size) ;
//-����	
/*
	if(size>64*1024)
	{
		ReadSram(0xE000000,pCpbuf,64*1024);
		SetRampage(startpage+16);
		left = size - 64*1024 ;
		ReadSram(0xE000000,pCpbuf+64*1024,left);
	}
	else
		ReadSram(0xE000000,pCpbuf,size);
*/	
	SetRampage(0);	
}

u8 Checkaladdin()
{
	SAVEFILEINFO saverinfo ;
	char EZSAVER[]="EZFlash Saver FileSystem-aladdin =!@#$%^&*(){}[]:-<>,.?=OVER+END" ;
	SetRampage(0);	
	//const char EZSAVER[]="EZFlash Saver FileSystem-aladdin" ;
	
	ReadSram(_FileInfoSaver,(u8*)&saverinfo,sizeof(SAVEFILEINFO));
	if(memcmp(saverinfo.flag,EZSAVER,64))
	{//��ͬ��������߸ոս��롣
		return 1 ;
	}
	else
		return 0 ;
}
u16 CheckLanguage()
{
	u16 lan ;
	ReadSram(0x0E0081F0,(u8*)&lan,2) ;
	return lan ;	
}

u8 CheckSram()
{//����������Ҫ����RAM��NAND������ֵΪ�ڼ�����Ϸ��
 //����0����ʾʲô��û�иı䣬ֱ���˳���
	MD5_CTX context;
	SAVEFILEINFO saverinfo ;
	char EZSAVER[]="EZFlash Saver FileSystem-aladdin =!@#$%^&*(){}[]:-<>,.?=OVER+END" ;
 	char buf[36];
 	int  size = 0 , i =0;
 	keypad keys ;
	SetRampage(0);	
//const char EZSAVER[]="EZFlash Saver FileSystem-aladdin" ;
	ReadSram(_FileInfoSaver,(u8*)&saverinfo,sizeof(SAVEFILEINFO));
	if(memcmp(saverinfo.flag,EZSAVER,64))
	{//��ͬ��������߸ոս��롣
		//д������,��ʼ
		CreateWindow(gl_warning,gl_firstin,60,40,1);
		while(1){
			sleep(15);	keys.update();
			if(keys.release(KEY_A)||keys.release(KEY_B))	break;				
		}	
		memset(&saverinfo,0,sizeof(SAVEFILEINFO));
		memcpy(saverinfo.flag,EZSAVER,64);
		saverinfo.saverforce = 1 ;
		WriteSram(_FileInfoSaver,(u8*)&saverinfo,sizeof(SAVEFILEINFO));
		return 0;
	}
	else
	{
		size = saverinfo.saversize *4096 ;
		CopybackSaver((u8*)_ramFatCache,size,16);
		
		MD5Init(&context);
		MD5Update(&context, (u8*)_ramFatCache, size);
		MD5Final((u8*)buf, &context);
		for(i=0;i<16;i++)
			buf[16+i] = saverinfo.md5code[i];
		if(memcmp(buf,buf+16,16))
		{//����ȣ���Ҫ���ݴ浵����
			return 1 ;
		}
		else
		{//��ȣ��浵����Ҫ����
			return 0 ;
		}
	}
}

int SaveCurrentSaverToFlash()
{
	MD5_CTX context;
	keypad keys ;
	SAVEFILEINFO saverinfo ;
	char buf[256] ;
	char savername[512] ;
	int get,handle;
	
 	SetRampage(0);
	ReadSram(_FileInfoSaver,(u8*)&saverinfo,sizeof(SAVEFILEINFO));
	int size = saverinfo.saversize * 4096 ;
	//���ļ���
	if(size ==0)
		return -1 ;
	
	FormatSaverName(saverinfo.gamename,savername,0);
 	//�����ݱ��ݣ�ͬʱ����md5��
 	OpenWrite();
 	CopybackSaver((u8*)0x8800000,size,16);
 	CloseWrite();
	MD5Init(&context);
	MD5Update(&context, (u8*)0x8800000, size);
	MD5Final((u8*)buf, &context);
	memcpy(saverinfo.md5code,buf,16);
	saverinfo.isSaved = 1 ;

	//׼��������д��sram��
	get = fat_init(1);
	if(get)
	{
		CreateWindow(gl_warning,gl_faterror,60,40,1);
		while(1){
			sleep(15);	keys.update();
			if(keys.press(KEY_A)||keys.release(KEY_B))	break;				
		}
		return -3;
	}
	get = fat_getfirst("\\Saver",buf);
	if(get)
	{//����һ��dir
		fat_mkdir("\\Saver");
	}
	
	memcpy(buf,savername,256);
	sprintf(savername,"\\Saver\\%s",buf);
	sprintf(buf,"%s%s[%dKB]",gl_writing,savername,size/1024);
	CreateWindow(gl_waiting,buf,60,40,1);
	
	handle = fat_creat(savername,ATTR_ARCHIVE);
	if(handle<0)
		handle =fat_open(savername);
	fat_write(handle,(char*)0x8800000,size);
	fat_close(handle);
	fat_deinit();
	
//���д����ġ�
	WriteSram(_FileInfoSaver,(u8*)&saverinfo ,sizeof(SAVEFILEINFO));

	return 0 ;
}

void FormatSaverName(char* inname1 , char* outname,int numb)
{
	char *pt ;
	char  pp[4] ;
	strcpy(outname,inname1);
	pt = strrchr(outname,'.');
	if(numb>0)
	{
		
		sprintf(pp,"%02d",numb);
		pt[1] = 's';
		pt[2] = pp[0];
		pt[3] = pp[1];
		pt[4] = 0;
	}
	else
	{
		pt[1] = 's';
		pt[2] = 'a';
		pt[3] = 'v';
		pt[4] = 0;
	}
/*
	char inname[54];
	int i = 0 ;
	memcpy(inname,inname1,54);
	char *p = strrchr(inname,'.');
	if(p)
		p[0] = 0 ;
	int len = strlen(inname);
	for(i=0;i<len;i++)
	{
		if((inname[i] == ' ')||(inname[i] == '.')||(inname[i] == '+')||(inname[i] == '&'))
			inname[i] = '_' ;
	}
	if(len>8)
		inname[8]=0 ;
	sprintf(outname,"%s.sav",inname);
	p = strrchr(outname,'.');
	if(p)
	{
		p++ ;
		if(numb>0)
		{
			sprintf(inname,"%02d",numb);
			
			p[1] = inname[0] ;
			p[2] = inname[1] ;
		}
	}*/
}

void WriteSaverInfo(char* inname,int saversize,u8 where)
{
	SAVEFILEINFO saverinfo ;
	SetRampage(0);	
	ReadSram(_FileInfoSaver,(u8*)&saverinfo,sizeof(SAVEFILEINFO));
	sprintf(saverinfo.gamename,"%s",inname);
	saverinfo.isSaved = 0 ;
	saverinfo.where = where ;
	saverinfo.saversize = saversize ;
	WriteSram(_FileInfoSaver,(u8*)&saverinfo,sizeof(SAVEFILEINFO));
}

//��ȡnand Flash���Ĵ浵
int ReadFileToSram(char* name)
{
	MD5_CTX context;
	_stat filestat ;
 	char *pRam = (char*)_FileInfoSaver ;
 	u8 *pRead = (u8*)_ramFatBlockCache;
 	char buf[64];
  	char savername[64] ;
 	int get =0,handle=0,left=0 , vfsize = 0,rpage = 0 ;
 	keypad keys ;
	//׼����������
	get = fat_init(1);
	if(get)
	{
		CreateWindow(gl_warning,gl_faterror,60,40,1);
		while(1){
			sleep(15);	keys.update();
			if(keys.press(KEY_A)||keys.release(KEY_B))	break;				
		}
		return get ;
	}
	sprintf(savername,"\\Saver\\%s",name);
	sprintf(buf,"%s%s",gl_saverfile,savername);	
	CreateWindow(gl_waiting,buf,60,40,1);
	handle =fat_open(savername);
	if(handle<0)
	{
		return handle ;
	}
	fat_get_stat(savername,&filestat);
	MD5Init(&context);
	left = 0 ;
	rpage = 0 ;
	do
	{
		if((filestat.FileSize - left)>63336)
			vfsize = 65536 ;
		else
			vfsize = (filestat.FileSize - left) ;
		fat_read(handle,pRead,vfsize);
		MD5Update(&context,(u8*)pRead,vfsize);
		
		SetRampage(16+rpage);		
		WriteSram(_SRAMSaver,pRead,vfsize);
		left += 64*1024 ;
		rpage += 0x10 ;
	}
	while(left< filestat.FileSize) ;
	
	MD5Final((u8*)buf, &context);
	fat_close_withouwrit(handle);
		
	SetRampage(0);	
	//�����е�md5У����������sram��
	SAVEFILEINFO saverinfo ;
	WriteSram(_FileInfoSaver+64,(u8*)buf,16);
	return 0 ;
}

void ClearSaver(int size)
{
	int left ;
 	u8 *pRead = (u8*)_ramFatBlockCache;
 	DmaClear(3,0,_ramFatBlockCache,0x20000,32);
	SetRampage(16);
	if(size>64*1024)
	{
		WriteSram(_SRAMSaver,pRead,64*1024);
		SetRampage(32);
		left = size - 64*1024 ;
		WriteSram(_SRAMSaver,pRead+64*1024,left);
	}
	else
		WriteSram(_SRAMSaver,pRead,size);
	
	SetRampage(0);
}
// ����浵����
int EnterSaver(u8 mode,int sel)
{
//u8 =0 ��ʾ��ȡ�浵 =1 ��ʾ�洢�浵
//����ֵΪ-1����ʾȡ��������>=0��ʾ��ѡ��Ĵ浵����
	FILE_SAVE_INFO fsi;
	char *saverspace[5] ;
	int ct =0 ;
	saverspace[0]=(char *)_UnusedEram;
	saverspace[1]=(char *)_UnusedEram+64;
	saverspace[2]=(char *)_UnusedEram+64*2;
	saverspace[3]=(char *)_UnusedEram+64*3;
	saverspace[4]=(char *)_UnusedEram+64*4;
	int func_i ;
//�����浵����������
	if(!mode)
	{
		load(SAVETXTPOS,(u8*)&fsi,sizeof(FILE_SAVE_INFO));
		if(fsi.flg=='eztx')
			sprintf(saverspace[ct++],"Auto - %s",fsi.fname);
		else
			sprintf(saverspace[ct++],"Auto - Unused");
	}		
//�ڶ����浵		
	load(_txtSaver1,(u8*)&fsi,sizeof(FILE_SAVE_INFO));
	if(fsi.flg=='eztx')
		sprintf(saverspace[ct++],"saver1 - %s",fsi.fname);
	else
		sprintf(saverspace[ct++],"saver1 - Unused");
//�ڶ����浵		
	load(_txtSaver2,(u8*)&fsi,sizeof(FILE_SAVE_INFO));
	if(fsi.flg=='eztx')
		sprintf(saverspace[ct++],"saver2 - %s",fsi.fname);
	else
		sprintf(saverspace[ct++],"saver2 - Unused");
//�ڶ����浵		
	load(_txtSaver3,(u8*)&fsi,sizeof(FILE_SAVE_INFO));
	if(fsi.flg=='eztx')
		sprintf(saverspace[ct++],"saver3 - %s",fsi.fname);
	else
		sprintf(saverspace[ct++],"saver3 - Unused");
//�ڶ����浵		
	load(_txtSaver4,(u8*)&fsi,sizeof(FILE_SAVE_INFO));
	if(fsi.flg=='eztx')
		sprintf(saverspace[ct++],"saver4 - %s",fsi.fname);
	else
		sprintf(saverspace[ct++],"saver4 - Unused");

	if(mode)
	{
		func_i = ShowWin_Menu("ezoption.bmp","","txtSaveSaver.bmp",(char**)saverspace,ct,sel);
	}
	else
	{
		func_i = ShowWin_Menu("ezoption.bmp","","TXTLoadSaver.bmp",(char**)saverspace,ct,sel);
	}
	return 	func_i ;
}