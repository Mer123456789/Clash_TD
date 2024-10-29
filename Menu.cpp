#include "Menu.h"

Menu::Menu(float width, float height) {
	//Chargement de la police
	if (!font.loadFromFile("Data/font/LibreBaskerville-Bold.ttf")) {
		cerr << "No font is here";
	}

	//Boutton Jouer
	textMenu[0].setFont(font);
	textMenu[0].setFillColor(Color(218, 165, 32));
	textMenu[0].setString("Jouer");
	textMenu[0].setCharacterSize(size_2);
	textMenu[0].setPosition(width / 4 - size_2, height / (Max_menu_items + 3) + 100 * 1);

	//Boutton Présentation
	textMenu[1].setFont(font);
	textMenu[1].setFillColor(Color(255, 255, 255));
	textMenu[1].setString("Mode d'emploi");
	textMenu[1].setCharacterSize(size_1);
	textMenu[1].setPosition(width / 4 - size_2, height / (Max_menu_items + 3) + 100 * 2);

	//Boutton Sortir
	textMenu[2].setFont(font);
	textMenu[2].setFillColor(Color(255, 255, 255));
	textMenu[2].setString("Quitter");
	textMenu[2].setCharacterSize(size_1);
	textMenu[2].setPosition(width / 4 - size_2, height / (Max_menu_items + 3) + 100 * 3);

	selectedItem = -1;
}
// Destructeur
Menu::~Menu() {

}
//Dessiner Le Menu
void Menu::draw(RenderWindow& window) {
	for (int i = 0; i < Max_menu_items; i++) {
		window.draw(textMenu[i]);
	}
}
//fonction MoveUp, quand on fait deffiler vers le haut
void Menu::MoveUp() {
	if (selectedItem >= 0) {
		textMenu[selectedItem].setFillColor(Color(255, 255, 255));
		textMenu[selectedItem].setCharacterSize(size_1);

		selectedItem--;
		if (selectedItem == -1) {
			selectedItem = Max_menu_items - 1;
		}
		textMenu[selectedItem].setFillColor(Color(218, 165, 32));
		textMenu[selectedItem].setCharacterSize(size_2);//augmentation de la taille quand le bouton est selectionné
	}
}
//fonction MoveDown, quand on fait deffiler vers le bas
void Menu::MoveDown() {
	if (selectedItem + 1 <= Max_menu_items) {
		textMenu[selectedItem].setFillColor(Color(255, 255, 255));
		textMenu[selectedItem].setCharacterSize(size_1);
		selectedItem++;
		if (selectedItem == Max_menu_items) {
			selectedItem = 0;
		}
		textMenu[selectedItem].setFillColor(Color(218, 165, 32));
		textMenu[selectedItem].setCharacterSize(size_2);
	}
}