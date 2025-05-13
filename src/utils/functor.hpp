#pragma once

#include <functional>

class Functor{
    public:
        virtual void exec() = 0;
        virtual ~Functor() = default;
};

class Lambda : public Functor{
    std::function<void()> fun;
    
    public:
        Lambda(std::function<void()> f) : fun(f){};
        void exec() override { fun(); }
};