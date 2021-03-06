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

#ifndef _PRPDATA_H_
#define _PRPDATA_H_

#include "tnvme.h"
#include "dnvme.h"
#include "../Singletons/memBuffer.h"

typedef enum {
    DATADIR_NONE,
    DATADIR_FROM_DEVICE,
    DATADIR_TO_DEVICE,
    DATADIR_BIDIRECTIONAL
} DataDir;


/**
* This class is the interface for the user data buffer associated with the PRP
* entries of a cmd.
*
* @note This class may throw exceptions.
*/
class PrpData
{
public:
    PrpData();
    virtual ~PrpData();

    /**
     * Accept a previously created Read/Write (RW) user space buffer as the
     * user data buffer to be populated in the PRP fields of a cmd.
     * @param prpFields Pass the appropriate combination of bitfields to
     *      indicate to dnvme how to populate the PRP fields of a cmd with
     *      this the buffer.
     * @param memBuffer Hand off this cmds data buffer.
     */
    void SetPrpBuffer(send_64b_bitmask prpFields, SharedMemBufferPtr memBuffer);
    void SetPrpBufferUnsafe(send_64b_bitmask prpFields, SharedMemBufferPtr memBuffer);
    /**
     * This method is only safe to use if and only if the
     * SetPrpBuffer(SharedMemBufferPtr) was used to setup a buffer. This allows
     * access to that RW buffer, however some commands must enforce RO buffers
     * and thus this method won't be useful because it cannot guarantee RO
     * access. A MemBuffer::NullMemBufferPtr will be returned if the
     * the SetPrpBuffer(SharedMemBufferPtr) version wasn't used to setup a
     * buffer, or there is no buffer setup at all.
     * @return A pointer to RW memory, otherwise MemBuffer::NullMemBufferPtr.
     */
    SharedMemBufferPtr GetRWPrpBuffer() { return mBufRW; }

    /**
     * This method will always return a buffer if one is presently setup. Any
     * of the SetPrpBuffer() methods can be used to setup a buffer. This is
     * safe due to the const return pointer.
     * @return A pointer to the buffer, otherwise NULL indicates no buffer was
     *      setup, i.e. there is no user data at all for the PRP fields.
     */
    uint8_t const *GetROPrpBuffer() const;

    /**
     * This methods will always return a size if a buffer is presently setup.
     * @return The number of bytes encompassing any buffer which has been setup
     */
    uint64_t GetPrpBufferSize() const { return mBufSize; }

    /// Solely used to notify dnvme how to issue a PRP data buffer
    send_64b_bitmask GetPrpBitmask() const { return mPrpFields; }

    /// Returns the direction of data transfer for the PRP buffer
    virtual DataDir GetDataDir() const = 0;

    /**
     * Append the entire contents of this cmd's payload to the named file.
     * @param filename Pass the filename as generated by macro
     *      FileSystem::PrepDumpFile().
     * @param fileHdr Pass a custom file header description to dump
     */
    void Dump(DumpFilename filename, string fileHdr) const;


protected:
    /**
     * Accept a previously created Read Only (RO) IOQ buffer as the user data to
     * be populated in the PRP fields of a cmd. This method is only intended to
     * be used with the creation of IOQ's because those memories are never
     * allowed to be modified by user space; kernel only access, but user space
     * has RO access to those memories for debug.
     * @param prpFields Pass the appropriate combination of bitfields to
     *      indicate to dnvme how to populate the PRP fields of a cmd with
     *      this the buffer.
     * @param bufSize Pass the number of bytes consisting of memBuffer
     * @param memBuffer Point to an IOQ's RO memory.
     */
    void SetPrpBuffer(send_64b_bitmask prpFields, uint8_t const *memBuffer,
        uint64_t bufSize);

    /**
     * Each cmd has unique requirements as to how its PRP ptrs can be
     * interpreted This method should be called during child class instantiation
     * to set all the ways a PRP ptr can be interpreted. If ever a test attempts
     * to set the send_64b_bitmask to anything that what is allowed, an
     * exception will be thrown. There is no usefulness in supplying illegal
     * PRP ptrs to the hdw because the spec states the outcome is unpredictable.
     * @param allowedBitmask Pass all allowed bitmask ptr values
     */
    void SetPrpAllowed(send_64b_bitmask allowedBitmask);


private:
    /// Used for RW memory for a cmd's user data
    SharedMemBufferPtr mBufRW;
    /// Used for RO memory assoc with a IOQ's data memory
    uint8_t const *mBufRO;
    /// Number of bytes consisting of either mBufRO or mBufRW
    uint64_t mBufSize;

    /// Allowed values of send_64b_bitmask for a cmd's PRP ptrs
    send_64b_bitmask mPrpAllowed;
    /// What fields in a cmd can we populate for the buffer?
    send_64b_bitmask mPrpFields;
};


#endif
