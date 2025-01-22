//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <memory>
#include <typeindex>

namespace Graphics::Shape
{
    class GameObject;
}

namespace Graphics::Components
{
    using std::shared_ptr;
    using std::weak_ptr;
    using std::type_index;

    using Graphics::Shape::GameObject;

    class Component
    {
    public:
        Component() = default;
        virtual ~Component() = default;

        void SetOwner(shared_ptr<GameObject> owner)
        {
            this->owner = owner;
        }

        std::shared_ptr<GameObject> GetOwner() const
        {
            return owner.lock();
        }

        virtual type_index GetType() const = 0;

        virtual void Update(float deltaTime) {}

    private:
        weak_ptr<GameObject> owner;
    };
}