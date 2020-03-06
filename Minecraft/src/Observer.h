#pragma once

class InputEvent;
class Observer {
 public:
    virtual void on_notify(const InputEvent *event) = 0;
    virtual ~Observer() = default;
};