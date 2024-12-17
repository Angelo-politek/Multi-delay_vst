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

#include "LFO.h"

LFO::LFO() : _amount(0.0f), _rate(1.0f), _shape(shape_sine), _phase(0.0f) {}                            // Costruttore dell'oggetto LFO con inizializzazione dei parametri

void LFO::set_rate(float rateHz)                                                                        // Metodo per impostare la frequenza
{
    _rate = juce::jlimit(0.1f, 5.0f, rateHz);                                                           // Limita la frequenza tra 0.1 e 5 Hz attraverso il metodo jlimit
}

void LFO::set_shape(int shape)                                                                          // Metodo per impostare la forma d'onda
{
    _shape = static_cast<Shape>(juce::jlimit(0, 2, shape));                                             // Prende un valore intero e lo casta al tipo Shape, limitando il valore tra 0 e 2
}

void LFO::set_amount(float amount)                                                                      // Metodo per impostare l'ampiezza
{
    _amount = juce::jlimit(0.0f, 1.0f, amount);                                                         // Limita l'ampiezza tra 0 e 1 attraverso il metodo jlimit
}

float LFO::getNextValue(double sampleRate)                                                              // Metodo per ottenere il valore successivo della forma d'onda
{
    float value = 0.0f;                                                                                 // Inizializza il valore a 0

    switch (_shape)                                                                                     // Cerca il caso della forma d'onda selezionata
    {
        case shape_sine:                                                                                // Caso forma d'onda sinusoidale
            value = std::sin(2.0 * juce::MathConstants<double>::pi * _phase);                           // Calcola il valore della sinusoide in funzione della fase (sin(2*pi*phase))
            break;
        case shape_saw:                                                                                 // Caso forma d'onda sawtooth con interpolazione lineare
        {
            float t = _phase;                                                                           // Inizializza la variabile t alla fase
            if (t < 0.5f)
                value = 2.0f * t;                                                                       // Se t < 0.5, il valore è 2t
            else
                value = -2.0f * (1.0f - t);                                                             // Altrimenti il valore è -2(1-t)
            break;
        }
        case shape_square:                                                                              // Caso forma d'onda square con interpolazione lineare
        {
            float t = _phase;                                                                           // Inizializza la variabile t alla fase
            const float transitionWidth = 0.1f;                                                         // Larghezza della transizione
            
            if (t < 0.5f - transitionWidth)
                value = 1.0f;                                                                           // Se t < 0.5 - transitionWidth, il valore è 1
            else if (t < 0.5f + transitionWidth)
                value = 1.0f - (t - (0.5f - transitionWidth)) * (2.0f / (2.0f * transitionWidth));      // Se t < 0.5 + transitionWidth, il valore è 1 - (t - (0.5 - transitionWidth)) * (2 / (2 * transitionWidth))
            else if (t < 1.0f - transitionWidth)
                value = -1.0f;                                                                          // Se t < 1 - transitionWidth, il valore è -1
            else
                value = -1.0f + (t - (1.0f - transitionWidth)) * (2.0f / (2.0f * transitionWidth));     // Altrimenti il valore è -1 + (t - (1 - transitionWidth)) * (2 / (2 * transitionWidth))
            break;
        }
    }

    _phase += _rate / sampleRate;                                                                       // Incrementa la fase in funzione della frequenza e del sample rate
    if (_phase >= 1.0f)
        _phase -= 1.0f;                                                                                 // Se la fase è maggiore o uguale a 1, la decrementa di 1 per rimanere nel range [0, 1]

    return value * _amount;                                                                             // Restituisce il valore della forma d'onda moltiplicato per l'ampiezza nel range [-1, 1]
}
