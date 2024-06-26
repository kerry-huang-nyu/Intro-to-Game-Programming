#pragma once
#include "Scene.h"

#include <random>


struct FruitStats { 
    //status about fruits but also stats on the user 
    std::vector<std::vector<float>> COLOR = { {1.0f, 0.0f, 0.0f, 1.0f},
        {1.0f, 0.5f, 0.0f, 1.0f },
        {1.0f, 1.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f, 1.0f},
        {0.29f, 0.0f, 0.51f, 1.0f} };

    std::vector<float> RADIUS = { 0.1f, 0.4f, 0.65f, 0.8f, 0.9f, 0.95f };

    int nextfruit = 0;
};

struct Goals {
    std::vector<int> have; 
    std::vector<bool> forbid; 
    int clicks;
};

class Level1 : public Scene {
public:

    std::random_device rd;
    std::mt19937 gen;

    FruitStats fruit_stats;
    Goals goals;

    // ����� CONSTRUCTOR ����� //
    ~Level1();

    // ����� METHODS ����� //
    void initialise() override;
    void update(float delta_time, std::ofstream& log) override;

    void render_next_fruit(ShaderProgram* program, ShaderProgram* text_program, std::ofstream& log);
    void render_goals(ShaderProgram* program, ShaderProgram* text_program, std::ofstream& log);

    void render(ShaderProgram* program, ShaderProgram* text_program, std::ofstream&) override;
    void spawn(float x, float y, int fruitindex) override;

    void clean_death();
    void spawn_new_fruits(std::vector<Info>& info);

    bool check_win();

    void reset() override;
};