#pragma once
#include <string>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class TextureLoader
{
public:
    static sf::Texture* loadTexture(std::string filename);

    static void deallocateTextures();

private:
    static std::unordered_map<std::string, sf::Texture*> umapTexturesLoaded;
};

class SoundLoader
{
public:
    static sf::SoundBuffer* loadSound(std::string filename);

    static void deallocateSounds();

private:
    static std::unordered_map<std::string, sf::SoundBuffer*> umapSoundsLoaded;
};

class Timer
{
public:
    Timer(float setTimeSMax, float setTimeSCurrent = 0.0f);

    void countUp(float dT);

    void countDown(float dT);

    void resetToZero();
    void resetToMax();

    bool timeSIsZero();

    bool timeSIsGreaterThanOrEqualTo(float timeSCheck);
private:
    float timeSMax;
    float timeSCurrent;
};


