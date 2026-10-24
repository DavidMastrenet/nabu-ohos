#pragma once
#include <stdint.h>
#include <stdatomic.h>
typedef void (*NabuSensorCallback)(int, uint64_t, const float[3], void *);
int NabuSscRun(NabuSensorCallback callback, void *user, atomic_int *stop);
