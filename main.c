
#include "interface.h"
#include "efeitos.h"

#include <sndfile.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SAMPLE_WINDOW_BUFFER_SIZE   100
#define TIME_LENGTH_SECONDS         20

long int i;

int main(){

    // leitura do audio -----------------------------------------------------------------------------------------------

        // abrir o arquivo de audio e obter metadados
        SF_INFO info_wav_entrada;
        info_wav_entrada.format = 0;
        SNDFILE * wav_entrada = sf_open("audio_snippets/audio_snippet_2.wav", SFM_READ, &info_wav_entrada);

        // info temporal
        long int sampling_rate = info_wav_entrada.samplerate;
        long int signal_length = TIME_LENGTH_SECONDS * sampling_rate;

    // um buffer de floats pra receber todos os frames (n canais por frame)
    sf_count_t REQUESTED_FRAMES = signal_length;
    long int BUFFER_SIZE = REQUESTED_FRAMES * info_wav_entrada.channels;
    float * in_buffer;
    in_buffer = (float*) malloc(BUFFER_SIZE * sizeof(float));
    
    sf_readf_float (wav_entrada, in_buffer, REQUESTED_FRAMES);

    // cada sinal a ser processado deve receber apenas um canal do buffer 
    // esquerdo : canal 0
    // direito  : canal 1
    float *esquerdo;
    esquerdo = (float*) malloc(signal_length * sizeof(float));

    float *direito;
    direito = (float*) malloc(signal_length * sizeof(float));
    
    for (i = 0; i < signal_length; i++)
    {
        esquerdo[i]     = in_buffer[info_wav_entrada.channels * i];
        direito[i]      = in_buffer[info_wav_entrada.channels * i + 1];
    }

    sf_close  (wav_entrada);

    // processamento do audio ------------------------------------------------------------------------------------------

    // cada canal precisa ser processado separadamente
    float *output_esquerdo;
    output_esquerdo = (float*) malloc(signal_length * sizeof(float));

    float *output_direito;
    output_direito = (float*) malloc(signal_length * sizeof(float));

    // leitura dos presets ---------------------------------------------------------------------------------------------

        type_effect efeitoA, efeitoB, efeitoC;

        inicializar_efeito(&efeitoA); 
        inicializar_efeito(&efeitoB); 
        inicializar_efeito(&efeitoC);

        FILE *file_preset;
        file_preset = fopen("preset_2.bin", "rb");
        carregar_preset(&efeitoA, &efeitoB, &efeitoC, file_preset);
        fclose(file_preset);

        imprime_efeito(&efeitoA);
        imprime_efeito(&efeitoB);
        imprime_efeito(&efeitoC);

        alocar_residuais(&efeitoA, sampling_rate);
        alocar_residuais(&efeitoB, sampling_rate);
        alocar_residuais(&efeitoC, sampling_rate);

    // processamento do sinal ------------------------------------------------------------------------------------------

    switch_process(&efeitoA, esquerdo, direito, output_esquerdo, output_direito, signal_length, sampling_rate);
    switch_process(&efeitoB, output_esquerdo, output_direito, output_esquerdo, output_direito, signal_length, sampling_rate);
    switch_process(&efeitoC, output_esquerdo, output_direito, output_esquerdo, output_direito, signal_length, sampling_rate);

    // audio output ----------------------------------------------------------------------------------------------------

    // gerar as structs de info dos arquivos

        SF_INFO info_wav_input;
        SF_INFO info_wav_output;

        info_wav_input.samplerate = sampling_rate;
        info_wav_input.channels = 2;
        info_wav_input.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;

        info_wav_output.samplerate = sampling_rate;
        info_wav_output.channels = 2;
        info_wav_output.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;

    // abrir os arquivos em modo de escrita
    SNDFILE * wav_input =   sf_open("in.wav", SFM_WRITE, &info_wav_input);
    SNDFILE * wav_output =  sf_open("out.wav", SFM_WRITE, &info_wav_output);

    // escrever os dados e liberar os arquivos
    
    float *input_stereo;
    input_stereo = (float*)malloc(signal_length*info_wav_entrada.channels*sizeof(float));

    for(i=0; i<signal_length; i++)
    {
        input_stereo[info_wav_entrada.channels*i] =   esquerdo[i];
        input_stereo[info_wav_entrada.channels*i+1] = direito[i];
    }
    
    float *output_stereo;
    output_stereo = (float*)malloc(signal_length*info_wav_entrada.channels*sizeof(float));

    for(i=0; i<signal_length; i++)
    {
        output_stereo[info_wav_entrada.channels*i] =   output_esquerdo[i];
        output_stereo[info_wav_entrada.channels*i+1] = output_direito[i];
    }
    
    sf_writef_float(wav_input, input_stereo, signal_length);
    sf_writef_float(wav_output, output_stereo, signal_length);
    
    sf_close(wav_input);
    sf_close(wav_output);

    // livrar as alocacoes fixas --------------------------------------------------------------------------------------

    free(in_buffer);

    free(esquerdo);
    free(output_esquerdo);
    
    free(direito);
    free(output_direito);

    free(input_stereo);
    free(output_stereo);

    // livrar as alocacoes residuais dos efeitos

    livrar_residuais(&efeitoA);
    livrar_residuais(&efeitoB);
    livrar_residuais(&efeitoC);

    // ----------------------------------------------------------------------------------------------------------------
    return 0;
}
