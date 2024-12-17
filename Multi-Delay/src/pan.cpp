// Classe Pan per la gestione del panning
// La classe prevede un oggetto Pan con i seguenti parametri:
//      - _pan: Panning (-1.0 = full left, 0.0 = center, +1.0 = full right)
// Per impostare i parametri si utilizzano i metodi:
//      - set_pan(float pan) per il panning
//      - reset() per resettare il panning al centro
// Per processare il segnale stereo si utilizza il metodo:
//      - process(juce::AudioBuffer<float>& buffer) che prende in input il buffer stereo e applica il panning
/////////////////////////////////////////////////////////////////////////////////////////////


#include "Pan.h"


Pan::Pan() : _pan{0.0f} {}                                          // Costruttore dell'oggetto Pan con inizializzazione del panning a 0

void Pan::reset()                                                   // Metodo per resettare il panning al centro
{
    _pan = 0.0f;                                                    // Reset pan to center
}

void Pan::set_pan(float pan)                                        // Metodo per impostare il panning
{
    _pan = juce::jlimit(-1.0f, 1.0f, pan);                          // Limita il panning tra -1 e 1 attraverso il metodo jlimit
}

void Pan::process(juce::AudioBuffer<float>& buffer)                 // Metodo per processare il segnale stereo
{
    auto numSamples = buffer.getNumSamples();                       // Ottiene il numero di campioni
    auto* leftChannel = buffer.getWritePointer(0);                  // Ottiene il puntatore al canale sinistro
    auto* rightChannel = buffer.getWritePointer(1);                 // Ottiene il puntatore al canale destro

                                                                    // Calcola partendo dal valore di _pan nel range [-1, 1] i guadagni dei canali sinistro e destro nel range [0, 1]
    float leftGain = (_pan <= 0.0f) ? 1.0f : (1.0f - _pan);         // Guadagno sinistro (1 se _pan è minore o uguale a 0, altrimenti 1 - _pan)
    float rightGain = (_pan >= 0.0f) ? 1.0f : (1.0f + _pan);        // Guadagno destro (1 se _pan è maggiore o uguale a 0, altrimenti 1 + _pan)

    for (int i = 0; i < numSamples; ++i)                            // Per ogni campione
    {
        leftChannel[i] *= leftGain;                                 // Moltiplica il campione sinistro per il guadagno sinistro
        rightChannel[i] *= rightGain;
    }
}