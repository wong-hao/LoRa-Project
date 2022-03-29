//
// Created by 229077035 on 2021/11/21.
//

#ifndef COMPONENT_3_CONNECTTON_ERROR_CORRECTION_TIME_H
#define COMPONENT_3_CONNECTTON_ERROR_CORRECTION_TIME_H

#endif//COMPONENT_3_CONNECTTON_ERROR_CORRECTION_TIME_H

#pragma once

#include <ctime>
#include <sys/time.h>

#define NANOSECOND 1000000000

double difftimespec(struct timespec end, struct timespec beginning);//https://github.com/Lora-net/sx1302_hal/blob/4b42025d1751e04632c0b04160e0d29dbbb222a5/packet_forwarder/src/lora_pkt_fwd.c#L287

void anotherStartTime(struct timespec *start, struct timespec *interv, struct timespec *anotherstart);