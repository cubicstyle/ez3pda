#ifndef	SHELL_HEADER
#define SHELL_HEADER

#include "AGB.h"
#include "global.h"

typedef struct
{
	char FileName[MAX_PATH];
	unsigned int isDir;
	unsigned int size;
	unsigned int padsize;
} RomFile;


//******************************************************************************
//**** ��Ŀ¼�ṹ��norFlash�еĽṹ��д�ڷ��������2Mλ��,ע��
//**** 
typedef struct
{
	char FileName[MAX_PATH-4];
	unsigned int filetype ;
	unsigned int rompage; //λ��
	unsigned int size;
	unsigned int saversize;
} NorRomFile;

//******************************************************************************
//**** ����һ��·���е�б����Ŀ
u16 getDirLayerNum(char* path);

//******************************************************************************
//**** ����һ��·���е��ļ���
void getDirFileName(char* path, char* filename);

//******************************************************************************
//**** ���ص�ǰĿ¼��ROM�ļ�����
u16 GetDirFileCount(char *path);

//******************************************************************************
//**** �����ļ������ţ���ָ��DIR��ROM�м���ĳ�ļ������ظ��ļ���ַ
u8* GetCurRomFile(char *path,u16 index,u8 isgoon);

//******************************************************************************
//**** �����ļ�����·����ȷ�������ظ��ļ�ָ��
u8* GetRomFileByName(char *path, char *Name);

//******************************************************************************
//**** �ļ�������������
//int runShell(void);

//******************************************************************************
//**** EZ3����Shell������
int runEz3DiskShell(void);
	int  GetFileListFromNor(void);
	u32  EnterWriteGBAtoNor() ;
	u32 FormatNor();
//******************************************************************************
//**** ������������
void StartProgress();
void StopProgress();
//******************************************************************************
//**** EZ3����ָ��Ŀѡ������
int EnterCheatCodeSelect();
//******************************************************************************
//**** EZ3��λ������
void softrest(void);
void hardrest(void);
//******************************************************************************
//**** EZ3ʱ����ʾ����
void ShowTime();
//******************************************************************************
//**** EZ3���ش浵
int LoadSaver();
int WriteSaverEx();
//******************************************************************************
//***** �����˵���
u16 popupmenu();

void WaitingErase();

//******************************************************************************
//**** ��ʾ�˵�����
//**** ָ������ͼƬΪbg������ͼƬΪtitleBG������˵��ַ�������mnu��ָ���˵�������count��Ĭ�Ͻ���defualtI
//int ShowWin_Menu(char *bg, char *titleBG, char** mnu, u8 count, u8 defaultI);
int ShowWin_Menu(char *bg, char *titleBG,char *overlap, char** mnu, u8 count, u8 defaultI);

#endif
