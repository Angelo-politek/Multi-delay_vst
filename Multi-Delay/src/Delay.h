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


#ifndef __DELAY_HPP__
#define __DELAY_HPP__

#include <juce_audio_basics/juce_audio_basics.h>                                // Libreria JUCE

class Delay
{
public:
    enum Mode_clr                                                               // Enumerazione per la modalità pingpong
    {
        mode_center = 0,                                                        // Centro
        mode_left = 1,                                                          // Sinistra
        mode_right = 2,                                                         // Destra
    };

    enum Mode                                                                   // Enumerazione per la modalità delay
    {
        mode_feedback = 0,                                                      // Feedback
        mode_pingpong = 1,                                                      // Pingpong
    };

private:
    juce::AudioBuffer<float> _delay;                                            // Buffer circolare per il delay di tipo juce::AudioBuffer<float>
    int _read_i, _write_i_sx, _write_i_dx;                                      // Indici di lettura e scrittura per i canali sinistro e destro

    double _sample_rate;                                                        // Sample rate del progetto

    int _max_delay;                                                             // Massimo ritardo in campioni
    float _dry_wet;                                                             // Dry/Wet
    float _feedback;                                                            // Feedback
    bool _sync_enable;                                                          // Abilita il delay sincronizzato tra i canali sinistro e destro
    
    Mode_clr _mode_pingpong;                                                    // Modalità del delay pingpong
    Mode _mode_delay;                                                           // Modalità del delay

public:
    Delay();                                                                    // Costruttore dell'oggetto Delay


    void prepare(double sample_rate, int max_num_samples);                      // Metodo per inizializzare il delay
    void process(juce::AudioBuffer<float>& samples);                            // Metodo per applicare l'effetto delay
    void reset();                                                               // Metodo per resettare il delay

    void set_delay_dx_in_ms(float delay_in_ms);                                 // Metodo per impostare il ritardo del canale destro
    void set_delay_sx_in_ms(float delay_in_ms);                                 // Metodo per impostare il ritardo del canale sinistro
    void set_dry_wet(float value);                                              // Metodo per impostare il dry/wet
    void set_feedback(float feedback);                                          // Metodo per impostare il feedback
    void enable_sync(bool enable);                                              // Metodo per abilitare il delay sincronizzato

    void set_delay_mode(int mode);                                              // Metodo per impostare la modalità del delay
    void set_pingpong_mode(int mode);                                           // Metodo per impostare la modalità del delay pingpong

};

#endif // __DELAY_HPP__