#pragma once
#include "Engine.h"
class DisconnectedScene :
    public Engine
{
public:
    DisconnectedScene(std::shared_ptr<Renderer> renderer, std::shared_ptr<Window> window)
        : Engine(renderer, window)
    {}
    virtual void Initialize() override;
};

