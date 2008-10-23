/*!
* \file ExtCrypteur.cpp
* Fichier source de la classe ExtCrypteur
* \author Xavier Monin
* \date 30/10/2008
*/

#include "ExtCrypteur.h"

#include <iostream>

/*!
* \brief Crypte les données de manière très simple.
*/
ListeChars ExtCrypteur::modifier(const ListeChars & a_crypter)
{
	ListeChars donnees_cryptees;
	donnees_cryptees.resize(a_crypter.size());

	// On crypte les données entre deux octets
	size_t nb_iteration = a_crypter.size();
	if ((a_crypter.size()%2)==1)
		nb_iteration-=1;
	for (size_t i=0; i<nb_iteration; i+=2)
	{
		unsigned char a = a_crypter[i];
		unsigned char b = a_crypter[i+1];
		// On échange les demi octets des deux octets
		echangerDemiOctet(a, b);
		
		donnees_cryptees[i]=a;
		donnees_cryptees[i+1]=b;
	}

	if ((donnees_cryptees.size()%2)==1)
		// On est dans le cas où il reste un octet non crypté
		donnees_cryptees[a_crypter.size()-1]=a_crypter[a_crypter.size()-1]; // L'octet est juste récopié, il n'est pas crypté

	return donnees_cryptees;
}

/*!
* \brief Echange 4 bits entre 2 octets.
* Echange la partie de droite du premier octet avec la partie de gauche du deuxième
* \param[in,out] a : Premier octet (modifié par référence)
* \param[in,out] b : Deuxième octet (modifié par référence)
*/
void ExtCrypteur::echangerDemiOctet(unsigned char &a, unsigned char &b)
{
	// Décalage de bits (la moitié d'un octet, donc 4 bits)
	size_t decalage=(sizeof(a)*4);

	unsigned char a_temp=a;
	unsigned char b_temp=b;

	// Exemple:		a		|		b
	// Avant:	1010 1101	|	0111 1011
	// Après:	1010 0111	|	1101 1011
	a_temp = (char)(a&0xF0)|(char)(b>>decalage); // (moitié gauche de a) concaténé à (la moitié gauche de b)
	b_temp = (char)(a<<decalage)|(char)(b&0x0F); // (moitié droite de a) concaténé à (la moitié droite de b)

	a=a_temp;
	b=b_temp;
}
