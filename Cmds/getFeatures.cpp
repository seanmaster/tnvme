/*
 * Copyright (c) 2011, Intel Corporation.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "getFeatures.h"
#include "../Utils/buffers.h"

#define BYTE_BITMASK_SEL       0x07

SharedGetFeaturesPtr GetFeatures::NullGetFeaturesPtr;
const uint8_t GetFeatures::Opcode = 0x0a;


GetFeatures::GetFeatures() : BaseFeatures(Trackable::OBJ_GETFEATURES)
{
    Init(Opcode, DATADIR_FROM_DEVICE, 64);

    // No cmd should ever be created which violates these masking possibilities
    send_64b_bitmask allowPrpMask = (send_64b_bitmask)
        (MASK_PRP1_PAGE | MASK_PRP2_PAGE);
    SetPrpAllowed(allowPrpMask);
}


GetFeatures::~GetFeatures()
{
}



void
GetFeatures::SetIntVecConfigIV(uint16_t iv)
{
    LOG_NRM("Setting interrupt vector (IV): 0x%04X", iv);
    SetWord(iv, 11, 0);
}


void
GetFeatures::SetSelField(uint8_t selVal)
{
	uint8_t newVal = GetByte(10, 1);
	LOG_NRM("Original DW10 byte 1: 0x%d", newVal);
	newVal &= ~BYTE_BITMASK_SEL;
	newVal |= (selVal & BYTE_BITMASK_SEL);
	LOG_NRM("Setting SEL field to: 0x%02X", newVal);
	SetByte(newVal, 10, 1);
}


