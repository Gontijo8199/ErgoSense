#pragma once

#include <Arduino.h>
#include "../sensors/vl53l4cd.h"
#include "../sensors/vl53l5cx.h"

#define L4CD_QUEUE_SIZE 40
#define L5CX_QUEUE_SIZE 60

struct L4CD_Queue {
    VL53L4CD_Data buffer[L4CD_QUEUE_SIZE];
    int head = 0;
    int tail = 0;
    int count = 0;
};

struct L5CX_Queue {
    VL53L5CX_Data buffer[L5CX_QUEUE_SIZE];
    int head = 0;
    int tail = 0;
    int count = 0;
};

void queueL4CD(const VL53L4CD_Data &data);
void processL4CDQueue();

void queueL5CX(const VL53L5CX_Data &data);
void processL5CXQueue();
