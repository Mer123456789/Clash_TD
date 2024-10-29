#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <ctime>
#include "Game.h"
#include "Menu.h"

int delta;

int main() {
	//Creation du Menu
	//taille et création de la fenetre du Menu
	int width = 960;
	int height = 720;
	RenderWindow MENU(VideoMode(width, height), "Menu", Style::Default);
	Menu textMenu(float(MENU.getSize().x), float(MENU.getSize().y));
	//charger l'icone
	auto icon = sf::Image{};
	if (!icon.loadFromFile("Data/Images/logoTD.jpeg"))
	{
		std::cerr << "Error loading Icon file!" << std::endl;
		return 1;
	}
	MENU.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	Music music;
	// Chargement de la musique
	if (!music.openFromFile("Data/Sounds/menuMusic.ogg")) {
		std::cerr << "Error loading music file!" << std::endl;
		return 1;
	}
	// Jouer la musique
	music.setLoop(true); // Créer une boucle
	music.play();

	// créer l'arrière plan du menu
	RectangleShape mbackground;
	mbackground.setSize(Vector2f(float(width), float(height)));
	Texture menu;
	menu.loadFromFile("Data/Images/menu.jpg");
	mbackground.setTexture(&menu);

	// créer l'arrière plan de la fenetre présentation
	RectangleShape pbackground;
	pbackground.setSize(Vector2f(float(width), float(height)));
	Texture present;
	present.loadFromFile("Data/Images/Présentation.png");
	pbackground.setTexture(&present);

	//Boucle du programme
	while (MENU.isOpen()) {
		Event event;
		while (MENU.pollEvent(event))
		{
			//Options des différents évenements dans le menu
			if (event.type == Event::Closed) {
				MENU.close();
			}
			if (event.type == Event::KeyReleased || event.type == Event::MouseWheelMoved)
			{
				if (event.type == sf::Event::MouseWheelMoved)
				{
					delta = event.mouseWheel.delta;
				}
				else {
					delta = 0;
				}
				if (event.key.code == Keyboard::Up || delta > 0)
				{
					textMenu.MoveUp();
					break;
				}
				if (event.key.code == Keyboard::Down || delta < 0)
				{
					textMenu.MoveDown();
					break;
				}
				if (event.key.code == Keyboard::Enter)
				{
					//Création des fenêtres
					RenderWindow Present(VideoMode(width, height), "Mode d'emploi");
					Present.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

					RenderWindow window(VideoMode::getDesktopMode(), "Clash TD", Style::Fullscreen);
					window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

					int x = textMenu.MenuPressed();
					//Gére les options choisis
					if (x == 0) {
						music.stop();//arreter la music du menu
						//fermeture des deux autres fenetre
						Present.close();

						//creation du jeu
						// 
						// Générateur de nb aléatoire se basant sur le temps pour etre vraiment aléatoire
						srand(static_cast<unsigned int>(time(nullptr)));
						//Information sur la fenetre
						window.setVerticalSyncEnabled(true);
						int window_width = window.getSize().x;
						int window_height = window.getSize().y;

						//Chargement et lancement de la musique du jeu
						Music Gamemusic;
						if (!Gamemusic.openFromFile("Data/Sounds/gameMusic.mp3")) {
							std::cerr << "Error loading gameMusic file!" << std::endl;
							return 1;
						}
						Gamemusic.setLoop(true);
						Gamemusic.play();

						//fonction du jeu
						Game game(&window, window_width, window_height);

						window.close();//ferme la fenetre

						//création de la fenetre Game Over
						RenderWindow GameOver(VideoMode(width, height), "Game Over");
						GameOver.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

						RectangleShape gameover;
						gameover.setSize(Vector2f(float(width), float(height)));
						Texture over;
						over.loadFromFile("Data/Images/game over.jpg");
						gameover.setTexture(&over);

						while (GameOver.isOpen()) {
							sf::Event eventa;
							while (GameOver.pollEvent(eventa)) {
								if (eventa.type == Event::Closed)
								{
									GameOver.close();
								}
								if (eventa.type == Event::KeyPressed)
								{
									if (eventa.key.code == sf::Keyboard::Escape)
									{
										GameOver.close();
									}
								}
							}
							GameOver.clear();
							GameOver.draw(gameover);
							GameOver.display();
						}

						Gamemusic.stop();//arret de la music du jeu
						music.play();//rejoue la music du menu
					}
					//Gestion de la fenetre de présentation du jeu
					if (x == 1) {
						while (Present.isOpen()) {
							Event eventa;
							while (Present.pollEvent(eventa)) {
								if (eventa.type == Event::Closed)
								{
									Present.close();
								}
								if (eventa.type == Event::KeyPressed)
								{
									if (eventa.key.code == Keyboard::Escape)
									{
										Present.close();
									}
								}
							}
							window.close();
							Present.clear();
							Present.draw(pbackground);
							Present.display();
						}
					}
					//Fermeture du code
					if (x == 2) {
						return 0;
					}
				}
			}
		}
		//Dessiner le menu
		MENU.clear();
		MENU.draw(mbackground);
		textMenu.draw(MENU);
		MENU.display();
	}
	return 0;
}