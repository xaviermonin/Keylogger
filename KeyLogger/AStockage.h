#ifndef _ASTOCKAGE_H_
#define _ASTOCKAGE_H_

#include "AExtension.h"

class AStockage
{
public:
	AStockage();
	virtual ~AStockage();

	virtual void envoyerDonnees(const std::string&) = 0;

	void definirExtension(AExtension*);

protected:
	std::string vectorVersString(const ListeChars &);
	ListeChars stringVersVector(const std::string &);

	//! Objet AExtension.
	AExtension* extension;
};

#endif
