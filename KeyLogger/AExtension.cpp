#include "AExtension.h"

ListeChars AExtension::resultat()
{
	ListeChars tmp = donnees;
	tmp = modifier(tmp);
	if (modification_suivante)
	{
		(*modification_suivante) << tmp;
		tmp = modification_suivante->resultat();
	}
	effacerDonnees();
	return tmp;
}

void AExtension::effacerDonnees()
{
	effacer();
	donnees.clear();
}

/*!
* \brief Ajoute des données à modifier.
* \param[in] ajout : Tableau de char à ajouter.
* \return L'objet AExtension courant.
*/
AExtension & AExtension::operator <<(const ListeChars & ajout)
{
	ListeChars tmp = ajouter(ajout);
	donnees.insert(donnees.end(), tmp.begin(), tmp.end());
	return (*this);
}