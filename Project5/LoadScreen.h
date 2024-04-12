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
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

    void reset() override;
};