#pragma once
#pragma once
#include "Scene.h"

class LoadScreen : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //


    // ————— CONSTRUCTOR ————— //
    ~LoadScreen();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time, std::ofstream&) override;
    void render(ShaderProgram* program, ShaderProgram* text_program,std::ofstream&) override;

    void reset() override;
};