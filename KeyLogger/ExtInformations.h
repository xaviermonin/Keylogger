#ifndef _AJOUTERENTETEINFORMATIONS_H_
#define _AJOUTERENTETEINFORMATIONS_H_

#include "AExtension.h"
#include <windows.h>

class ExtInformations : public AExtension
{
public:
	ExtInformations();
protected:
	virtual ListeChars modifier(const ListeChars &);
private:
	void recupererInformationsUtilisateur();
	std::string LPWSTRVersString(const LPWSTR, UINT codepage=CP_ACP);

	//! Nom de l'utilisateur de l'ordinateur.
	std::string informations_utilisateur;
};

#endif