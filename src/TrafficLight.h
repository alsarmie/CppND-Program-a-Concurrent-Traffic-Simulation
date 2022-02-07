#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <random>

// forward declarations to avoid include cycle
class Vehicle;
enum class TrafficLightPhase { red, green };

//  FP.3 Define a class „MessageQueue“ which has the public methods send
// and receive. Send should take an rvalue reference of type TrafficLightPhase
// whereas receive should return this type. Also, the class should define an
// std::dequeue called _queue, which stores objects of type TrafficLightPhase.
// Also, there should be an std::condition_variable as well as an std::mutex as
// private members.

template <class T> class MessageQueue {
public:
  MessageQueue() = default;
  ~MessageQueue() = default;
  void send(T &&_msg);
  T receive();

private:
  std::deque<T> _queue;
  std::condition_variable _msg_cnd;
  std::mutex _msg_mtx;
};
class RNGCycleGenerator {
public:
  // Constructors & Destructor
  RNGCycleGenerator() : _lbound(4), _ubound(6), _rng(rd()) {
    _un_dist = std::uniform_int_distribution<>(_lbound, _ubound);
  };
  RNGCycleGenerator(int lbound, int ubound)
      : _lbound(lbound), _ubound(ubound), _rng(rd()) {
    _un_dist = std::uniform_int_distribution<>(_lbound, _ubound);
  };
  ~RNGCycleGenerator() = default;
  // General Methods
  unsigned long cycle() { return _un_dist(_rng); }
  // Getters & Setters
  void setLbound(int &value) { _lbound = value; }
  void setUbound(int &value) { _ubound = value; }
  [[nodiscard]] int getUbound() const { return _ubound; }
  [[nodiscard]] int getLbound() const { return _lbound; }

private:
  std::uniform_int_distribution<> _un_dist;
  std::random_device rd;
  std::mt19937_64 _rng;
  int _lbound;
  int _ubound;
};
//  FP.1 : Define a class „TrafficLight“ which is a child class of
// TrafficObject. The class shall have the public methods „void waitForGreen()“
// and „void simulate()“ as well as „TrafficLightPhase getCurrentPhase()“, where
// TrafficLightPhase is an enum that can be either „red“ or „green“. Also, add
// the private method „void cycleThroughPhases()“. Furthermore, there shall be
// the private member _currentPhase which can take „red“ or „green“ as its
// value.

class TrafficLight : public TrafficObject {
public:
  // constructor / destructor
  TrafficLight();
  ~TrafficLight() = default;
  TrafficLight(TrafficLight &T) = delete;
  TrafficLight operator=(TrafficLight &T) = delete;
  TrafficLight(TrafficLight &&T) = delete;
  TrafficLight operator=(TrafficLight &&T) = delete;
  // getters / setters
  TrafficLightPhase getCurrentPhase();
  // typical behaviour methods
  void waitForGreen();
  void simulate() override;

private:
  // typical behaviour methods
  [[noreturn]] void cycleThroughPhases();
  //   FP.4b : create a private member of type MessageQueue for messages of
  // type TrafficLightPhase and use it within the infinite loop to push each new
  // TrafficLightPhase into it by calling send in conjunction with move
  // semantics.
  std::unique_ptr<RNGCycleGenerator> _generator =
      std::make_unique<RNGCycleGenerator>();
  std::unique_ptr<MessageQueue<TrafficLightPhase>> _phaseMsgQueue =
      std::make_unique<MessageQueue<TrafficLightPhase>>();
  // MessageQueue<TrafficLightPhase> _phaseMsgQueue;
  TrafficLightPhase _currentPhase;
  // RNGCycleGenerator _generator;
  std::condition_variable _condition;
  std::mutex _mutex;
};

#endif