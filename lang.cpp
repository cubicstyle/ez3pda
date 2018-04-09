#include "lang.h"

char*	gl_warning ;
char*	gl_tip ;
char*	gl_lskip ;
char*	gl_faterror ;
char*	gl_nofile ;
char*	gl_unkownType ;
char*	gl_waitread ;
char*	gl_fileerror ;
char*	gl_showprog ;
char*	gl_op_error ;
char*	gl_golderror ;
char*	gl_readsaver ;
char*	gl_saverfile ;
char*	gl_waiting ;
char*	gl_writing ;
char*	gl_firstin ;
char*   gl_nosaver;
char*  gl_flashwrite ;
char* gl_checkingsaver;
/*********blowfish add for NOR OP**********/
char*   g1_norDelete;
char*   g1_norSpace;
char*   g1_norWrite;
char*	g1_norErasing;
char*   g1_norWriting;
char*	gl_norformat;
char*   gl_psramout;
char*	gl_nosupport;
/*********blowfish add for NOR OP**********/
char**  gl_hint_content;
char**  gl_hint_title;
char**  gl_option_set ;

#define ICON_COUNT	7
#define OPTION_CNT		2
extern const char *hint_content[ICON_COUNT] ;
extern const char *hint_content_en[ICON_COUNT] ;
extern const char *hint_title[ICON_COUNT] ;
extern const char *hint_title_en[ICON_COUNT] ;
extern const char *option_set_ch[OPTION_CNT] ;
extern const char *option_set_en[OPTION_CNT] ;


//����
const char zh_warning[]="����";
const char zh_tip[]="��ʾ";
const char zh_lskip[]="����L�����浵����";
const char zh_faterror[]="FAT�ļ�ϵͳ����!";
const char zh_nofile[]="�޴��ļ�!";
const char zh_unkownType[]="δ֪�ļ�����,�򿪸��ļ�Y/N";
const char zh_waitread[]="��ȴ��ĵ��Ķ�ȡ";
const char zh_fileerror[]="�ļ���ʽ����";
const char zh_showprog[]="��ǰ������ʾ";
const char zh_op_error[]="δ֪������";
const char zh_golderror[]="����ָ����!����=A ����B";
const char zh_readsaver[]="��ȡ�浵,ԭ�浵������Y/N";
const char zh_saverfile[]="�浵�ļ�";
const char zh_waiting[]="�ȴ�";
const char zh_writing[]="д��";
const char zh_firstin[]="�������ϵͳ�ոս���";
const char zh_nosaver[]="�Բ�������ǰû�����ô��̵�!";
const char zh_writeflash[]="���Ѵ�256M��Ϸд��Flash,Y/N";
const char zh_norDelete[]="ɾ��Y/N";
const char zh_norFormat[]="��ʽ��Flash��";
const char zh_psramout[]="û���㹻��PSRAM�ռ�";
const char zh_nosupport[]="�˿���֧��GBA��Ϸ";
const char zh_norSpace[]="û���㹻��FLASH�ռ�";
const char zh_norWrite[]="д��NOR Flash Y/N";
const char zh_norErasing[]="���ڲ���";
const char zh_norWriting[]="����д";
const char zh_checkingsaver[] = "���浵" ;

//Ӣ��
const char en_warning[]="WARNING";
const char en_tip[]="TIP";
const char en_lskip[]="Hold L to skip Saving";
const char en_faterror[]="FAT Filesystem Error";
const char en_nofile[]="No such File";
const char en_unkownType[]="Unknown type Open or not ?";
const char en_waitread[]="Waiting for reading";
const char en_fileerror[]="File format error";
const char en_showprog[]="progressing ";
const char en_op_error[]="Unkonwn operation";
const char en_golderror[]="Cheat error!continue=A returnB";
const char en_readsaver[]="Read saver to current";
const char en_saverfile[]="Save file";
const char en_waiting[]="waiting";
const char en_writing[]="writing";
const char en_firstin[]="Battery is dry or first in";
const char en_nosaver[]="sorry,no saver has been saved";
const char en_writeflash[]="A 256MROM be writen to Flash,Y/N";
const char en_norDelete[]="Delete Y/N";
const char en_norFormat[]="Format Nor Flash Y/N"; 
const char en_psramout[]="PSRAM is not enough";
const char en_norSpace[]="NOR Space is not Enough";
const char en_nosupport[]="This card do not support GBA game";
const char en_norWrite[]="Write NOR Flash Y/N";
const char en_norErasing[]="Erasing...";
const char en_norWriting[]="Writing...";
const char en_checkingsaver[] = "Checking Saver" ;
/*
//����
const char en_warning[]="ATTENTION";
const char en_tip[]="ASTUCE";
const char en_lskip[]="Enfoncer L pour ne pas sauvegarder";
const char en_faterror[]="Erreur systeme de fichier FATr";
const char en_nofile[]="Fichier introuvable";
const char en_unkownType[]="Type inconnu Ouvrir ou non?";
const char en_waitread[]="Lecture en cours";
const char en_fileerror[]="Erreur format de fichier";
const char en_showprog[]="en cours ";
const char en_op_error[]="Operation inconnue";
const char en_golderror[]="Erreur cheat!Continuer=A Retour=B";
const char en_readsaver[]="Charger slot1";
const char en_saverfile[]="Sauvegarder";
const char en_waiting[]="En attente";
const char en_writing[]="ecriture";
const char en_firstin[]="Batterie neuve ou HS";
const char en_nosaver[]="Desole rien n'a ete sauvegarde";
*/

