#include "Ressource.h"
#include<iostream>

// Gestion des textures
std::unordered_map<std::string, sf::Texture*> TextureLoader::umapTexturesLoaded;

sf::Texture* TextureLoader::loadTexture(std::string filename) {
    if (!filename.empty()) {
        auto found = umapTexturesLoaded.find(filename);

        if (found != umapTexturesLoaded.end()) {
            // La texture a déjà été chargée, on la retourne.
            return found->second;
        }
        else {
            // Créer le chemin d'accès relatif au dossier d'images avec le nom du fichier.
            std::string filepath = "Data/Images/" + filename;

            // Tente de charger la texture à partir du fichier.
            try {
                sf::Texture* textureOutput = new sf::Texture();
                if (textureOutput->loadFromFile(filepath)) {
                    // Ajoute la texture à la map des textures chargées pour la gestion de la mémoire.
                    umapTexturesLoaded[filename] = textureOutput;

                    return textureOutput;
                }
                else {
                    delete textureOutput;
                }
            }
            catch (const std::exception& e) {
                // Gérer l'erreur de mémoire
                std::cerr << "Erreur de mémoire : " << e.what() << std::endl;
            }
        }
    }

    return nullptr;
}

void TextureLoader::deallocateTextures() {
    // Détruit toutes les textures.
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
            // Le SoundBuffer a déjà été chargé, on le retourne.
            return found->second;
        }
        else {
            // Créer le chemin d'accès relatif au dossier de sons avec le nom du fichier.
            std::string filepath = "Data/Sounds/" + filename;

            // Tente de charger le SoundBuffer à partir du fichier.
            sf::SoundBuffer* soundBuffer = new sf::SoundBuffer();
            if (!soundBuffer->loadFromFile(filepath)) {
                delete soundBuffer;
            }
            else {
                // Ajoute le SoundBuffer à la map des SoundBuffers chargés pour la gestion de la mémoire.
                umapSoundsLoaded[filename] = soundBuffer;

                return soundBuffer;
            }
        }
    }

    return nullptr;
}

void SoundLoader::deallocateSounds() {
    // Détruit tous les SoundBuffers.
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
