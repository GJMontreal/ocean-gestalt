#include "ADSREnvelope.hpp"

void ADSREnvelope::trigger(float a, float d, float s, float r) {
  attack = a;
  decay = d;
  sustain = s;
  release = r;
  time = 0.0f;
  value = 0.0f;
  state = State::Attack;
  active = true;
}

void ADSREnvelope::update(float dt) {
  if (!active)
    return;
  time += dt;

  switch (state) {
    case State::Attack:
      value = time / attack;
      if (time >= attack) {
        time -= attack;
        state = State::Decay;
      }
      break;
    case State::Decay:
      value = 1.0f - (1.0f - sustain) * (time / decay);
      if (time >= decay) {
        time -= decay;
        state = State::Sustain;
      }
      break;
    case State::Sustain:
      value = sustain;
      break;
    case State::Release:
      value *= 1.0f - (dt / release);
      if (value <= 0.0001f) {
        value = 0.0f;
        state = State::Idle;
        active = false;
      }
      break;
    case State::Idle:
      value = 0.0f;
      active = false;
      break;
  }
}

void ADSREnvelope::releaseEnv() {
  if (state != State::Idle && state != State::Release) {
    state = State::Release;
    time = 0.0f;
  }
}