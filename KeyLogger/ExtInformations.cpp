#include "ExtInformations.h"

ExtInformations::ExtInformations()
{
	recupererInformationsUtilisateur();
}

ListeChars ExtInformations::modifier(const ListeChars & a_modifier)
{
	ListeChars modifie;
	modifie.insert(modifie.end(), informations_utilisateur.begin(), informations_utilisateur.end());
	modifie.insert(modifie.end(), a_modifier.begin(), a_modifier.end());
	return modifie;
}

void ExtInformations::recupererInformationsUtilisateur()
{
	const int ELEMENTS = 256;
	DWORD taille=ELEMENTS;
	WCHAR chaine[ELEMENTS];
	GetUserNameW(chaine, &taille);
	informations_utilisateur=LPWSTRVersString(chaine);

	informations_utilisateur+='-';

	taille=ELEMENTS;
	GetComputerNameW(chaine, &taille);
	informations_utilisateur+=LPWSTRVersString(chaine);
	informations_utilisateur+="\r\n";
}

std::string ExtInformations::LPWSTRVersString(const LPWSTR lpwstr_chaine, UINT codepage)
{
	std::string string_chaine;
    char* char_chaine = 0;
    int nb_caract;

    nb_caract = WideCharToMultiByte(codepage, 0, lpwstr_chaine,-1, 0, 0, 0, 0);
    if (nb_caract > 0)
	{
		char_chaine = new char[nb_caract];
		WideCharToMultiByte(codepage, 0, lpwstr_chaine, -1, char_chaine, nb_caract, 0, 0);
		char_chaine[nb_caract-1] = 0;
		string_chaine = char_chaine;
		delete [] char_chaine;
	}
	return string_chaine;
}