#include <ohaudio/native_audiostreambuilder.h>
#include <ohaudio/native_audiorenderer.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

static int64_t position;
static int32_t write_audio(OH_AudioRenderer *renderer, void *user, void *buffer, int32_t bytes)
{
    int16_t *out = buffer;
    int frames = bytes / 4;
    for (int i = 0; i < frames; i++, position++) {
        double gain = 0;
        if (position < 48000) {
            gain = 0.06;
            if (position < 2400) gain *= position / 2400.0;
            if (position > 45600) gain *= (48000 - position) / 2400.0;
        }
        int16_t sample = (int16_t)(sin(position * (2.0 * 3.141592653589793 * 660 / 48000)) * gain * 32767);
        out[2*i] = sample;
        out[2*i+1] = sample;
    }
    return 0;
}
int main(void)
{
    OH_AudioStreamBuilder *builder = NULL;
    OH_AudioRenderer *renderer = NULL;
    OH_AudioRenderer_Callbacks callbacks = {0};
    callbacks.OH_AudioRenderer_OnWriteData = write_audio;
    int ret = OH_AudioStreamBuilder_Create(&builder, AUDIOSTREAM_TYPE_RENDERER);
    printf("builder=%d\n", ret);
    if (ret) return 1;
    OH_AudioStreamBuilder_SetSamplingRate(builder, 48000);
    OH_AudioStreamBuilder_SetChannelCount(builder, 2);
    OH_AudioStreamBuilder_SetSampleFormat(builder, AUDIOSTREAM_SAMPLE_S16LE);
    OH_AudioStreamBuilder_SetRendererInfo(builder, AUDIOSTREAM_USAGE_MUSIC);
    OH_AudioStreamBuilder_SetLatencyMode(builder, AUDIOSTREAM_LATENCY_MODE_NORMAL);
    OH_AudioStreamBuilder_SetRendererCallback(builder, callbacks, NULL);
    ret = OH_AudioStreamBuilder_GenerateRenderer(builder, &renderer);
    printf("renderer=%d\n", ret);
    if (!ret) {
        ret = OH_AudioRenderer_Start(renderer);
        printf("start=%d\n", ret); fflush(stdout);
        sleep(4);
        printf("stop=%d frames=%lld\n", OH_AudioRenderer_Stop(renderer), (long long)position);
        OH_AudioRenderer_Release(renderer);
    }
    OH_AudioStreamBuilder_Destroy(builder);
    return ret != 0;
}
