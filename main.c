
#include "geradores.h"
#include "efeitos.h"

#include <complex.h>
#include <fftw3.h>
#include <sndfile.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define SAMPLING_WINDOW_SIZE 1024
long int i;

int main(){

        // abrir o arquivo de auido e obter seus dados
        SF_INFO info_wav_entrada;
        info_wav_entrada.format = 0;
        SNDFILE * wav_entrada = sf_open("audio_snippet_2.wav", SFM_READ, &info_wav_entrada) ;

        // sampling rate agora vem do arquivo de audio
        long int sampling_rate = info_wav_entrada.samplerate;

    float time_length = 2.5;
    long int signal_length = time_length * sampling_rate;

    fftw_complex *sinal;
    sinal = (fftw_complex*) fftw_malloc(signal_length * sizeof(fftw_complex));

    fftw_complex *soft;
    soft = (fftw_complex*) fftw_malloc(signal_length * sizeof(fftw_complex));

    fftw_complex *hard;
    hard = (fftw_complex*) fftw_malloc(signal_length * sizeof(fftw_complex));

        // um buffer de floats
        long int BUFFER_SIZE = signal_length * info_wav_entrada.channels;
        float buffer[BUFFER_SIZE];
        sf_count_t FRAMES = signal_length;

        //printf("%ld", BUFFER_SIZE);

        sf_readf_float (wav_entrada, buffer, FRAMES);

        for (i = 0; i < BUFFER_SIZE/2; i++)
        {   
            sinal[i] = SHRT_MAX * buffer[info_wav_entrada.channels*i];
            //printf("%f\n", creal(sinal[i]));
        }

        sf_close  (wav_entrada);

    double SATURATION_WINDOW = 1 * SHRT_MAX;
    double SATURATION_GAIN   = 0.1;

    saturador_soft(sinal, soft, signal_length, SATURATION_WINDOW, SATURATION_GAIN);
    saturador_hard(sinal, hard, signal_length, SATURATION_WINDOW, SATURATION_GAIN);  

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

            fprintf(fp, "%ld %f\n", i, creal(hard_escrita[i]));
        }
        //fprintf(gnupipe, "%s\n", GnuCommands[0]); // invocar o gnuplot

        fclose(fp);
        fclose(gnupipe);

    //------------------------------------------------------------------------------------------------------------------

    fftw_free(sinal);
    fftw_free(soft);
    fftw_free(hard);
    
    free(sinal_escrita);
    free(soft_escrita);
    free(hard_escrita);
    return 0;
}
