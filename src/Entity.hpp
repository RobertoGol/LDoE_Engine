#pragma once
#include "Math.hpp"

class Entity {
public:
    Vector3 position;
    
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual ~Entity() = default;
};
