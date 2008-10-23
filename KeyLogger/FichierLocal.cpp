#include "FichierLocal.h"

#include <iostream>

FichierLocal::FichierLocal(const std::string& nom)
{
	fichier.open(nom.c_str(), std::ios_base::app);
}

FichierLocal::~FichierLocal()
{
	fichier.close();
}

void FichierLocal::envoyerDonnees(const std::string& donnees)
{
	fichier << donnees.c_str();
	fichier.flush();
}
