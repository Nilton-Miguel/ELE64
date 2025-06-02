
#include "efeitos.h"

#include <complex.h>
#include <sndfile.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <limits.h>

#define SAMPLE_WINDOW_BUFFER_SIZE 1024
#define TIME_LENGTH_SECONDS 4

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
    long int BUFFER_SIZE = signal_length * info_wav_entrada.channels;
    float buffer[BUFFER_SIZE];
    sf_count_t FRAMES = signal_length;
    sf_readf_float (wav_entrada, buffer, FRAMES);

    // cada sinal a ser processado deve receber apenas um canal do buffer 
    // esquerdo : canal 0
    // direito  : canal 1
    float esquerdo[signal_length];
    float direito[signal_length];

    for (i = 0; i < signal_length; i++)
    {
        esquerdo[i]     = buffer[info_wav_entrada.channels * i];
        direito[i]      = buffer[info_wav_entrada.channels * i + 1];
    }
    sf_close  (wav_entrada);
    //------------------------------------------------------------------------------------------------------------------

    // teste dos efeitos

    long int DURATION = (long int) (0.5 * sampling_rate);
    float DECAY = 0.3;

    // cada canal precisa ser processado separadamente
    float output_esquerdo[signal_length];
    float output_direito[signal_length];

    float residual_esquerdo[DURATION];
    float residual_direito[DURATION];

    for(i=0; i<DURATION; i++) 
    {
        residual_esquerdo[i] = 0;
        residual_direito[i] = 0;
    }

    echo(esquerdo, output_esquerdo, residual_esquerdo, signal_length, DURATION, DECAY);
    echo(direito, output_direito, residual_direito, signal_length, DURATION, DECAY);

    // audio output ----------------------------------------------------------------------------------------------------

        // gerar as structs de info dos arquivos

            SF_INFO info_wav_input;
            SF_INFO info_wav_output;

            info_wav_input.samplerate = sampling_rate;
            info_wav_input.channels = 2;
            info_wav_input.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

            info_wav_output.samplerate = sampling_rate;
            info_wav_output.channels = 2;
            info_wav_output.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

        // abrir os arquivos em modo de escrita
        SNDFILE * wav_input =   sf_open("in.wav", SFM_WRITE, &info_wav_input);
        SNDFILE * wav_output =  sf_open("out.wav", SFM_WRITE, &info_wav_output);

        // escrever os dados e liberar os arquivos

        float input_stereo[signal_length*info_wav_entrada.channels];
        for(i=0; i<signal_length; i++)
        {
            input_stereo[info_wav_entrada.channels*i] =   esquerdo[i];
            input_stereo[info_wav_entrada.channels*i+1] = direito[i];
        }
        
        float output_stereo[signal_length*info_wav_entrada.channels];
        for(i=0; i<signal_length; i++)
        {
            output_stereo[info_wav_entrada.channels*i] =   output_esquerdo[i];
            output_stereo[info_wav_entrada.channels*i+1] = output_direito[i];
        }
        
        sf_writef_float(wav_input, input_stereo, signal_length);
        sf_writef_float(wav_output, output_stereo, signal_length);

        sf_close(wav_input);
        sf_close(wav_output);

    // plotagem --------------------------------------------------------------------------------------------------------

        /*
        FILE *fp = NULL;
        FILE *gnupipe = NULL;
        char *GnuCommands [] = {"plot 'data.tmp' using 1:2 with points pt 7 ps 0.3, '' using 1:3 with points pt 7 ps 0.3"};

        fp      = fopen("data.tmp", "w");
        gnupipe = popen("gnuplot -persistent", "w");

        printf("\n");
        for(i=0; i<signal_length; i++){

            fprintf(fp, "%ld %f %f\n", i, buffer[i], input_stereo[i]);
        }
        fprintf(gnupipe, "%s\n", GnuCommands[0]); // invocar o gnuplot

        fclose(fp);
        fclose(gnupipe);
        */
               
    //------------------------------------------------------------------------------------------------------------------

    return 0;
}
