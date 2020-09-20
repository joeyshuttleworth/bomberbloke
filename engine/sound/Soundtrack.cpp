#include "Soundtrack.hpp"

#include <functional>
#include <random>
#include <string>

#include "Sound.hpp"
#include "engine.hpp"

const int FADE_OUT_MS = 1000;

const std::list<std::string> OST1_CLIP_NAMES = {"ost1_idle", "ost1_a_first", "ost1_b_first_neptune",
  "ost1_a_main_neptune", "ost1_b_main_neptune", "ost1_final_neptune"};
const std::list<std::list<double>> OST1_TRANSITIONS = {
  { 0,    1,    0,    0,    0,    0     },
  { 0,    0,    1,    0,    0,    0     },
  { 0,    0.2,  0,    0.8,  0,    0     },
  { 0,    0,    0.3,  0,    0.7,  0     },
  { 0,    0.2,  0,    0.8,  0,    0     },
  { 0,    0,    0,    1,    0,    0     }
};
const std::list<std::list<double>> OST1_INTENSE_TRANSITIONS = {
  { 0,    0,    0,    1,    0,    0     },
  { 0,    0,    0,    0,    1,    0     },
  { 0,    0,    0,    0.5,  0,    0.5   },
  { 0,    0,    0,    0,    1,    0     },
  { 0,    0,    0,    0.5,  0,    0.5   },
  { 0,    0,    0,    1,    0,    0     }
};

const std::list<std::string> OST2_CLIP_NAMES = {"ost2_idle", "ost2_intro", "ost2_first",
  "ost2_main1", "ost2_main2", "ost2_solo", "ost2_pads"};
const std::list<std::list<double>> OST2_TRANSITIONS = {
  { 0,    0.7,  0.3,  0,    0,    0,    0   },
  { 0,    0,    0.9,  0,    0,    0,    0.1 },
  { 0,    0,    0,    0.5,  0.5,  0,    0   },
  { 0,    0,    0,    0.1,  0.7,  0.15, 0.05},
  { 0,    0,    0,    0.7,  0.1,  0.15, 0.05},
  { 0,    0,    0,    0.4,  0.4,  0,    0.2 },
  { 0,    0,    0,    0.45, 0.45, 0,    0.1 }
};
const std::list<std::list<double>> OST2_INTENSE_TRANSITIONS = {
  { 0,    0.7,  0.3,  0,    0,    0,    0   },
  { 0,    0,    0.9,  0,    0,    0,    0.1 },
  { 0,    0,    0,    0.5,  0.5,  0,    0   },
  { 0,    0,    0,    0.05, 0.4,  0.5,  0.05},
  { 0,    0,    0,    0.4,  0.05, 0.5,  0.05},
  { 0,    0,    0,    0.4,  0.4,  0,    0.2 },
  { 0,    0,    0,    0.45, 0.45, 0,    0.1 }
};

Soundtrack::Soundtrack(std::list<std::string> clipNames, std::list<std::list<double>> transitions, std::list<std::list<double>> intenseTransitions) {
  mTransitions = transitions;
  mIntenseTransitions = intenseTransitions;

  auto callback = std::bind(&Soundtrack::onClipFinished, this);
  for (auto i = clipNames.begin(); i != clipNames.end(); i++) {
    std::shared_ptr<Sound> clipSound = soundManager.createSound(*i);
    clipSound->mGroup = SOUND_MUSIC;
    clipSound->onFinishedPlaying = callback;
    mClipSounds.push_back(clipSound);
  }
}

void Soundtrack::playIdle() {
  if (!mIsIdle && mIsPlaying) {
    auto iter = mClipSounds.begin();
    std::advance(iter, mCurrentIndex);
    mIsIdle = true;
    mIsPlaying = true;
    (*iter)->stop();
  } else if (!mIsPlaying) {
    mIsIdle = true;
    mIsPlaying = true;
    soundManager.playSound(mClipSounds.front());
  }
}

void Soundtrack::play() {
  if (mIsIdle && mIsPlaying) {
    mIsIdle = false;
    mClipSounds.front()->stop();
  } else if (mIsPlaying) {
    auto iter = mClipSounds.begin();
    std::advance(iter, mCurrentIndex);
    (*iter)->stop();
  } else {
    mIsIdle = false;
    mIsPlaying = true;
    mCurrentIndex = 0;
    onClipFinished();
  }
}

void Soundtrack::stop() {
  mIsPlaying = false;
  for (auto iter = mClipSounds.begin(); iter != mClipSounds.end(); iter++) {
    (*iter)->stop();
  }
}

void Soundtrack::onClipFinished() {
  if (mIsIdle && mIsPlaying) {
    // If idle, continue playing idle clip
    mIsPlaying = true;
    soundManager.playSound(mClipSounds.front());
  } else if (mIsPlaying) {
    // Generate uniform random sample in range [0, 1]
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distribution(0, 1);
    double probValue = distribution(gen);

    // Get transition probabilities
    auto currentProbs = mTransitions.begin();
    auto currentIntenseProbs = mTransitions.begin();
    std::advance(currentProbs, mCurrentIndex);
    std::advance(currentIntenseProbs, mCurrentIndex);

    // Use probValue to generate a random next clip
    double cumProb = 0;
    auto transitionProbs = currentProbs->begin();
    auto intenseTransitionProbs = currentProbs->begin();
    for (int i = 0; i < currentProbs->size(); i++) {
      cumProb += (*transitionProbs) * (1 - mIntensity) + (*intenseTransitionProbs) * mIntensity;
      if (cumProb >= probValue) {
        mCurrentIndex = i;
        break;
      }
      transitionProbs++;
      intenseTransitionProbs++;
    }

    // Play new clip
    auto iter = mClipSounds.begin();
    std::advance(iter, mCurrentIndex);
    mIsPlaying = true;
    soundManager.playSound(*iter);
  }
}

Soundtrack1::Soundtrack1()
  : Soundtrack(OST1_CLIP_NAMES, OST1_TRANSITIONS, OST1_INTENSE_TRANSITIONS) {};

Soundtrack2::Soundtrack2()
  : Soundtrack(OST2_CLIP_NAMES, OST2_TRANSITIONS, OST2_INTENSE_TRANSITIONS) {};
