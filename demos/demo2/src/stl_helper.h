/** @file stl_helper.h
 * Soubor obsahující pomocné třídy a funkce na obecné úrovni.
 */

#ifndef STL_HELPER_H
#define STL_HELPER_H

#include <iostream>
#include <string>
#include <sstream>

/** Načte ze stringu hodnotu zadané proměnné.
 * @param s string, ze kterého se čte
 * @param val proměnná, do které ukládáme přečtenou hodnotu,
 * musí pro ní být definovaný operátor>>
 * @return Vrací false, pokud nelze přečíst hodnotu.
 */
template<typename T> bool string2x(const std::string & s, T & val){
	std::stringstream ss(s);
	ss >> val;
	return !ss.fail();
}

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

#endif
