#pragma once

#include "pch.h"
#include "Observer.h"

class InputEvent;
class Subject {
 protected:
  std::unordered_set<Observer*> m_observers;

  void notify(InputEvent *event) {
    for (const auto &observer : m_observers) {
      observer->on_notify(event);
    }
  }
 public:
  void add(Observer* observer) { m_observers.insert(observer);};
  void remove(Observer* observer) { m_observers.erase(observer);};
};


