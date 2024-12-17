// Classe Pan per la gestione del panning
// La classe prevede un oggetto Pan con i seguenti parametri:
//      - _pan: Panning (-1.0 = full left, 0.0 = center, +1.0 = full right)
// Per impostare i parametri si utilizzano i metodi:
//      - set_pan(float pan) per il panning
//      - reset() per resettare il panning al centro
// Per processare il segnale stereo si utilizza il metodo:
//      - process(juce::AudioBuffer<float>& buffer) che prende in input il buffer stereo e applica il panning
/////////////////////////////////////////////////////////////////////////////////////////////



#ifndef __PAN_HPP__
#define __PAN_HPP__

#include <juce_audio_basics/juce_audio_basics.h>                // Libreria JUCE

class Pan
{
private:
    float _pan;                                                  // -1.0 = full left, 0.0 = center, +1.0 = full right

public:
    Pan();                                                       // Costruttore dell'oggetto Pan

    void set_pan(float pan);                                     // Metodo per impostare il panning
    void reset();                                                // Metodo per resettare il panning al centro
    void process(juce::AudioBuffer<float>& buffer);              // Metodo per processare il segnale stereo
};

#endif // __PAN_HPP__