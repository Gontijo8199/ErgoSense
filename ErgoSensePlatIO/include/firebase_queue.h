#pragma once

#include <Arduino.h>
#include "vl53l4cd.h"
#include "vl53l5cx.h"

#define L4CD_QUEUE_SIZE 500
#define L5CX_QUEUE_SIZE 300

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

#define L4CD_BATCH_SIZE 10
#define L5CX_BATCH_SIZE 4


struct L4CD_Batch {
    VL53L4CD_Data items[L4CD_BATCH_SIZE];
    int count = 0;
};


struct L5CX_Batch {
    VL53L5CX_Data items[L5CX_BATCH_SIZE];
    int count = 0;
};

void queueL4CD(const VL53L4CD_Data &data);
void processL4CDQueue();

void queueL5CX(const VL53L5CX_Data &data);
void processL5CXQueue();
