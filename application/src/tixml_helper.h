/** @file tixml_helper.h
 * Soubor obsahující pomocné třídy a funkce ke knihovně tinyXML.
 */

#ifndef TIXML_HELPER_H
#define TIXML_HELPER_H

#define TIXML_FILE_EXTENSION ".xml"
#define TIXML_FILE_PATH "xml/"

#include <iostream>
#include <string>
// #include <sstream>
#include <map>
#include <stdexcept>

#include "tinyxml.h"
#include "stl_helper.h"
#include "SDL.h"

/// Načte dokument a v něm najde RootElement, variabilně kontroluje atribut name.
TiXmlElement* TiXmlRootElement(TiXmlDocument & doc,
		std::string & filename, const std::string& rootEl_name, bool checkAttr_name);

/// Uloží dokument.
void TiXmlSaveDocument(TiXmlDocument & doc, std::string & filename);


class TiXmlException: public std::logic_error {
	public:
		TiXmlException(const std::string & what_arg):
			std::logic_error(what_arg) {}
};

/** Vytiskne chybu a ukončí program.
 * @details Na standardní chybový výstup vytiskne chybové hlášení.
 * @param error Chybová hláška, pro zadaný typ musí být definovaný operator<<
 * @throw int Hodnota pro skončení programu.
 */
template<typename T> void TiXmlError(T error){
	throw TiXmlException("Error in XML occured: "+x2string(error));
}

/** Vytiskne chybu se jménem souboru a ukončí program.
 * @details Na standardní chybový výstup vytiskne zadané jméno souboru
 * a chybové hlášení.
 * @param filename Soubor, ve kterém nastala chyba. Pro zadaný typ musí existovat operator<<
 * @param error Chybová hláška, pro zadaný typ musí být definovaný operator<<
 * @throw int Hodnota pro skončení programu.
 */
template<typename S, typename T> void TiXmlError(S filename, T error){
	throw TiXmlException(
		"Error in XML file " + x2string(filename)+
		" occured: " + x2string(error));
}

/// Načtení hodnoty atributu do stringu.
bool readStringAttr(TiXmlElement *El, const char* name, std::string & outValue);

/** Načtení hodnoty atributu do proměnné.
 * Uloží do proměnné hodnotu zadaného atributu,
 * pokud atribut nebo element neexistuje a parametr force není nastaven na false,
 * vyvolá výjimku string s chybovým hlášením,
 * pokud nelze hodnota atributu přetypovat vyvolá se vždy výjimka typu string.
 * @param El element, jehož atribut hledáme
 * @param name jméno atributu, jehož hodnotu chceme získat
 * @param outValue šablonovaná proměnná, do které se uloží hodnota nalezeného atributu
 * @param force nepovinný parametr, pokud je
 * @return Vrací TRUE při úspěchu, jinak false.
 * @throw string Vypustí výjimku, když je atribut špatného typu nebo neexistuje a je povinný.
 */
template<typename T>
		bool readAttr(TiXmlElement *El, const char* name,
				T & outValue, bool force=true){
	std::string strValue;
	if(readStringAttr(El,name, strValue)){
		if(!string2x(strValue, outValue))
			throw std::string("wrong type of attribute ")+name+
					" in element <"+El->Value()+">.";
		return true;
	}
	else if(force)
			throw std::string("missing attribute ")+name+
					" in element <"+El->Value()+">.";
	else return false;
}



/// Nalezení subelementu.
TiXmlElement* subElement(TiXmlElement *Element, const char* name);

/// Zpracování specifických atributů.
void attr_Name(TiXmlElement *El, std::string & name);
/// Zpracování specifických atributů.
void attr_HeightWidth(TiXmlElement *El, int & height, int & width);
/// Zpracování specifických atributů.
void attr_XY(TiXmlElement *El, int & x, int & y);
/// Zpracování specifických atributů.
void attr_ShadowXY(TiXmlElement *El, int & x, int & y);
/// Zpracování specifických atributů.
void attr_Count(TiXmlElement *El, int & count);
/// Zpracování specifických atributů.
void attr_SpeedLivesIntelligence(TiXmlElement *El, Uint16 & s, Uint16 & l, Uint16 & i);

#endif
