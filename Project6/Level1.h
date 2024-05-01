#pragma once
#include "Scene.h"

#include <random>


struct FruitStats {
    std::vector<std::vector<float>> color = { {1.0f, 0.0f, 0.0f, 1.0f},
        {1.0f, 0.5f, 0.0f, 1.0f },
        {1.0f, 1.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f, 1.0f},
        {0.29f, 0.0f, 0.51f, 1.0f} };

    std::vector<float> radius = { 1, 1.4, 1.65, 1.8, 1.9, 2 };

    std::vector<EntityType> type = { F1, F2, F3, F4, F5, F6 };

};

class Level1 : public Scene {
public:

    std::random_device rd;
    std::mt19937 gen;

    FruitStats fruit_stats;
    

    // ————— CONSTRUCTOR ————— //
    ~Level1();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time, std::ofstream& log) override;

    void render(ShaderProgram* program, std::ofstream&) override;
    void spawn(int x, int y);

    void reset() override;
};