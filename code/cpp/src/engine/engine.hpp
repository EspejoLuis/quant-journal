#pragma once

class Engine {

  public:
    struct Arguments {
        virtual ~Arguments() = default;
    };
    virtual Arguments* getArguments() = 0;

    virtual void calculate() = 0;
    virtual double getResult() const = 0;

    virtual ~Engine() = default;
};
