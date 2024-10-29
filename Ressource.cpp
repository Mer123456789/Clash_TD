#include "Ressource.h"
#include<iostream>

// Gestion des textures
std::unordered_map<std::string, sf::Texture*> TextureLoader::umapTexturesLoaded;

sf::Texture* TextureLoader::loadTexture(std::string filename) {
    if (!filename.empty()) {
        auto found = umapTexturesLoaded.find(filename);

        if (found != umapTexturesLoaded.end()) {
            // La texture a d�j� �t� charg�e, on la retourne.
            return found->second;
        }
        else {
            // Cr�er le chemin d'acc�s relatif au dossier d'images avec le nom du fichier.
            std::string filepath = "Data/Images/" + filename;

            // Tente de charger la texture � partir du fichier.
            try {
                sf::Texture* textureOutput = new sf::Texture();
                if (textureOutput->loadFromFile(filepath)) {
                    // Ajoute la texture � la map des textures charg�es pour la gestion de la m�moire.
                    umapTexturesLoaded[filename] = textureOutput;

                    return textureOutput;
                }
                else {
                    delete textureOutput;
                }
            }
            catch (const std::exception& e) {
                // G�rer l'erreur de m�moire
                std::cerr << "Erreur de m�moire : " << e.what() << std::endl;
            }
        }
    }

    return nullptr;
}

void TextureLoader::deallocateTextures() {
    // D�truit toutes les textures.
    while (!umapTexturesLoaded.empty()) {
        auto it = umapTexturesLoaded.begin();
        if (it->second != nullptr) {
            delete it->second;
        }
        umapTexturesLoaded.erase(it);
    }
}

// Gestion des sons
std::unordered_map<std::string, sf::SoundBuffer*> SoundLoader::umapSoundsLoaded;

sf::SoundBuffer* SoundLoader::loadSound(std::string filename) {
    if (!filename.empty()) {
        auto found = umapSoundsLoaded.find(filename);

        if (found != umapSoundsLoaded.end()) {
            // Le SoundBuffer a d�j� �t� charg�, on le retourne.
            return found->second;
        }
        else {
            // Cr�er le chemin d'acc�s relatif au dossier de sons avec le nom du fichier.
            std::string filepath = "Data/Sounds/" + filename;

            // Tente de charger le SoundBuffer � partir du fichier.
            sf::SoundBuffer* soundBuffer = new sf::SoundBuffer();
            if (!soundBuffer->loadFromFile(filepath)) {
                delete soundBuffer;
            }
            else {
                // Ajoute le SoundBuffer � la map des SoundBuffers charg�s pour la gestion de la m�moire.
                umapSoundsLoaded[filename] = soundBuffer;

                return soundBuffer;
            }
        }
    }

    return nullptr;
}

void SoundLoader::deallocateSounds() {
    // D�truit tous les SoundBuffers.
    while (!umapSoundsLoaded.empty()) {
        auto it = umapSoundsLoaded.begin();
        if (it->second != nullptr) {
            delete it->second;
        }
        umapSoundsLoaded.erase(it);
    }
}

// Timer
Timer::Timer(float setTimeSMax, float setTimeSCurrent) :
    timeSMax(setTimeSMax), timeSCurrent(setTimeSCurrent) {

}

void Timer::countUp(float dT) {
    if (timeSCurrent < timeSMax) {
        timeSCurrent += dT;
        if (timeSCurrent > timeSMax)
            timeSCurrent = timeSMax;
    }
}

void Timer::countDown(float dT) {
    if (timeSCurrent > 0.0f) {
        timeSCurrent -= dT;
        if (timeSCurrent < 0.0f)
            timeSCurrent = 0.0f;
    }
}

void Timer::resetToZero() {
    timeSCurrent = 0.0f;
}

void Timer::resetToMax() {
    timeSCurrent = timeSMax;
}

bool Timer::timeSIsZero() {
    return (timeSCurrent <= 0.0f);
}

bool Timer::timeSIsGreaterThanOrEqualTo(float timeSCheck) {
    return (timeSCurrent >= timeSCheck);
}
