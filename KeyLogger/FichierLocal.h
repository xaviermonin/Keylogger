#ifndef _FICHIER_LOCAL_H_
#define _FICHIER_LOCAL_H_

#include "AStockage.h"

#include <fstream>

class FichierLocal :
	public AStockage
{
public:
	FichierLocal(const std::string& nom);
	virtual ~FichierLocal();

	virtual void envoyerDonnees(const std::string&);

private:
	std::ofstream fichier;
};

#endif
