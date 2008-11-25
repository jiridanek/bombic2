/** @file menu_base.h
 * Soubor obsahuje třídy,
 * na kterých jsou postaveny jednotlivá menu.
 */

#ifndef MENU_BASE_H
#define MENU_BASE_H

#include <stack>
#include "agar_helper.h"

class MenuStack;

#define MENU_ITEM_WIDTH 300
#define MENU_OFFSET 50

class MenuBase {
	public:
		MenuBase();
		virtual ~MenuBase();
		void show();
		void hide();

		static void handlerBack(AG_Event * ev);
		static void handlerBack();

		static void clearStack();
	protected:
		void createHeading(const char * text);
		AG_Box * createItem(const char * text);

		AG_Window *win;
		static MenuStack menu_stack;
};

class MenuStack {
	public:
		void insert(MenuBase * menu);
		void remove();
		void clear();
	private:
		std::stack< MenuBase * > stack_;
};

#endif
