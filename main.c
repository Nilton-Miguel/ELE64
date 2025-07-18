#include "interface.h"
#include "efeitos.h"

#include <alsa/asoundlib.h>
#include <sndfile.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <errno.h>
#include <unistd.h>

#define SAMPLE_RATE 48000
#define CHANNELS 2
#define FORMAT SND_PCM_FORMAT_FLOAT_LE
#define LATENCY 10000
#define BUFFER_SIZE 64

#define SAMPLE_WINDOW_BUFFER_SIZE   1024
#define TIME_LENGTH_SECONDS         14


// A main recebe duas entradas, um inteiro com a quantidade de argumentos, e um ponteiro para uma string com os argumentos em si
int main(int argc, char** argv){

	// O primeiro argumento (índice 0) é sempre o comando que foi invocado para executar o programa
	//printf("argc: %d; comando usado: %s\n", argc, argv[0]);

	// getopt() itera sobre as strings recebidas e retorna todos os caracteres de opções que encontra
	int c;
	char *presetName = NULL;
	while((c = getopt(argc, argv, "hp:")) != -1) {
		switch(c) {
			case 'h':
				printf("Ajuda.\n");
				return 0;
				break;
			case 'p':
			 	presetName = optarg;
				break;
			default:
				printf("Tente usar '%s -h' para ver a ajuda.\n", argv[0]);
				return 1;
		}
	}
	if(!presetName) {
		printf("Nenhum arquivo de preset selecionado! Use '%s -p [preset]'\n", argv[0]);
		return -1;
	}

	int retVal = 0;
	snd_pcm_t *playbackHandle;
	snd_pcm_t *captureHandle;
	int err = 0;

	// Open PCM device for capture/playback
	if((err = snd_pcm_open(&playbackHandle, "plughw:0,0", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		fprintf(stderr, "Cannot open playback device: %s\n", snd_strerror(err));
		return err;
	}
	if((err = snd_pcm_open(&captureHandle, "plughw:0,1", SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		fprintf(stderr, "Cannot open capture device: %s\n", snd_strerror(err));
		return err;
	}

	// Configure parameters for capture/playback
	snd_pcm_format_t format = FORMAT;
	snd_pcm_access_t access = SND_PCM_ACCESS_RW_INTERLEAVED;
	unsigned int channels = CHANNELS;
	unsigned int rate = SAMPLE_RATE;
	int resample = 0;
	unsigned int latency = LATENCY;

	if((err = snd_pcm_set_params(captureHandle, format, access, channels, rate, resample, latency)) < 0) {
		fprintf(stderr, "Cannot set parameters: %s\n", snd_strerror(err));
		return err;
	}
	if((err = snd_pcm_set_params(playbackHandle, format, access, channels, rate, resample, latency)) < 0) {
		fprintf(stderr, "Cannot set parameters: %s\n", snd_strerror(err));
		return err;
	}

	// Alocate buffer
	unsigned int signal_frames = BUFFER_SIZE;
	unsigned int signal_samples = signal_frames * channels;
	unsigned int format_width = snd_pcm_format_width(format) / 8;

	printf("Buffer size: %d frames, %d samples\n", signal_frames, signal_samples);
	float *readBuffer = malloc(signal_samples * format_width);
	float *writeBuffer = malloc(signal_samples * format_width);
	float *esquerdo = malloc(signal_frames * format_width);
	float *direito = malloc(signal_frames * format_width);

	// Initialize output buffer with zeros for first loop
	for(int i = 0; i < signal_samples; i++) {
		writeBuffer[i] = 0.0;
	}

	// Prepare processing
	type_effect efeitoA, efeitoB, efeitoC;
	inicializar_efeito(&efeitoA);
	inicializar_efeito(&efeitoB);
	inicializar_efeito(&efeitoC);

	printf("Abrindo preset %s...\n", presetName);
	FILE *file_preset;
	if((file_preset = fopen(presetName, "rb")) == NULL) {
		fprintf(stderr, "Erro ao abrir preset: %s \n", strerror(errno));
		return errno;
	}
	carregar_preset(&efeitoA, &efeitoB, &efeitoC, file_preset);
	fclose(file_preset);

	imprime_efeito(&efeitoA);
	imprime_efeito(&efeitoB);
	imprime_efeito(&efeitoC);

	alocar_residuais(&efeitoA, rate);
	alocar_residuais(&efeitoB, rate);
	alocar_residuais(&efeitoC, rate);

	/*
	// Prepare interface
	if((err = snd_pcm_prepare(captureHandle)) < 0) {
		fprintf(stderr, "Cannot prepare capture: %s\n", snd_strerror(err));
		return err;
	}

	if((err = snd_pcm_prepare(playbackHandle)) < 0) {
		fprintf(stderr, "Cannot prepare playback: %s\n", snd_strerror(err));
		return err;
	}
	*/

	if((err = snd_pcm_start(captureHandle)) < 0) {
		fprintf(stderr, "Cannot start capture: %s\n", snd_strerror(err));
		return err;
	}

	printf("Captura iniciada. Esperando por %d frames...\n", signal_frames);

	unsigned long avail = 0;
	do {
		avail = snd_pcm_avail(captureHandle);
		//printf("%d\n", avail);
	}
	while(avail < signal_frames);

	printf("Iniciando processamento!\n");

	// Audio processing loop
	while(1) {
		// Read from capture
		//printf("To capture: %d frames; To playback: %d frames\n", snd_pcm_avail(captureHandle), snd_pcm_avail(playbackHandle));
		int readn = snd_pcm_readi(captureHandle, readBuffer, signal_frames);
		if(readn < 0) {
			//fprintf(stderr, "Read failed: %s\n", snd_strerror(readn));
			int recover = snd_pcm_recover(captureHandle, readn, 0);
			if(recover < 0) {
				fprintf(stderr, "Failed to recover ALSA state\n");
				err = readn;
				break;
			}
			//fprintf(stderr, "ALSA State recovered\n");
		}
		// Processing
		for(int i = 0; i < signal_frames; i++) {
			esquerdo[i] = readBuffer[channels * i];
			direito[i] = readBuffer[channels * i + 1];
		}
		//bufferi(readBuffer, writeBuffer, signal_frames);
		switch_process(&efeitoA, esquerdo, direito, esquerdo, direito, signal_frames, rate);
		switch_process(&efeitoB, esquerdo, direito, esquerdo, direito, signal_frames, rate);
		switch_process(&efeitoC, esquerdo, direito, esquerdo, direito, signal_frames, rate);
		for(int i = 0; i < signal_frames; i++) {
			writeBuffer[channels * i] = esquerdo[i];
			writeBuffer[channels * i + 1] = direito[i];
		}
		// Write to playback
		int writen = snd_pcm_writei(playbackHandle, writeBuffer, signal_frames);
		if(writen < 0) {
			//fprintf(stderr, "Write failed: %s\n", snd_strerror(writen));
			int recover = snd_pcm_recover(playbackHandle, writen, 0);
			if(recover < 0) {
				fprintf(stderr, "Failed to recover ALSA state\n");
				err = writen;
				break;
			}
			//fprintf(stderr, "ALSA State recovered\n");
		}
		//printf("Read %d frames, wrote %d frames.\n", readn, writen);

	}
	// Close interface
	snd_pcm_close(playbackHandle);
	snd_pcm_close(captureHandle);

	// Desalocar buffers e residuais 
	livrar_residuais(&efeitoA);
	livrar_residuais(&efeitoB);
	livrar_residuais(&efeitoC);
	free(readBuffer);
	free(writeBuffer);
	free(esquerdo);
	free(direito);

	return err;
}
