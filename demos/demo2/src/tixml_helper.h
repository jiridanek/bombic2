/** @file tixml_helper.h
 * Soubor obsahující pomocné třídy a funkce ke knihovně tinyXML.
 */

#ifndef TIXML_HELPER_H
#define TIXML_HELPER_H

#include <iostream>
#include <string>
#include <sstream>
#include <map>

#include "tinyxml.h"
#include "stl_helper.h"
#include "SDL_lib.h" // TODO

/// Načte dokument a v něm najde RootElement, variabilně kontroluje atribut name.
TiXmlElement* TiXmlRootElement(TiXmlDocument & doc,
		std::string & filename, const std::string& rootEl_name, bool checkAttr_name);


/// Vytiskne chybu a ukončí program.
template<typename T> void TiXmlError(T error){
	std::cerr << "Error in XML occured: " << error << std::endl;
	throw 1;
}

/// Vytiskne chybu se jménem souboru a ukončí program.
template<typename S, typename T> void TiXmlError(S filename, T error){
	std::cerr << "Error in XML file " << filename
		<< " occured: " << error << std::endl;
	throw 1;
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