void LoadChinese()
{
	gl_warning = (char*)zh_warning ;
	gl_tip = (char*)zh_tip ;
	gl_lskip = (char*)zh_lskip ;
	gl_faterror = (char*)zh_faterror ;
	gl_nofile = (char*)zh_nofile;
	gl_unkownType = (char*)zh_unkownType ;
	gl_waitread = (char*)zh_waitread ;
	gl_fileerror = (char*)zh_fileerror ;
	gl_showprog = (char*)zh_showprog;
	gl_op_error = (char*)zh_op_error ; 
	gl_golderror = (char*)zh_golderror ;
	gl_readsaver = (char*)zh_readsaver ;
	gl_saverfile = (char*)zh_saverfile ;
	gl_waiting = (char*)zh_waiting ;
	gl_writing = (char*)zh_writing ;
	gl_firstin = (char*)zh_firstin ;
	gl_nosaver = (char*)zh_nosaver;
	gl_flashwrite = (char*)zh_writeflash;
	
	g1_norDelete = (char *)zh_norDelete;
	gl_norformat = (char *)zh_norFormat;
	gl_psramout = (char *)zh_psramout;
	gl_nosupport = (char *)zh_nosupport;
	g1_norSpace = (char *)zh_norSpace;
	g1_norWrite = (char *)zh_norWrite;
	g1_norErasing = (char *)zh_norErasing;
	g1_norWriting = (char *)zh_norWriting;
	gl_checkingsaver = (char*)zh_checkingsaver ;
	
	gl_hint_title = (char**)hint_title ;
	gl_hint_content = (char**)hint_content;
	gl_option_set = (char**)option_set_ch ;
}

void LoadEnglish()
{
	gl_warning = (char*)en_warning ;
	gl_tip = (char*)en_tip ;
	gl_lskip = (char*)en_lskip ;
	gl_faterror = (char*)en_faterror ;
	gl_nofile = (char*)en_nofile;
	gl_unkownType = (char*)en_unkownType ;
	gl_waitread = (char*)en_waitread ;
	gl_fileerror = (char*)en_fileerror ;
	gl_showprog = (char*)en_showprog;
	gl_op_error = (char*)en_op_error ;
	gl_golderror = (char*)en_golderror ;
	gl_readsaver = (char*)en_readsaver ;
	gl_saverfile = (char*)en_saverfile ;
	gl_waiting = (char*)en_waiting ;
	gl_writing = (char*)en_writing ;
	gl_firstin = (char*)en_firstin ;
	gl_nosaver = (char*)en_nosaver;
	gl_flashwrite = (char*)en_writeflash;

	g1_norDelete = (char *)en_norDelete;
	gl_norformat = (char *)en_norFormat;
	gl_psramout = (char *)en_psramout;
	gl_nosupport = (char *)en_nosupport;
	g1_norSpace = (char *)en_norSpace;
	g1_norWrite = (char *)en_norWrite;
	g1_norErasing = (char *)en_norErasing;
	g1_norWriting = (char *)en_norWriting;
	gl_checkingsaver = (char*)en_checkingsaver ;
	
	gl_hint_title = (char**)hint_title_en ;
	gl_hint_content = (char**)hint_content_en;
	gl_option_set = (char**)option_set_en ;
}
