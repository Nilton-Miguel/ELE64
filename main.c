
#include "efeitos.h"

#include <complex.h>
#include <sndfile.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <limits.h>

#define SAMPLE_WINDOW_BUFFER_SIZE 1024
#define TIME_LENGTH_SECONDS 5

long int i;

int main(){

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

    // o sinal recebe apenas um dos canais de cada frame
    float sinal[signal_length];
    for (i = 0; i < BUFFER_SIZE/info_wav_entrada.channels; i++) sinal[i] = buffer[info_wav_entrada.channels*i];

    sf_close  (wav_entrada);
    //------------------------------------------------------------------------------------------------------------------

    // teste dos efeitos

    long int DURATION = (long int) (0.5 * sampling_rate);
    float DECAY = 0.3;

    float output[signal_length];
    float residual[DURATION];
    for(i=0; i<DURATION; i++) residual[i] = 0;

    echo(sinal, output, residual, signal_length, DURATION, DECAY);

    // audio output ----------------------------------------------------------------------------------------------------

        // gerar as structs de info dos arquivos

            SF_INFO info_wav_input;
            SF_INFO info_wav_output;

            info_wav_input.samplerate = sampling_rate;
            info_wav_input.channels = 1;
            info_wav_input.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

            info_wav_output.samplerate = sampling_rate;
            info_wav_output.channels = 1;
            info_wav_output.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

        // abrir os arquivos em modo de escrita
        SNDFILE * wav_input =   sf_open("in.wav", SFM_WRITE, &info_wav_input);
        SNDFILE * wav_output =  sf_open("out.wav", SFM_WRITE, &info_wav_output);

        // escrever os dados e liberar os arquivos
        sf_write_float(wav_input, sinal, signal_length);
        sf_write_float(wav_output, output, signal_length);

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

            fprintf(fp, "%ld %f\n", i, sinal[i]);
        }
        fprintf(gnupipe, "%s\n", GnuCommands[0]); // invocar o gnuplot

        fclose(fp);
        fclose(gnupipe);
        */

    //------------------------------------------------------------------------------------------------------------------

    return 0;
}
