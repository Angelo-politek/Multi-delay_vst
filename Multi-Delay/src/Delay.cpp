// Classe Delay per l'effetto delay
// La classe prevede un oggetto Delay con i seguenti parametri:
//      - _delay: Buffer circolare per il delay
//      - _read_i: Indice di lettura
//      - _write_i_sx, _write_i_dx: Indici di scrittura per i canali sinistro e destro
//      - _sample_rate: Sample rate del progetto
//      - _max_delay: Massimo ritardo in campioni
//      - _dry_wet: Rapporto tra segnale diretto e segnale ritardato
//      - _feedback: Feedback del segnale ritardato
//      - _sync_enable: Abilita il delay sincronizzato tra i canali sinistro e destro
//      - _mode_delay: Modalità del delay (feedback, pingpong)
//      - _mode_pingpong: Modalità del delay pingpong (center, left, right)
// Per impostare i parametri si utilizzano i metodi:
//      - set_delay_sx_in_ms(float delay_in_ms) per il ritardo del canale sinistro
//      - set_delay_dx_in_ms(float delay_in_ms) per il ritardo del canale destro
//      - set_dry_wet(float value) per il rapporto tra segnale diretto e segnale ritardato
//      - set_feedback(float feedback) per il feedback del segnale ritardato
//      - enable_sync(bool enable) per abilitare il delay sincronizzato
//      - set_delay_mode(int mode) per impostare la modalità del delay
//      - set_pingpong_mode(int mode) per impostare la modalità del delay pingpong
// Per processare il segnale si utilizza il metodo:
//      - prepare(double sample_rate, int max_num_samples) per inizializzare il delay
//      - process(juce::AudioBuffer<float>& samples) per applicare l'effetto delay
//      - reset() per resettare il delay
/////////////////////////////////////////////////////////////////////////////////////////////


#include "Delay.h"

#define INITAL_SAMPLE_RATE 44100

Delay::Delay() : _delay{2, INITAL_SAMPLE_RATE}, _read_i{0}, _write_i_sx{1}, _write_i_dx{1}, _sample_rate{INITAL_SAMPLE_RATE}, _max_delay{INITAL_SAMPLE_RATE}, _dry_wet{0.15}, _feedback{0.5}{}        // Costruttore dell'oggetto Delay con inizializzazione dei parametri


void Delay::reset()                                                                                                                                                                                   // Metodo per resettare il delay
{
    _delay.clear();                                                                                                                                                                                   // Pulisce il buffer circolare
}

void Delay::prepare(double sample_rate, int max_num_samples)                                                                                                                                          // Metodo per inizializzare il delay
{
    _sample_rate = sample_rate;                                                                                                                                                                       // Imposta il sample rate
    _max_delay = juce::jmax((int)sample_rate, max_num_samples);                                                                                                                                       // Imposta il massimo ritardo tra il sample rate e il numero massimo di campioni

    _delay.setSize(2, _max_delay);                                                                                                                                                                    // Imposta la dimensione del buffer circolare a 2 canali e _max_delay campioni
    _delay.clear();                                                                                                                                                                                   // Pulisce il buffer circolare
}

