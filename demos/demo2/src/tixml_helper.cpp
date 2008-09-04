
#include <iostream>
#include <string>

#include "tixml_helper.h"

using namespace std;

/**
 * @details Otevře xml dokument, zkontroluje správnost root elementu,
 * variabilně zkontroluje správnost atributu name proti filename.
 * Pokud kontrola selže, vyvolá TiXmlError().
 * Filename obalí cestou a příponou.
 * @param doc prázdný TiXmlDocument
 * @param filename jméno souboru pro otevření, zadávejte bez cesty a bez přípony
 * @param rootEl_name vyžádané jméno root elementu, pokud je prázdné nekontrokluje se
 * @param checkAttr_name pokud true, provede kontrolu atributu name
 * @return Vrací root element.
 * @see TiXmlError()
 */
TiXmlElement* TiXmlRootElement(TiXmlDocument & doc, std::string & filename,
				const std::string& rootEl_name, bool checkAttr_name){
	// obalit cestou a priponou
	string name("xml/"+filename+".xml");
	// prohodit
	swap(name, filename);

	if(!doc.LoadFile(filename.c_str()))
		TiXmlError(filename,"can't read XML file or syntactic error occured.");

	TiXmlElement *rootEl;

	if(rootEl_name.empty())
		rootEl = doc.RootElement();
	else { // kontrola root elementu
		rootEl= doc.FirstChildElement(rootEl_name.c_str());
		if(!rootEl)
			TiXmlError(filename,"bad root element, it may be <"+rootEl_name+" ...>.");
	}
	// zkontrolovat atribut name
	if(checkAttr_name){
		string str;
		if(!readStringAttr(rootEl, "name", str))
			TiXmlError(filename, "missing attribute name.");
		else if(str!=name)
			TiXmlError(filename,"bad value of attribute name");
	}
	return rootEl;
}


/** @details
 * Vyhledá atribut požadovaného jména,
 * uloží jeho hodnotu typu string.
 * @param El element, v němž se hledá atribut
 * @param name jméno hledaného atributu
 * @param outValue reference na string, do kterého se uloží nalezená hodnota
 * @return Vrací false pokud element nebo jeho atribut neexistuje, jinak true.
 */
bool readStringAttr(TiXmlElement *El, const char* name,
			std::string & outValue){
	if(!El) return false;
	const char* pStr;
	pStr=El->Attribute(name);
	if(!pStr) return false;
	outValue= pStr;
	return true;
}

/** @details
 * Najde požadovaný vnořený element.
 * Při chybě vyvolá výjimku typu string s popisem chyby.
 * @param Element , v němž hledáme vnořený element
 * @param subEl_name název hledaného elementu
 * @param attr_map mapa zpracovaných atributů
 * @return Vrací subelement.
 */
TiXmlElement* subElement(TiXmlElement *Element, const char* subEl_name){
	TiXmlElement *subEl= Element->FirstChildElement(subEl_name);
	if(!subEl)
		throw "missing element <"+string(subEl_name)+">.";
	return subEl;
}

/**
 * @details
 * Naplní parametry mimo prvního hodnotami atributů.
 * Pokud atribut nenajde uloží do parametru empty string.
 * Pokud je atribut prázdný vyvolá výjimku s chybovou hláškou.
 * @param El element jehož hodnoty atributů hledáme
 * @param name hodnota stejnojmenného atributu nebo prázdny string
 */
void attr_Name(TiXmlElement *El, std::string & name){
	if(!El || !readStringAttr(El,"name", name) ){
		name.erase();
		return;
	}
	if(name.empty())
		throw string("empty value of attribute name.");
}

/**
 * @details
 * Naplní parametry mimo prvního hodnotami atributů.
 * Pokud atribut nenajde uloží do parametru -1.
 * Pokud je atribut prázdný nebo špatného typu vyvolá výjimku s chybovou hláškou.
 * @param El element jehož hodnoty atributů hledáme
 * @param height hodnota stejnojmenného atributu nebo -1
 * @param width hodnota stejnojmenného atributu nebo -1
 */
void attr_HeightWidth(TiXmlElement *El, int & height, int & width){
	if(!readAttr(El, "height", height, false))
		height = -1;
	else if(height<=0)
		throw string("value of attribute height must be higher than 0.");

	if(!readAttr(El, "width", width, false))
		width = -1;
	else if(width<=0)
		throw string("value of attribute width must be higher than 0.");
}

/**
 * @details
 * Naplní parametry mimo prvního hodnotami adekvátních atributů.
 * Kontroluje také zda je parametr přítomen.
 * Pokud atribut chybí nebo je špatného typu, vyvolá výjimku s chybovou hláškou.
 * @param El element jehož hodnoty atributů hledáme
 * @param x hodnota atributu x
 * @param y hodnota atributu y
 */
void attr_XY(TiXmlElement *El, int & x, int & y){
	if(!El){
		x = y = -1;
		return;
	}

	readAttr(El,"x", x);
	if(x<0)
		throw string("value of attribute x must be higher than 0 or equals.");
	readAttr(El,"y", y);
	if(y<0)
		throw string("value of attribute y must be higher than 0 or equals.");
}

/**
 * @details
 * Naplní parametry mimo prvního hodnotami adekvátních atributů.
 * Kontroluje také zda je parametr přítomen.
 * Pokud atribut chybí nebo je špatného typu, vyvolá výjimku s chybovou hláškou.
 * @param El element jehož hodnoty atributů hledáme
 * @param x hodnota atributu x
 * @param y hodnota atributu y
 */
void attr_ShadowXY(TiXmlElement *El, int & x, int & y){
	if(!El){
		x = y = -1;
		return;
	}

	readAttr(El,"shadow_x", x);
	if(x<0)
		throw string("value of attribute shadow_x must be higher than 0 or equals.");
	readAttr(El,"shadow_y", y);
	if(y<0)
		throw string("value of attribute shadow_y must be higher than 0 or equals.");
}

/**
 * @details
 * Naplní parametry mimo prvního hodnotami adekvátních atributů.
 * Pokud atribut chybí nebo je špatného typu, vyvolá výjimku s chybovou hláškou.
 * @param El element jehož hodnoty atributů hledáme
 * @param count hodnota atributu count
 */
void attr_Count(TiXmlElement *El, int & count){
	if(!El){
		count = -1;
		return;
	}
	readAttr(El,"count", count);
	if(count<1)
		throw string("attribute count must be higher than 0.");
}

/**
 * @details
 * Naplní parametry mimo prvního hodnotami adekvátních atributů.
 * Pokud atribut chybí nebo je špatného typu, vyvolá výjimku s chybovou hláškou.
 * @param El element jehož hodnoty atributů hledáme
 * @param speed hodnota atributu speed
 * @param lives hodnota atributu lives
 * @param intelligence hodnota atributu intelligence
 */
void attr_SpeedLivesIntelligence(TiXmlElement *El,
			Uint16 & speed, Uint16 & lives, Uint16 & intelligence){
	if(!El){
		speed = lives = intelligence = 0;
		return;
	}

	readAttr(El,"speed", speed);
	if(speed==0)
		throw string("attribute speed must be higher than 0.");
	readAttr(El,"lives", lives);
	if(lives==0)
		throw string("attribute lives must be higher than 0.");
	readAttr(El,"intelligence", intelligence);
}
