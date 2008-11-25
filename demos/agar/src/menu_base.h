/** @file menu_base.h
 * Soubor obsahuje třídy,
 * na kterých jsou postaveny jednotlivá menu.
 */

#ifndef MENU_BASE_H
#define MENU_BASE_H

#include <vector>
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

		static void clearStack();

		static void handlerBack(AG_Event * ev=0);
		static void handlerItems(AG_Event * event);
		static Uint16 activeItem();
		static void handlerIntItem(AG_Event * event);
		static void handlerBoolItem(AG_Event * event);
	protected:
		void createHeading(const char * text);
		AG_Box * createItem(const char * text);
		AG_Box * createItemHoriz(const char * text);

		AG_Window *win;

		typedef std::vector< AG_Box * > items_t;
		items_t items_;
		static items_t items;

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
