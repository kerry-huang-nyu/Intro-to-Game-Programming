#pragma once
#pragma once
#include "Scene.h"

class Level2 : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //


    // ————— CONSTRUCTOR ————— //
    ~Level2();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

    void reset() override;
};