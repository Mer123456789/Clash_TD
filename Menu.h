#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

using namespace std;
using namespace sf;

//definir le nb d'options dans le menu
#define Max_menu_items 3

class Menu
{
public:
	Menu(float width, float height);

	void draw(RenderWindow& window);
	void MoveUp();
	void MoveDown();

	int MenuPressed() {
		return 	selectedItem;
	}

	~Menu();

private:
	Font font;
	Text textMenu[Max_menu_items];

	int selectedItem;
	//taille du texte
	int size_1 = 70;
	int size_2 = 80;
};

