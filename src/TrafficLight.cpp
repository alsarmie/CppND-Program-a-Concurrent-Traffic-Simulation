#include "TrafficLight.h"
#include <iostream>
#include <random>

/* Implementation of class "MessageQueue" */

template <typename T> T MessageQueue<T>::receive() {

  //  FP.5a : The method receive should use std::unique_lock<std::mutex>
  // and _condition.wait() to wait for and receive new messages and pull them
  // from the queue using move semantics. The received object should then be
  // returned by the receive function.
  std::unique_lock<std::mutex> lock(_msg_mtx);
  _msg_cnd.wait(lock, [this]() { return !_queue.empty(); });
  T msg = std::move(_queue.back());
  _queue.pop_back();

  return msg;
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
  //  FP.4a : The method send should use the mechanisms
  // std::lock_guard<std::mutex> as well as _condition.notify_one() to add a new
  std::lock_guard<std::mutex> lock(_msg_mtx);
  _queue.emplace_back(std::move(msg));
  _msg_cnd.notify_one();
  // message to the queue and afterwards send a notification.
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::red; }

void TrafficLight::waitForGreen() {
  //  FP.5b : add the implementation of the method waitForGreen, in which
  // an infinite while-loop runs and repeatedly calls the receive function on
  // the message queue. Once it receives TrafficLightPhase::green, the method
  // returns.
  auto _phase{TrafficLightPhase::red};
  while (_phase == TrafficLightPhase::red)
    _phase = _phaseMsgQueue->receive();
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
  //  FP.2b : Finally, the private method „cycleThroughPhases“ should be
  // started in a thread when the public method „simulate“ is called. To do
  // this, use the thread queue in the base class.
  threads.emplace_back(&TrafficLight::cycleThroughPhases, this);
}

// virtual function which is executed in a thread
[[noreturn]] void TrafficLight::cycleThroughPhases() {
  //  FP.2a : Implement the function with an infinite loop that measures
  // the time between two loop cycles and toggles the current phase of the
  // traffic light between red and green and sends an update method to the
  // message queue using move semantics.
  //
  // The cycle duration should be a random
  // value between 4 and 6 seconds. Also, the while-loop should use
  // std::this_thread::sleep_for to wait 1ms between two cycles.

  while (true) {
    auto _cycle = std::chrono::seconds{_generator->cycle()};
    auto _startTime = std::chrono::system_clock::now();
    while (std::chrono::duration<double>(std::chrono::system_clock::now() -
                                         _startTime) <= _cycle)
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    {
      std::lock_guard<std::mutex> lck(_mutex);
      _currentPhase = (_currentPhase == TrafficLightPhase::red)
                          ? TrafficLightPhase::green
                          : TrafficLightPhase::red;
      _phaseMsgQueue->send(std::move(_currentPhase));
    }
  }
}