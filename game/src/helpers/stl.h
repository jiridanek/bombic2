/** @file stl.h
 * Soubor obsahující pomocné třídy a funkce na obecné úrovni.
 */

#ifndef STL_HELPER_H
#define STL_HELPER_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

/** Načte ze stringu hodnotu zadané proměnné.
 * @param s string, ze kterého se čte
 * @param val proměnná, do které ukládáme přečtenou hodnotu,
 * musí pro ní být definovaný operátor>>
 * @return Vrací false, pokud nelze přečíst hodnotu.
 */
template<typename T> bool string2x(const std::string & s, T & val){
	std::stringstream ss(s);
	ss >> val;
	return !ss.fail() && ss.eof();
}

bool string2x(const std::string & s, std::string & val);

/** Překonvertuje zadanou proměnnou na string.
 * @param val proměnná, jejíž hodnota se konvertuje,
 * musí pro ní být definovaný operátor<<
 * @return Vrací string se zapsanou hodnotou.
 */
template<typename T> std::string x2string(const T & val){
	std::stringstream ss;
	ss << val;
	return ss.str();
}

/// Rozdělení stringu do částí.
void explode(const std::string & str, char delim,
		std::vector<std::string> & parts);

/** Znaménko rozdílu.
 * požadavky na parametry:
 * jednosměrná konverze
 * bool operator<(T, T)
 * @param a menšenec
 * @param b menšitel
 * @return Vrací znaménko rozdílu a-b.
 */
template<typename T> int sgn_minus(T a, T b){
	if(a < b){
		return -1;
	} else if(b < a){
		return 1;
	} else {
		return 0;
	}
}

/** Absolutní hodnota rozdílu.
 * požadavky na parametry:
 * jednosměrná konverze
 * bool operator<(T, T)
 * T operator-(T, T)
 * @param a menšenec
 * @param b menšitel
 * @return Vrací kladný rozdíl.
 */
template<typename T> T abs_minus(T a, T b){
	return a < b ? b - a : a - b;
}


#endif