void Delay::process(juce::AudioBuffer<float> &buffer)                                                                                                                                                 // Metodo per applicare l'effetto delay
{
    float *left_channel = buffer.getWritePointer(0);                                                                                                                                                  // Ottiene il puntatore al canale sinistro
    float *right_channel = buffer.getWritePointer(1);                                                                                                                                                 // Ottiene il puntatore al canale destro

    float *left_delay = _delay.getWritePointer(0);                                                                                                                                                    // Ottiene il puntatore al buffer circolare del canale sinistro
    float *right_delay = _delay.getWritePointer(1);                                                                                                                                                   // Ottiene il puntatore al buffer circolare del canale destro

    for (int i = 0; i < buffer.getNumSamples(); i++, _read_i = (_read_i + 1) % _max_delay, _write_i_sx = (_write_i_sx + 1) % _max_delay, _write_i_dx = (_write_i_dx + 1) % _max_delay)                // Per ogni campione incrementa gli indici di lettura e scrittura
    {
        // Get output sample
        float out_left = left_channel[i];                                                                                                                                                             // Ottiene il campione di output sinistro
        float out_right = right_channel[i];                                                                                                                                                           // Ottiene il campione di output destro

        // Get delay
        float out_left_delay = left_delay[_read_i];                                                                                                                                                   // Ottiene il campione di delay sinistro
        float out_right_delay = right_delay[_read_i];                                                                                                                                                 // Ottiene il campione di delay destro

        // Write out samples
        left_channel[i] = out_left * (1.f - _dry_wet) + out_left_delay * _dry_wet;                                                                                                                   // Scrive il campione di output sinistro in base al dry/wet
        right_channel[i] = out_right * (1.f - _dry_wet) + out_right_delay * _dry_wet;                                                                                                                // Scrive il campione di output destro in base al dry/wet

        // Write delay
        switch (_mode_delay)                                                                                                                                                                         // Switch sulla modalità del delay
        {
        case Mode::mode_feedback:                                                                                                                                                                    // Caso modalità feedback
            left_delay[_write_i_sx] = out_left + out_left_delay * _feedback;                                                                                                                         // Scrive il campione di delay sinistro
            if (_sync_enable) right_delay[_write_i_sx] = out_right + out_right_delay * _feedback;                                                                                                    // Se il delay sincronizzato è abilitato, scrive il campione di delay destro in base al valore del delay sinistro
            else right_delay[_write_i_dx] = out_right + out_right_delay * _feedback;                                                                                                                 // Altrimenti scrive il campione di delay destro in base al valore del delay destro
            break;
        case Mode::mode_pingpong:                                                                                                                                                                    // Caso modalità pingpong
            switch (_mode_pingpong)                                                                                                                                                                  // Switch sulla modalità del delay pingpong
            {
            case Mode_clr::mode_center:                                                                                                                                                              // Caso modalità center
            {
                left_delay[_write_i_sx] = (out_left + out_right) / 2 + out_right_delay * _feedback;                                                                                                  // Scrive il campione di delay sinistro in base alla media dei campioni di output e delay
                right_delay[_write_i_dx] = (out_left + out_right) / 2 + out_left_delay * _feedback;                                                                                                  // Scrive il campione di delay destro in base alla media dei campioni di output e delay
            }
            break;

            case Mode_clr::mode_left:                                                                                                                                                                // Caso modalità left
            {
                left_delay[_write_i_sx] = (out_left + out_right) / 2 + out_right_delay * _feedback;                                                                                                  // Scrive il campione di delay sinistro in base alla media dei campioni di output e delay
                right_delay[_write_i_dx] = out_left_delay * _feedback;                                                                                                                               // Scrive il campione di delay destro in base al valore del delay sinistro
            }
            break;

            case Mode_clr::mode_right:                                                                                                                                                               // Caso modalità right
            {
                left_delay[_write_i_sx] = out_right_delay * _feedback;                                                                                                                               // Scrive il campione di delay sinistro in base al valore del delay destro
                right_delay[_write_i_dx] = (out_left + out_right) / 2 + out_left_delay * _feedback;                                                                                                  // Scrive il campione di delay destro in base alla media dei campioni di output e delay
            }
            break;
            }
            break;
        }
        

        // Clear read positions
        left_delay[_read_i] = 0.f;                                                                                                                                                                   // Pulisce il campione di delay sinistro appena letto
        right_delay[_read_i] = 0.f;                                                                                                                                                                  // Pulisce il campione di delay destro appena letto
    }
}

void Delay::set_delay_sx_in_ms(float delay_in_ms)                                                                                                                                                    // Metodo per impostare il ritardo del canale sinistro in un valore in ms
{
    int delay_in_samples = juce::jlimit(1, _max_delay, juce::roundToInt(delay_in_ms * _sample_rate / 1000.f));                                                                                       // Calcola il ritardo in campioni in base al sample rate e al ritardo in ms
    _write_i_sx = (_read_i + delay_in_samples) % _max_delay;                                                                                                                                         // Imposta l'indice di scrittura del canale sinistro in base al ritardo in campioni
}

void Delay::set_delay_dx_in_ms(float delay_in_ms)                                                                                                                                                    // Metodo per impostare il ritardo del canale destro in un valore in ms
{
    int delay_in_samples = juce::jlimit(1, _max_delay, juce::roundToInt(delay_in_ms * _sample_rate / 1000.f));                                                                                       // Calcola il ritardo in campioni in base al sample rate e al ritardo in ms
    _write_i_dx = (_read_i + delay_in_samples) % _max_delay;                                                                                                                                         // Imposta l'indice di scrittura del canale destro in base al ritardo in campioni
}

void Delay::set_feedback(float feedback)                                                                                                                                                             // Metodo per impostare il feedback
{
    _feedback = juce::jlimit(0.f, 1.f, feedback);                                                                                                                                                    // Limita il feedback tra 0 e 1 attraverso il metodo jlimit
}

void Delay::set_dry_wet(float dry_wet)                                                                                                                                                               // Metodo per impostare il dry/wet
{
    _dry_wet = juce::jlimit(0.f, 1.f, dry_wet);                                                                                                                                                      // Limita il dry/wet tra 0 e 1 attraverso il metodo jlimit
}

void Delay::enable_sync(bool enable)                                                                                                                                                                 // Metodo per abilitare il delay sincronizzato
{
    _sync_enable = enable;                                                                                                                                                                           // Imposta il delay sincronizzato in base al valore di enable
}

void Delay::set_delay_mode(int mode)                                                                                                                                                                 // Metodo per impostare la modalità del delay
{
    _mode_delay = static_cast<Mode>(juce::jlimit(0, 1, mode));                                                                                                                                       // Imposta la modalità del delay in base al valore di mode con un cast al tipo Mode
}

void Delay::set_pingpong_mode(int mode)                                                                                                                                                              // Metodo per impostare la modalità del delay pingpong
{
    _mode_pingpong = static_cast<Mode_clr>(juce::jlimit(0, 2, mode));                                                                                                                                // Imposta la modalità del delay pingpong in base al valore di mode con un cast al tipo Mode_clr
}
