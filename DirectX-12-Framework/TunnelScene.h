#pragma once
#include "Engine.h"


class TunnelScene : public Engine
{
public:
    TunnelScene(std::shared_ptr<Renderer> renderer, std::shared_ptr<Window> window)
        : Engine(renderer, window)
    {}
    virtual void Initialize() override;
};

