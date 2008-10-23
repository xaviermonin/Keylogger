#include "AStockage.h"

AStockage::AStockage(void)
{
}

AStockage::~AStockage(void)
{
}

/*!
* \brief Converti un string en vector<char>.
* \param[in] string_donnees : String à convertir en vector<char>
* \return vector<char> converti
*/
ListeChars AStockage::stringVersVector(const std::string &string_donnees)
{
	ListeChars vector_donnees;

	size_t taille_string=string_donnees.size();

	vector_donnees.resize(taille_string);

	// On rempli le vector<char>
	for (size_t i=0; i<string_donnees.size(); i++)
		vector_donnees[i]=string_donnees[i];

	return vector_donnees;
}

/*!
* \brief Converti un vector<char> en string.
* \param[in] vector_donnees : Vector<char> à convertir en string
* \return string converti
*/
std::string AStockage::vectorVersString(const ListeChars& vector_donnees)
{
	std::string string_donnees;

	size_t taille_vector=vector_donnees.size();

	string_donnees.resize(taille_vector);

	// On rempli le string
	for (size_t i=0; i<vector_donnees.size(); i++)
		string_donnees[i]=vector_donnees[i];

	return string_donnees;
}


/*!
* \brief Charge un objet AExtension.
* Charge un objet AExtension qui sera utilisé avant l'envoi des données.
*/
void AStockage::definirExtension(AExtension* extension)
{
	this->extension = extension;
}
