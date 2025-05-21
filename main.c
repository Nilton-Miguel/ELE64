
#include "geradores.h"
#include "efeitos.h"

#include <complex.h>
#include <fftw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SAMPLING_WINDOW_SIZE 1024
int i;

int main(){

    int sampling_rate = 48000;
    float time_length = 10;
    int signal_length = time_length * sampling_rate;

    fftw_complex *sinal;
    sinal = (fftw_complex*) fftw_malloc(signal_length * sizeof(fftw_complex));

    fftw_complex *auxiliar;
    auxiliar = (fftw_complex*) fftw_malloc(signal_length * sizeof(fftw_complex));

    fftw_complex *soft;
    soft = (fftw_complex*) fftw_malloc(signal_length * sizeof(fftw_complex));

    fftw_complex *hard;
    hard = (fftw_complex*) fftw_malloc(signal_length * sizeof(fftw_complex));

        frequencia_pura(sinal, signal_length, sampling_rate, 261.0, 1000.0);
        adicionar_ruido(sinal, signal_length, 10.0);
        
        frequencia_pura(auxiliar, signal_length, sampling_rate, 2049.0, 400.0);
        adicionar_ruido(auxiliar, signal_length, 10.0);
        for(i=0; i<signal_length; i++) 
            sinal[i] += auxiliar[i];

    saturador_soft(sinal, soft, signal_length, 3000.0, 3.0);
    saturador_hard(sinal, hard, signal_length, 3000.0, 3.0);  

    // audio output -----------------------------------------------------------------------------------------------------

        short int *sinal_escrita;
        sinal_escrita = (short int *) malloc(signal_length * sizeof(short int));
        rebaixar_16bits(sinal, sinal_escrita, signal_length);

        short int *soft_escrita;
        soft_escrita = (short int *) malloc(signal_length * sizeof(short int));
        rebaixar_16bits(soft, soft_escrita, signal_length);

        short int *hard_escrita;
        hard_escrita = (short int *) malloc(signal_length * sizeof(short int));
        rebaixar_16bits(hard, hard_escrita, signal_length);

        wav_header wav_file_header;

        strncpy(wav_file_header.data, "data", 4);
        strncpy(wav_file_header.riff, "RIFF", 4);
        strncpy(wav_file_header.fmt,  "fmt ", 4);
        strncpy(wav_file_header.wave, "WAVE", 4);
        wav_file_header.chunk_size          = 16;
        wav_file_header.format_tag          = 1;
        wav_file_header.num_chans           = 1;
        wav_file_header.sample_rate         = sampling_rate;
        wav_file_header.bits_per_sample     = 16;
        wav_file_header.bytes_per_sample    = (wav_file_header.bits_per_sample / 8) * wav_file_header.num_chans;
        wav_file_header.bytes_per_second    = wav_file_header.bytes_per_sample * wav_file_header.sample_rate;

        const int buffer_size   = wav_file_header.sample_rate * time_length;

        wav_file_header.dlength = buffer_size * wav_file_header.bytes_per_sample;
        wav_file_header.flength = wav_file_header.dlength + 44;

        FILE *wav_file_pointer = fopen("sinal.wav", "w");
        fwrite(&wav_file_header, 1, sizeof(wav_file_header), wav_file_pointer);
        fwrite(sinal_escrita, 8, signal_length, wav_file_pointer);
        fclose(wav_file_pointer);

        wav_file_pointer = fopen("soft.wav", "w");
        fwrite(&wav_file_header, 1, sizeof(wav_file_header), wav_file_pointer);
        fwrite(soft_escrita, 8, signal_length, wav_file_pointer);
        fclose(wav_file_pointer);

        wav_file_pointer = fopen("hard.wav", "w");
        fwrite(&wav_file_header, 1, sizeof(wav_file_header), wav_file_pointer);
        fwrite(hard_escrita, 8, signal_length, wav_file_pointer);
        fclose(wav_file_pointer);

    // plotagem --------------------------------------------------------------------------------------------------------

        FILE *fp = NULL;
        FILE *gnupipe = NULL;
        char *GnuCommands [] = {"plot 'data.tmp' using 1:2 with points pt 7 ps 0.3"};

        fp      = fopen("data.tmp", "w");
        gnupipe = popen("gnuplot -persistent", "w");

        printf("\n");
        for(i=0; i<signal_length; i++){

            fprintf(fp, "%d %f\n", i, creal(hard_escrita[i]));
        }
        //fprintf(gnupipe, "%s\n", GnuCommands[0]); // invocar o gnuplot

        fclose(fp);
        fclose(gnupipe);

    //------------------------------------------------------------------------------------------------------------------

    fftw_free(sinal);
    fftw_free(auxiliar);
    fftw_free(soft);
    fftw_free(hard);

    free(sinal_escrita);
    free(soft_escrita);
    free(hard_escrita);
    return 0;
}
