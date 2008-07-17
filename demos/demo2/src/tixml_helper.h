/** @file tixml_helper.h
 * Soubor obsahující pomocné třídy a funkce ke knihovně tinyXML.
 */

#ifndef TIXML_HELPER_
#define TIXML_HELPER_

#include <iostream>
#include <string>
#include <map>

#include "tinyxml.h"
// #include "SDL_lib.h"

/// Načte dokument a v něm najde RootElement, variabilně kontroluje atribut name.
TiXmlElement* TiXmlRootElement(TiXmlDocument & doc,
		std::string & filename, const std::string& rootEl_name, bool checkAttr_name);


/// Vytiskne chybu a ukončí program.
template<typename T> void TiXmlError(T error){
	std::cerr << "Chyba v XML souboru: " << error << std::endl;
	exit(1);
}

/// Vytiskne chybu se jménem souboru a ukončí program.
template<typename S, typename T> void TiXmlError(S filename, T error){
	std::cerr << "Chyba v XML souboru " << filename
		<< " : " << error << std::endl;
	exit(1);
}

/// Mapa číselných atributů indexovaná jménem atrobutu.
typedef std::map< std::string, int> attr_map_t;

/// Zjištění hodnoty atributu typu string.
int QueryStringAttribute(TiXmlElement *El, const char* name,
			std::string* outValue);

/// Zpracování subelementu.
void subElement(TiXmlElement *Element, const char* name, attr_map_t & attr_map);
/// Zpracování atributů typu int.
void parseIntAttributes(TiXmlElement *El, attr_map_t & attr_map);
/// Vyhledá hodnotu atributu.
bool attrIntValue(const char* attr , int & value, const attr_map_t & attr_map );


/// Zpracování specifických atributů.
void attr_Name(TiXmlElement *El, std::string & name);
/// Zpracování specifických atributů.
void attr_HeightWidth(TiXmlElement *El, int & height, int & width);
/// Zpracování specifických atributů.
void attr_XY(TiXmlElement *El, int & x, int & y);
/// Zpracování specifických atributů.
void attr_Count(TiXmlElement *El, int & count);
/// Zpracování specifických atributů.
// TODO void attr_Speed..(TiXmlElement *El, int & x, int & y);

#endif
