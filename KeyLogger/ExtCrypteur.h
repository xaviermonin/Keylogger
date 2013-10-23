/*!
* \file ExtCrypteur.h
* Fichier ent�te de la classe ExtCrypteur
* \date 30/10/2008
* \author Xavier Monin
*/

#ifndef _CRYPTERDONNEES_H_
#define _CRYPTERDONNEES_H_

#include "AExtension.h"

/*!
* \class ExtCrypteur
* \brief Crypte des donn�es
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