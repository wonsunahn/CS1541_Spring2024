#ifndef MEMREQUEST_H
#define MEMREQUEST_H

#include <stdint.h>
#include <assert.h>
#include <string>

enum MemOperation {
  MemRead = 0,
  MemWrite,
  MemWriteBack,
};

/** 
 * @copyright Copyright 2024 Spring 2024 Wonsun Ahn. All rights reserved.
 * 
 * @brief A memory request.
 *
 * There are three types of memory requests: MemRead, MemWrite, and
 * MemWriteBack.  The memory request conceptually travels down the memory
 * hierarchy and then up incurring latency as it visits each memory object
 * in its path.
 */
class MemRequest {
  protected:
    /** Latency incurred by the memory request */
    uint32_t latency;

    /** The address for the memory request */
    uint32_t addr;

    /** The type of memory operation for the memory request */
    MemOperation memOp;

    /** Whether this is a write request pending in a write buffer */
    bool isPendingInWriteBuffer;

    /** How many cycles the write request should be pending in write buffer */
    uint32_t writeBufferPendingCycles;

  public:

    /** Constructor.  
     *
     * @param a - The address for the memory request.
     * @param m - The type of memory operation.
     */
    MemRequest(uint32_t a, MemOperation m) {
      latency = 0;
      addr = a;
      memOp = m;
      isPendingInWriteBuffer = false;
      writeBufferPendingCycles = 0;
    }

    /** Returns the type of memory operation */
    MemOperation getMemOperation() const { return memOp; }
    std::string getMemOperationString() const {
      switch(memOp){
        case MemRead: return "MemRead";
        case MemWrite: return "MemWrite";
        case MemWriteBack: return "MemWriteBack";
        default: assert(0); return "";
      }
    }

    /** Returns the latency incurred so far by the memory request */
    uint32_t getLatency() const { return latency; }
    /** Adds to the latency incurred by the memory request */
    void addLatency(uint32_t lat) {
      if(!isPendingInWriteBuffer) {
        // Add to request latency only if this is not a buffered write.
        latency += lat;
      } else {
        // Otherwise, latency goes to cycles pending in the write buffer.
        writeBufferPendingCycles += lat;
      }
    }
    /** Returns the number of cycles request should be pending in write buffer */
    uint32_t getPendingCycles() const { return writeBufferPendingCycles; }

    /** Returns the address for the memory request */
    uint32_t getAddr() const { return addr; }
    /** Sets the address for the memory request */
    void  setAddr(uint32_t a) { addr = a; }

    /** A write miss for WBCache needs to be converted to a read when
     * accessing lower levels to fetch data for the alloated line.
     */
    void mutateWriteToRead() {
      assert(memOp == MemWrite);
      memOp = MemRead;
    }
    /** The write request has been stored as pending in a write buffer, so
     * no additional latency should be incurred going forward.
     */
    void mutateToBufferedWrite() {
      assert(memOp == MemWrite);
      isPendingInWriteBuffer = true;
    }

};

#endif   // MEMREQUEST_H
