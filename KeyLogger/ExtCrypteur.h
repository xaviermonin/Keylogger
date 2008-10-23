/*!
* \file ExtCrypteur.h
* Fichier entête de la classe ExtCrypteur
* \date 30/10/2008
* \author Xavier Monin
*/

#ifndef _CRYPTERDONNEES_H_
#define _CRYPTERDONNEES_H_

#include "AExtension.h"

/*!
* \class ExtCrypteur
* \brief Crypte des données
* \author Xavier Monin
*/
class ExtCrypteur : public AExtension
{
protected:
	virtual ListeChars modifier(const ListeChars &);
private:
	void echangerDemiOctet(unsigned char &, unsigned char &);
};

#endif