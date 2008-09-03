/** @file c++_helper.h
 * Soubor obsahující pomocné třídy a funkce na obecné úrovni.
 */

#ifndef CPP_HELPER_H
#define CPP_HELPER_H

#include <iostream>
#include <string>
#include <sstream>

/// Načte ze stringu hodnotu zadané proměnné.
template<typename T> bool string2x(const std::string & s, T & val){
	std::stringstream ss(s);
	ss >> val;
	return !ss.fail();
}

/// Načte ze stringu hodnotu zadané proměnné.
template<typename T> std::string x2string(const T & val){
	std::stringstream ss;
	ss << val;
	return ss.str();
}

#endif
