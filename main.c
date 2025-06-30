#include "interface.h"
#include "efeitos.h"

#include <alsa/asoundlib.h>
#include <sndfile.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SAMPLE_RATE 96000
#define CHANNELS 2
#define FORMAT SND_PCM_FORMAT_S32_LE
#define LATENCY 500000

#define SAMPLE_WINDOW_BUFFER_SIZE   1024
#define TIME_LENGTH_SECONDS         14

long int i;

int main(){
	int retVal = 0;
	snd_pcm_t *playbackHandle;
	snd_pcm_t *captureHandle;
	int err = 0;

	// Open PCM device for capture/playback
	if((err = snd_pcm_open(&playbackHandle, "hw:1,0", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		fprintf(stderr, "Cannot open playback device: %s\n", snd_strerror(err));
		return err;
	}
	if((err = snd_pcm_open(&captureHandle, "hw:1,1", SND_PCM_STREAM_CAPTURE, 0)) < 0) {
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
	unsigned int signal_length = rate/1000 * latency/1000;
	int format_width = snd_pcm_format_width(format) / 8;

	printf("Buffer size: %d frames, %d samples\n", signal_length, signal_length * channels);
	int *readBuffer = malloc(signal_length * format_width * channels);
	int *writeBuffer = malloc(signal_length * format_width * channels);
	float *esquerdo = malloc(signal_length * sizeof(float));
	float *direito = malloc(signal_length * sizeof(float));

	// Initialize output buffer with zeros for first loop
	for(int i = 0; i < signal_length * channels; i++) {
		writeBuffer[i] = 0;
	}

	// Prepare processing
	type_effect efeitoA, efeitoB, efeitoC;
	inicializar_efeito(&efeitoA);
	inicializar_efeito(&efeitoB);
	inicializar_efeito(&efeitoC);

	FILE *file_preset;
	file_preset = fopen("preset_1.bin", "rb");
	carregar_preset(&efeitoA, &efeitoB, &efeitoC, file_preset);
	fclose(file_preset);

	imprime_efeito(&efeitoA);
	imprime_efeito(&efeitoB);
	imprime_efeito(&efeitoC);

	alocar_residuais(&efeitoA, SAMPLE_RATE);
	alocar_residuais(&efeitoB, SAMPLE_RATE);
	alocar_residuais(&efeitoC, SAMPLE_RATE);

	// Prepare interface
	if((err = snd_pcm_prepare(playbackHandle)) < 0) {
		fprintf(stderr, "Cannot prepare playback: %s\n", snd_strerror(err));
		return err;
	}
	if((err = snd_pcm_prepare(captureHandle)) < 0) {
		fprintf(stderr, "Cannot prepare capture: %s\n", snd_strerror(err));
		return err;
	}

	// Audio processing loop
	while(1) {
		// Read from capture
		int readn = snd_pcm_readi(captureHandle, readBuffer, signal_length);
		if(readn < 0) {
			fprintf(stderr, "Read failed: %s\n", snd_strerror(readn));
			int recover = snd_pcm_recover(captureHandle, readn, 0);
			if(recover < 0) {
				fprintf(stderr, "Failed to recover ALSA state\n");
				err = readn;
				break;
			}
			fprintf(stderr, "ALSA State recovered\n");
		}
		// Write to playback
		int writen = snd_pcm_writei(playbackHandle, writeBuffer, signal_length);
		if(writen < 0) {
			fprintf(stderr, "Write failed: %s\n", snd_strerror(writen));
			int recover = snd_pcm_recover(playbackHandle, writen, 0);
			if(recover < 0) {
				fprintf(stderr, "Failed to recover ALSA state\n");
				err = writen;
				break;
			}
			fprintf(stderr, "ALSA State recovered\n");
		}
		printf("Read %d frames, wrote %d frames.\n", readn, writen);

		// Processing
		for(int i = 0; i < signal_length; i++) {
			esquerdo[i] = (float)readBuffer[channels * i];
			direito[i] = (float)readBuffer[channels * i + 1];
		}
		switch_process(&efeitoA, esquerdo, direito, esquerdo, direito, signal_length, SAMPLE_RATE);
		//switch_process(&efeitoB, esquerdo, direito, esquerdo, direito, signal_length, SAMPLE_RATE);
		//switch_process(&efeitoC, esquerdo, direito, esquerdo, direito, signal_length, SAMPLE_RATE);
		for(int i = 0; i < signal_length; i++) {
			writeBuffer[channels * i] = (int)esquerdo[i];
			writeBuffer[channels * i + 1] = (int)direito[i];
		}
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
