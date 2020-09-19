#ifndef SOUNDTRACK_HPP
#define SOUNDTRACK_HPP

#include <string>
#include <list>
#include <memory>

class Sound;

class Soundtrack {
public:
  /**
   * Constructor for soundtrack.
   *
   * @param clipNames           ID's for the sound files for each clip.
   * @param transitions         Transition matrix for clips (square matrix).
   * @param intenseTransitions  Intense transition matrix for clips (square matrix).
   */
  Soundtrack(std::list<std::string> clipNames, std::list<std::list<double>> transitions, std::list<std::list<double>> intenseTransitions);

  /**
   * Plays idle music on loop.
   */
  void playIdle();

  /**
   * Plays music.
   */
  void play();

  /**
   * Stops music.
   */
  void stop();

  /**
   * Set intensity level (influences choice of sound clip).
   */
  void setIntensity(double intensity) {
    mIntensity = intensity;
  }

  /**
   * Callback function for when clip is finished.
   */
  void onClipFinished();

protected:
  // List of clip sounds.
  std::list<std::shared_ptr<Sound>> mClipSounds;
  // Transition matrix.
  std::list<std::list<double>> mTransitions = {{}};
  // Transition matrix for intense mode.
  std::list<std::list<double>> mIntenseTransitions = {{}};

  // Current index of mClipSounds playing
  int mCurrentIndex = 0;
  // Intensity value, between 0-1 where 1 signals the music should be more intense.
  bool mIntensity = 0;
  // True if music is playing.
  bool mIsPlaying = 0;
  // True if idle music is playing.
  bool mIsIdle = 0;
};

class Soundtrack1: public Soundtrack {
public:
  Soundtrack1();
};

class Soundtrack2: public Soundtrack {
public:
  Soundtrack2();
};

#endif
