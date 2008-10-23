/*!
* \file AExtension.h
* Fichier entête de la classe abstraite AExtension.
* \date 30/10/2008
* \author Xavier Monin
*/

#ifndef _MODIFIERDONNEES_H_
#define _MODIFIERDONNEES_H_

#include <vector>

/*!
* \class AExtension
* \brief Modification de données.
* Classe abstraite permettant la modification de données.
* \author Xavier Monin
*/

typedef std::vector<char> ListeChars;

class AExtension
{
public:
	inline AExtension() : modification_suivante(0) {}

	AExtension & operator << (const ListeChars&);

	ListeChars resultat();
	void effacerDonnees();
	inline void definirExtensionSuivante(AExtension* suivant) { modification_suivante = suivant; };
protected:
	virtual ListeChars modifier(const ListeChars &)=0;
	inline virtual ListeChars ajouter(const ListeChars & ajout){ return ajout; };
	inline virtual void effacer() {};
private:
	ListeChars donnees;

	AExtension* modification_suivante;
};

#endif