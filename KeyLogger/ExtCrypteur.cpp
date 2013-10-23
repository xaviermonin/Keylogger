/*!
* \file ExtCrypteur.cpp
* Fichier source de la classe ExtCrypteur
* \author Xavier Monin
* \date 30/10/2008
*/

#include "ExtCrypteur.h"

#include <iostream>

/*!
* \brief Crypte les donn�es de mani�re tr�s simple.
*/
ListeChars ExtCrypteur::modifier(const ListeChars & a_crypter)
{
	ListeChars donnees_cryptees;
	donnees_cryptees.resize(a_crypter.size());

	// On crypte les donn�es entre deux octets
	size_t nb_iteration = a_crypter.size();
	if ((a_crypter.size()%2)==1)
		nb_iteration-=1;
	for (size_t i=0; i<nb_iteration; i+=2)
	{
		unsigned char a = a_crypter[i];
		unsigned char b = a_crypter[i+1];
		// On �change les demi octets des deux octets
		echangerDemiOctet(a, b);
		
		donnees_cryptees[i]=a;
		donnees_cryptees[i+1]=b;
	}

	if ((donnees_cryptees.size()%2)==1)
		// On est dans le cas o� il reste un octet non crypt�
		donnees_cryptees[a_crypter.size()-1]=a_crypter[a_crypter.size()-1]; // L'octet est juste r�copi�, il n'est pas crypt�

	return donnees_cryptees;
}

/*!
* \brief Echange 4 bits entre 2 octets.
* Echange la partie de droite du premier octet avec la partie de gauche du deuxi�me
* \param[in,out] a : Premier octet (modifi� par r�f�rence)
* \param[in,out] b : Deuxi�me octet (modifi� par r�f�rence)
*/
void ExtCrypteur::echangerDemiOctet(unsigned char &a, unsigned char &b)
{
	// D�calage de bits (la moiti� d'un octet, donc 4 bits)
	size_t decalage=(sizeof(a)*4);

	unsigned char a_temp=a;
	unsigned char b_temp=b;

	// Exemple:		a		|		b
	// Avant:	1010 1101	|	0111 1011
	// Apr�s:	1010 0111	|	1101 1011
	a_temp = (char)(a&0xF0)|(char)(b>>decalage); // (moiti� gauche de a) concat�n� � (la moiti� gauche de b)
	b_temp = (char)(a<<decalage)|(char)(b&0x0F); // (moiti� droite de a) concat�n� � (la moiti� droite de b)

	a=a_temp;
	b=b_temp;
}
