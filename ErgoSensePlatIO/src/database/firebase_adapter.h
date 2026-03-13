#pragma once

#include <Arduino.h>
#include "../sensors/vl53l4cd.h"
#include "../sensors/vl53l5cx.h"
#include "firebase_queue.h"


bool sendL4CDBatchToFirebase(const L4CD_Batch &batch);
bool sendL5CXBatchToFirebase(const L5CX_Batch &batch);