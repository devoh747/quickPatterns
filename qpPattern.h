#ifndef QP_PATTERN_H
#define QP_PATTERN_H

#include <FastLED.h>
#include <qpLinkedList.h>
#include <qpColor.h>


class qpColor;

class qpPattern {

  friend class qpColor;

  private:

    bool isActive = true;

    // Period counters

    int ticks = 0;
    int updates = 0;
    int cycles = 0;
    int activations = 0;

    int nextRenderTick = 0;

    // Colors

    qpLinkedList<qpColor> colors;
    qpColor *lastReferencedColor;

    // Animation

    int ticksBetweenFrames = 1;

    // Periodic activation

    unsigned int minTicksBetweenActivations = 0;
    unsigned int maxTicksBetweenActivations = 0;
    unsigned int ticksUntilActive = 0;

    int *activePeriodsCounter = NULL;
    unsigned int periodCountAtLastActivation = 0;
    unsigned int minPeriodsToStayActive = 1;
    unsigned int maxPeriodsToStayActive = 0;
    unsigned int currentPeriodsToStayActive = 0;
    byte chanceToActivatePattern = 0;

    void setActivePeriod(int minPeriods, int maxPeriods);

    void (qpPattern::*updateActiveStatus)();
    void activatePeriodically();
    void resetActivationTimer();

    void (qpPattern::*deactivateCheck)();
    void deactivateWhenActivePeriodOver();

    void doNothing() { /* empty function for pointers to pattern update steps that do nothing as per pattern config */ }


  protected:

    // Members and methods to be used in pattern animation code

    /*-->!! Note:
    * _numLeds and _targetLeds are undefined (empty pointers) when the pattern constructors are called.
    * Any pre-rendering calculations that require the number of LEDs to be known should be put in the initialize() function
    */

    CRGB *_targetLeds;
    int _numLeds = 0;

    // Color values

    CRGB _getColor(byte index = 0);

    // Animation util

    inline bool _inBounds(int pos) {
      return ((pos >= 0) && (pos < _numLeds));
    }

    inline void _countCycle() {
      this->cycles++;
    }

    inline void _clearLeds() {
      fill_solid(_targetLeds, _numLeds, CRGB::Black);
    }

  public:

    qpPattern();

    void assignTargetLeds(CRGB *leds, int numLeds); // Called when pattern is added to layer

    // These are the core animation functions to be implemented by the child classes

    virtual void draw() = 0; //called at each update interval
    virtual void initialize() { } // called once when pattern is created, after LEDs are assigned
    
    // Render hook for layer

    bool render();

    // Pattern speed

    qpPattern &drawEveryNTicks(int ticks);

    // Periodic activation

    qpPattern &activatePeriodicallyEveryNTicks(int minTicks, int maxTicks = 0);

    qpPattern &stayActiveForNTicks(int minTicks, int maxTicks = 0);
    qpPattern &stayActiveForNFrames(int minUpdates, int maxUpdates = 0);
    qpPattern &stayActiveForNCycles(int minCycles, int maxCycles = 0);

    qpPattern &withChanceOfActivation(byte percentage);

    // Colors

    qpPattern &color(byte index);
    qpColor &sameColor() { return *this->lastReferencedColor; }

    qpPattern &singleColor(CRGB color);
    qpPattern &chooseColorSequentiallyFromPalette(CRGBPalette16 colorPalette, byte stepSize = 3);
    qpPattern &chooseColorRandomlyFromPalette(CRGBPalette16 colorPalette);

    qpPattern &chooseColorSequentiallyFromSet(CRGB *colorSet, byte numColorsInSet);
    qpPattern &chooseColorRandomlyFromSet(CRGB *colorSet, byte numColorsInSet);

    qpPattern &changeColorEveryNTicks(int minTicks, int maxTicks = 0);
    qpPattern &changeColorEveryNCycles(int minCycles, int maxCycles = 0);
    qpPattern &changeColorEveryNFrames(int minFrames, int maxFrames = 0);
    qpPattern &changeColorEveryNActivations(int minActivations, int maxActivations = 0);

    qpPattern &withChanceToChangeColor(byte percentage);

    // Status control

    bool activate();
    void deactivate();

};

#endif