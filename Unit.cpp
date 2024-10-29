#include "Unit.h"
#include "Game.h"

float Unit::speed = 1.5f;//tile par secondes
const float Unit::size = 0.48f;//taille unit�

Unit::Unit(sf::RenderWindow* window, Vector2D setPos, int waves) :
	pos(setPos), timerJustHurt(0.25f) {
	sf::Texture* texture = TextureLoader::loadTexture("barbarian.bmp");//charge apparence
	sprite.setTexture(*texture);
	/*
	texture_top = TextureLoader::loadTexture("Archer_Sprites_top.bmp");//charge apparence
	texture_topRight = TextureLoader::loadTexture("Archer_Sprites_topRight.bmp");//charge apparence
	texture_topLeft = TextureLoader::loadTexture("Archer_Sprites_topLeft.bmp");//charge apparence
	texture_Left = TextureLoader::loadTexture("Archer_Sprites_Left.bmp");//charge apparence
	texture_Right = TextureLoader::loadTexture("Archer_Sprites_Right.bmp");//charge apparence
	texture_bottomRight = TextureLoader::loadTexture("Archer_Sprites_bottomRight.bmp");//charge apparence
	texture_bottomLeft = TextureLoader::loadTexture("Archer_Sprites_bottomLeft.bmp");//charge apparence
	texture_bottom = TextureLoader::loadTexture("Archer_Sprites_bottom.bmp");//charge apparence
	*/

	//change les statistiques en fonctions du nombre de vague pass�
	if (waves % WavesChange == 0) {
		int nbAl�a = rand() % 3;
		//augmente vie
		if (nbAl�a == 0) { healthMax = int(waves / WavesChange) * 2 + healthMax; healthCurrent = healthMax; }
		//augmente d�gats
		if (nbAl�a == 1) { damage = int(waves / WavesChange) * 2 + damage; }
		//augmente vitesse
		if (speed < 5.0f && nbAl�a == 2) { speed = 1.5f + float(waves / WavesChange) * 0.5f; }
	}

}

void Unit::update(float dT, Level& level, std::vector<std::shared_ptr<Unit>>& listUnits) {
	timerJustHurt.countDown(dT);

	//Determine la distance de l'unit� au point cible
	float distanceToTarget = (level.getCiblePos() - pos).magnitude();

	//si l'unit� est assez proche du point cible, on l'a supprime
	if (distanceToTarget < 0.5f) {
		healthCurrent = 0;
		hitTarget = true;
	}
	else {
		//Determiner la distance � parcourir chaque frame
		float distanceMove = speed * dT;
		if (distanceMove > distanceToTarget)
			distanceMove = distanceToTarget;//g�re si erreur pour pas planter le programme

		//prendre la direction dans laquelle l'unit� dois aller.
		Vector2D directionNormal(level.getNormalPath((int)pos.x, (int)pos.y));
		/*sprite_nb = int(directionNormal.angle() / 45);*/
		//si l'unit� a atteint le point cible, modifier le vecteur de direction pour qu'il reste sur la tile cible.
		if ((int)pos.x == (int)level.getCiblePos().x && (int)pos.y == (int)level.getCiblePos().y)
			directionNormal = (level.getCiblePos() - pos).normalize();

		Vector2D posAdd = directionNormal * distanceMove;//vecteur de changement de position

		//verifier si la prochaine position ne se confondera pas avec une autre unit�
		bool moveOk = true;//si c'est ok de bouger

		//regarder chaque unit�
		for (int count = 0; count < listUnits.size() && moveOk; count++) {
			auto& unitSelected = listUnits[count];
			//v�rifie si l'unit� existe, n'est pas celle pour qui on v�rifie et se confond bien avec cette derni�re
			if (unitSelected != nullptr && unitSelected.get() != this &&
				unitSelected->checkCollisionOver(pos, size)) {
				//cr�er la direction qu'ils on a parcourir entre les deux
				Vector2D directionToOther = (unitSelected->pos - pos);

				//v�rifier qu'ils ne sont pas directement l'un par dessus l'autre
				if (directionToOther.magnitude() > 0.01f) {
					//S'assurer que les deux unit� ne se fonce pas dedans en regardant l'angle entre les deux(max pi sur 4)
					Vector2D normalToOther(directionToOther.normalize());
					float angleBtw = abs(normalToOther.angleBetween(directionNormal));
					if (angleBtw < 3.14159265359f / 4.0f)
						//ne pas permettre � l'unit� de bouger
						moveOk = false;
				}
			}
		}

		if (moveOk) {

			const float spacing = 0.35f;//espace pour les garder �loign� des murs
			//v�rifie si il peut bouger dans la direction x ou si il y a un mur
			int x = (int)(pos.x + posAdd.x + copysign(spacing, posAdd.x));
			int y = (int)(pos.y);
			if (posAdd.x != 0.0f && level.isTileWall(x, y) == false)
				pos.x += posAdd.x;

			//m�me op�ration avec la direction y
			x = (int)(pos.x);
			y = (int)(pos.y + posAdd.y + copysign(spacing, posAdd.y));
			if (posAdd.y != 0.0f && level.isTileWall(x, y) == false)
				pos.y += posAdd.y;
		}
	}
}

void Unit::draw(sf::RenderWindow* window, int tileSize) {
	/*
	if (sprite_nb == 1) { sprite.setTexture(*texture_topRight); };
	if (sprite_nb == 2) { sprite.setTexture(*texture_top); };
	if (sprite_nb == 3) { sprite.setTexture(*texture_topLeft); };
	if (sprite_nb == 4) { sprite.setTexture(*texture_Left); };
	if (sprite_nb == 5) { sprite.setTexture(*texture_bottomLeft); };
	if (sprite_nb == 6) { sprite.setTexture(*texture_bottom); };
	if (sprite_nb == 7) { sprite.setTexture(*texture_bottomRight); };
	if (sprite_nb == 8 || sprite_nb==0) { sprite.setTexture(*texture_Right); };
	*/
	if (window != nullptr) {
		//Changer la couleur de la texture en rouge si l'unit� a �t� touch� recement.
		if (!timerJustHurt.timeSIsZero()) {
			sprite.setColor(sf::Color::Red);
		}
		else {
			sprite.setColor(sf::Color::White);
		}

		//Dessine l'image � la position de l'unit�.
		sprite.setPosition(pos.x * tileSize - (sprite.getLocalBounds().width / 2),
			pos.y * tileSize - (sprite.getLocalBounds().height / 2));
		window->draw(sprite);
	}
}

//cette fonction retourne oui si deux �l�ment sont � la m�me position
bool Unit::checkCollisionOver(Vector2D posOther, float sizeOther) {
	return (posOther - pos).magnitude() <= (sizeOther + size) / 2.0f;
}


//retourne oui si la vie de l'unit� est en dessus de z�ro
bool Unit::isAlive() {
	return (healthCurrent > 0);
}
//retourne oui si l'unit� � touch� le point cible
bool Unit::getCible() {
	return (hitTarget);
}
//return les d�gats des unit�
int Unit::getDammage(int setDammageBuff) {
	damage += setDammageBuff;//ajouter le buff de damage si il y a
	return damage;
}
//retourne la position des unit�s
Vector2D Unit::getPos() {
	return pos;
}

//enl�ve de la vie � l'unit�
void Unit::removeHealth(int damageUnit) {
	if (damageUnit > 0) {
		healthCurrent -= damageUnit;
		if (healthCurrent < 0)
			healthCurrent = 0;

		timerJustHurt.resetToMax();
	}
}
