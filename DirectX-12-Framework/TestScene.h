#pragma once
#include "Engine.h"
class TestScene :
    public Engine
{
public:
    TestScene(std::shared_ptr<Renderer> renderer, std::shared_ptr<Window> window)
        : Engine(renderer, window)
    {}
    virtual void Initialize() override;
};

