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

Delay::Delay() : 
    _sample_rate(INITAL_SAMPLE_RATE),
    _max_delay(INITAL_SAMPLE_RATE),
    _dry_wet(0.15f),
    _feedback(0.5f),
    _sync_enable(false),
    _mode_pingpong(Mode_clr::mode_center),
    _mode_delay(Mode::mode_feedback)
{
    // Inizializziamo i smooth values
    _smooth_delay_left.setCurrentAndTargetValue(0.0f);
    _smooth_delay_right.setCurrentAndTargetValue(0.0f);
}

void Delay::reset()
{
    if (_delay_left) _delay_left->Reset();
    if (_delay_right) _delay_right->Reset();
    _smooth_delay_left.reset(_sample_rate, 0.05);
    _smooth_delay_right.reset(_sample_rate, 0.05);
}

void Delay::prepare(double sample_rate, int max_num_samples)
{
    _sample_rate = sample_rate;
    _max_delay = juce::jmax((int)sample_rate, max_num_samples);

    _delay_left = std::make_unique<daisysp::DelayLine<float, 192000>>();
    _delay_right = std::make_unique<daisysp::DelayLine<float, 192000>>();
    
    _delay_left->Init();
    _delay_right->Init();

    // Inizializza lo smoothing
    _smooth_delay_left.reset(sample_rate, 0.05); // 50ms di smoothing time
    _smooth_delay_right.reset(sample_rate, 0.05);
    _smooth_delay_left.setCurrentAndTargetValue(0.0f);
    _smooth_delay_right.setCurrentAndTargetValue(0.0f);
}

void Delay::process(juce::AudioBuffer<float>& buffer)
{
    float* left_channel = buffer.getWritePointer(0);
    float* right_channel = buffer.getWritePointer(1);

    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        float current_delay_left = _smooth_delay_left.getNextValue();
        float current_delay_right = _sync_enable ? current_delay_left : _smooth_delay_right.getNextValue();
        
        if (_delay_left && _delay_right)
        {
            _delay_left->SetDelay(current_delay_left);
            _delay_right->SetDelay(current_delay_right);
        }

        float out_left = left_channel[i];
        float out_right = right_channel[i];

        float out_left_delay = _delay_left->Read();
        float out_right_delay = _delay_right->Read();

        left_channel[i] = out_left * (1.f - _dry_wet) + out_left_delay * _dry_wet;
        right_channel[i] = out_right * (1.f - _dry_wet) + out_right_delay * _dry_wet;

        switch (_mode_delay)
        {
        case Mode::mode_feedback:
            _delay_left->Write(out_left + out_left_delay * _feedback);
            _delay_right->Write(out_right + out_right_delay * _feedback);
            break;

        case Mode::mode_pingpong:
            switch (_mode_pingpong)
            {
            case Mode_clr::mode_center:
                _delay_left->Write((out_left + out_right) / 2 + out_right_delay * _feedback);
                _delay_right->Write((out_left + out_right) / 2 + out_left_delay * _feedback);
                break;

            case Mode_clr::mode_left:
                _delay_left->Write((out_left + out_right) / 2 + out_right_delay * _feedback);
                _delay_right->Write(out_left_delay * _feedback);
                break;

            case Mode_clr::mode_right:
                _delay_left->Write(out_right_delay * _feedback);
                _delay_right->Write((out_left + out_right) / 2 + out_left_delay * _feedback);
                break;
            }
            break;
        }
    }
}

void Delay::set_delay_sx_in_ms(float delay_in_ms)
{
    if (_delay_left) {
        float delay_in_samples = static_cast<float>(juce::jlimit(1, _max_delay, 
            juce::roundToInt(delay_in_ms * _sample_rate / 1000.f)));
        _smooth_delay_left.setTargetValue(delay_in_samples);
        
        if (_sync_enable)
            _smooth_delay_right.setTargetValue(delay_in_samples);
    }
}

void Delay::set_delay_dx_in_ms(float delay_in_ms)
{
    if (!_sync_enable && _delay_right) {
        float delay_in_samples = static_cast<float>(juce::jlimit(1, _max_delay, 
            juce::roundToInt(delay_in_ms * _sample_rate / 1000.f)));
        _smooth_delay_right.setTargetValue(delay_in_samples);
    }
}

void Delay::set_feedback(float feedback)
{
    _feedback = juce::jlimit(0.f, 1.f, feedback);
}

void Delay::set_dry_wet(float dry_wet)
{
    _dry_wet = juce::jlimit(0.f, 1.f, dry_wet);
}

void Delay::enable_sync(bool enable)
{
    _sync_enable = enable;
    if (enable)
    {
        _smooth_delay_right.setTargetValue(_smooth_delay_left.getCurrentValue());
    }
}

void Delay::set_delay_mode(int mode)
{
    _mode_delay = static_cast<Mode>(juce::jlimit(0, 1, mode));
}

void Delay::set_pingpong_mode(int mode)
{
    _mode_pingpong = static_cast<Mode_clr>(juce::jlimit(0, 2, mode));
}
