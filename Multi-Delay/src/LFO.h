// Classe LFO per la generazione di forme d'onda
// La classe prevede un oggetto LFO con i seguenti parametri:
//      - _rate: Frequenza in Hz
//      - _amount: Ampiezza della modulazione
//      - _shape: Forma d'onda (sine, saw, square)
//      - _phase: Fase (per determinare la posizione della forma d'onda)
// Per impostare i parametri si utilizzano i metodi:
//      - set_rate(float rateHz) per la frequenza
//      - set_shape(int shape) per la forma d'onda
//      - set_amount(float amount) per l'ampiezza
// Per ottenere il valore successivo della forma d'onda si utilizza il metodo:
//      - getNextValue(double sampleRate) che prende in input il sample rate del progetto e restituisce il valore successivo della forma d'onda
/////////////////////////////////////////////////////////////////////////////////////////////


#ifndef TEMPLATE_LFO_H
#define TEMPLATE_LFO_H

#include <juce_audio_basics/juce_audio_basics.h>    // Libreria JUCE

class LFO
{
public:
    enum Shape                                      // Enumerazione per le forma d'onda
    {
        shape_sine = 0,
        shape_saw = 1,
        shape_square = 2,
    };

private:
    float _rate;                                    // Frequenza in Hz
    float _amount;                                  // Ampiezza della modulazione
    double _phase;                                  // Fase
    Shape _shape;                                   // Forma d'onda


public:
    LFO();                                          // Costruttore dell'oggetto LFO

    void set_rate(float rateHz);                    // Metodo per impostare la frequenza
    void set_shape(int shape);                      // Metodo per impostare la forma d'onda
    void set_amount(float amount);                  // Metodo per impostare l'ampiezza
    float getNextValue(double sampleRate);          // Metodo per ottenere il valore successivo della forma d'onda
};

#endif
