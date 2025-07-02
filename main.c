#include "interface.h"
#include "efeitos.h"

#include <alsa/asoundlib.h>
#include <sndfile.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SAMPLE_RATE 48000
#define CHANNELS 2
#define FORMAT SND_PCM_FORMAT_FLOAT_LE
#define LATENCY 100000
#define BUFFER_SIZE 64

#define SAMPLE_WINDOW_BUFFER_SIZE   1024
#define TIME_LENGTH_SECONDS         14


int main(){
	int retVal = 0;
	snd_pcm_t *playbackHandle;
	snd_pcm_t *captureHandle;
	int err = 0;

	// Open PCM device for capture/playback
	if((err = snd_pcm_open(&playbackHandle, "plughw:1,0", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		fprintf(stderr, "Cannot open playback device: %s\n", snd_strerror(err));
		return err;
	}
	if((err = snd_pcm_open(&captureHandle, "plughw:1,1", SND_PCM_STREAM_CAPTURE, 0)) < 0) {
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
	for(int i = 0; i < signal_frames * channels; i++) {
		writeBuffer[i] = 0;
	}

	// Prepare processing
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

	printf("Starting capture\n");
	if((err = snd_pcm_start(captureHandle)) < 0) {
		fprintf(stderr, "Cannot start capture: %s\n", snd_strerror(err));
		return err;
	}

	printf("Waiting for capture device to deliver %d frames...\n", signal_frames);

	unsigned long avail = 0;
	do {
		avail = snd_pcm_avail(captureHandle);
		//printf("%d\n", avail);
	}
	while(avail < signal_frames);

	printf("Entering audio processing loop!\n");

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
