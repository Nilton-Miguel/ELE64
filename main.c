
#include "efeitos.h"

#include <complex.h>
#include <sndfile.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SAMPLE_WINDOW_BUFFER_SIZE   1024
#define TIME_LENGTH_SECONDS         4

long int i;

int main(){

    // leitura do audio -----------------------------------------------------------------------------------------------

        // abrir o arquivo de audio e obter metadados
        SF_INFO info_wav_entrada;
        info_wav_entrada.format = 0;
        SNDFILE * wav_entrada = sf_open("audio_snippets/audio_snippet_2.wav", SFM_READ, &info_wav_entrada);

        printf("\n%ld frames no arquivo\n", info_wav_entrada.frames);

        // info temporal
        long int sampling_rate = info_wav_entrada.samplerate;
        long int signal_length = TIME_LENGTH_SECONDS * sampling_rate;

    // um buffer de floats pra receber todos os frames (n canais por frame)
    sf_count_t REQUESTED_FRAMES = signal_length;
    long int BUFFER_SIZE = REQUESTED_FRAMES * info_wav_entrada.channels;
    float * in_buffer;
    in_buffer = (float*) malloc(BUFFER_SIZE * sizeof(float));

    printf("%ld frames requeridos\n", REQUESTED_FRAMES);
    printf("%ld floats no buffer\n", BUFFER_SIZE);
    printf("%d segundos\n\n", TIME_LENGTH_SECONDS);
    
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
    
    // teste dos efeitos -----------------------------------------------------------------------------------------------
    
    // cada canal precisa ser processado separadamente
    float *output_esquerdo;
    output_esquerdo = (float*) malloc(signal_length * sizeof(float));

    float *output_direito;
    output_direito = (float*) malloc(signal_length * sizeof(float));

    // parametros do BUFFER ----------------------------------------------------------- ok
    /*
    buffer(esquerdo, output_esquerdo, signal_length);
    buffer(direito, output_direito, signal_length);
    */

    // parametros do AMPLIFICADOR SOFT ------------------------------------------------ ok
    /*
    float GANHO = 7;

    amplificador_soft(esquerdo, output_esquerdo, signal_length, GANHO);
    amplificador_soft(direito, output_direito, signal_length, GANHO);
    */
    
    // parametros do AMPLIFICADOR HARD ------------------------------------------------ ok
    /*
    float GANHO = 0.1;

    amplificador(esquerdo, output_esquerdo, signal_length, GANHO);
    amplificador(direito, output_direito, signal_length, GANHO);
    */

    // parametros do SATURADOR SOFT --------------------------------------------------- ok
    /*
    float JANELA = 0.6;
    float GANHO = 6;

    saturador_soft(esquerdo, output_esquerdo, signal_length, JANELA, GANHO);
    saturador_soft(direito, output_direito, signal_length, JANELA, GANHO);
    */

    // parametros do SATURADOR HARD --------------------------------------------------- ok
    /*
    float JANELA = 1;
    float GANHO = 3;

    saturador_hard(esquerdo, output_esquerdo, signal_length, JANELA, GANHO);
    saturador_hard(direito, output_direito, signal_length, JANELA, GANHO);
    */

    // parametros do ECHO ------------------------------------------------------------- ok
    /*
    long int DURATION = 0.5 * sampling_rate;
    float DECAY = 0.3;

    float *residual_esquerdo;
    residual_esquerdo = (float*) malloc(DURATION * sizeof(float));

    float *residual_direito;
    residual_direito = (float*) malloc(DURATION * sizeof(float));

    for(i=0; i<DURATION; i++) 
    {
        residual_esquerdo[i] = 0;
        residual_direito[i] = 0;
    }

    echo(esquerdo, output_esquerdo, residual_esquerdo, signal_length, DURATION, DECAY);
    echo(direito, output_direito, residual_direito, signal_length, DURATION, DECAY);
    */

    // parametros do LOWPASS ---------------------------------------------------------- ok
    /*
    float CUTOFF = 100;

    float lowpass_residual_esquerdo = 0;
    float lowpass_residual_direito = 0;

    printf("residual antes: %f %f\n", lowpass_residual_esquerdo, lowpass_residual_direito);

    lowpass(esquerdo, output_esquerdo, &lowpass_residual_esquerdo, sampling_rate, signal_length, CUTOFF);
    lowpass(direito, output_direito, &lowpass_residual_direito, sampling_rate, signal_length, CUTOFF);

    printf("residual depois: %f %f\n", lowpass_residual_esquerdo, lowpass_residual_direito);
    */

    // parametros do HIGHPASS --------------------------------------------------------- ok
    /*
    float CUTOFF = 1000;

    float highpass_x_residual_esquerdo, highpass_y_residual_esquerdo    = 0;
    float highpass_x_residual_direito,  highpass_y_residual_direito     = 0;

    printf("x antes: %f %f y antes: %f %f\n", 
        highpass_x_residual_esquerdo, highpass_x_residual_direito, highpass_y_residual_esquerdo, highpass_y_residual_direito);

    highpass(esquerdo, output_esquerdo, &highpass_x_residual_esquerdo, &highpass_y_residual_esquerdo, sampling_rate, signal_length, CUTOFF);
    highpass(direito, output_direito, &highpass_x_residual_direito, &highpass_y_residual_direito, sampling_rate, signal_length, CUTOFF);

    printf("x depois: %f %f y depois: %f %f\n", 
        highpass_x_residual_esquerdo, highpass_x_residual_direito, highpass_y_residual_esquerdo, highpass_y_residual_direito);
    */

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

    // plotagem --------------------------------------------------------------------------------------------------------

        /*
        FILE *fp = NULL;
        FILE *gnupipe = NULL;
        char *GnuCommands [] = {"plot 'data.tmp' using 1:2 with points pt 7 ps 0.3, '' using 1:3 with points pt 7 ps 0.3"};

        fp      = fopen("data.tmp", "w");
        gnupipe = popen("gnuplot -persistent", "w");

        printf("\n");
        for(i=0; i<signal_length; i++){

            fprintf(fp, "%ld %f %f\n", i, esquerdo[i], output_esquerdo[i]);
        }
        fprintf(gnupipe, "%s\n", GnuCommands[0]); // invocar o gnuplot

        fclose(fp);
        fclose(gnupipe);
        */
        
    //------------------------------------------------------------------------------------------------------------------

    free(in_buffer);

    free(esquerdo);
    free(direito);

    free(output_esquerdo);
    free(output_direito);

    //free(residual_esquerdo);
    //free(residual_direito);

    free(input_stereo);
    free(output_stereo);

    return 0;
}
