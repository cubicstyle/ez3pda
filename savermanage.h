#ifndef _savermanage_h
#define _savermanage_h

#include "agb.h"

//******************************************************************************
//**** EZ3���浵�ļ�
void CopybackSaver(u8* pCpbuf,u32 size,u32 startpage);
//******************************************************************************
//**** EZ3���浵�ļ�
u8 CheckSram() ;
u8 Checkaladdin();
u16 CheckLanguage();
//******************************************************************************
//**** EZ3�ڴ��д
void ReadSram(u32 address, u8* data , u32 size );
void WriteSram(u32 address, u8* data , u32 size );
//******************************************************************************
//**** ����һ���ļ��������ƴ浵�ļ���
void FormatSaverName(char* inname1 , char* outname,int numb);
//******************************************************************************
//**** д�뵱ǰ��Ϸ��Ϣ
void WriteSaverInfo(char* inname,int saversize,u8 where);
//******************************************************************************
//**** ����ǰ��Ϸд��浵
int SaveCurrentSaverToFlash();
//******************************************************************************
//**** ���浵�ļ�
int ReadFileToSram(char* name);
//******************************************************************************
//**** ����ĵ�
void ClearSaver(int size) ;
//******************************************************************************
//**** ����浵��ȡ�洢����
int EnterSaver(u8 mode,int sel) ;

/*
typedef struct _SAVERINFO {
char flag[32] ;
char md5code[16];
u8 isSaved ;
u8 where ;
u32 gamesize ;
u16 saversize ;
u8 reserved[7];
char gamename[176];
} SAVEFILEINFO ;
*/
typedef struct _SAVERINFO {
char flag[64] ; //������Ϸ�ĺϷ���ʾ
char md5code[16];
u32 gamesize ;	//��Ϸ��С
u8 isSaved ;
u8 where ;	//Nand��Ϸ����Nor��Ϸ
u16 saversize ; //�浵��С
u16 language ;
u8 saverforce ;
u8 reserved[5]; 
char gamename[256]; //��Ϸ��
char savername[80]; //�浵��
} SAVEFILEINFO ;

#endif //_savermanage_h
