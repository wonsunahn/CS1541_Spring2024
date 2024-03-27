#ifndef WRITEBUFFER_H
#define WRITEBUFFER_H

#include <map>
#include <glib.h>
#include "CPU.h"
#include "config.h"

/** @brief A write buffer.
 *
 * This write buffer is fully associative and infinitely sized.
 */
class WriteBuffer: public MemObj
{
  protected:
    typedef struct {
            unsigned int pendingUntil;	// cycle number until which this entry is pending in the write buffer
    } Entry;

    /** Fully associative memory for this write buffer */
    std::map<unsigned int, Entry> buffer;
    /** Size of the write buffer */
    uint32_t size;
    /** The hit time in clock cycles */
    uint32_t hitDelay;

    // BEGIN Statistics
    Counter readHits;
    Counter readMisses;
    Counter writeHits;
    Counter writeMisses;
    Counter writeOverflows;
    // END Statistics

  public:
    /** Constructor.  First invokes the parent MemObj constructor then
     * initializes hitDelay from the config file.
     *
     * @param name - The name of the cache object on the config file.
     */
    WriteBuffer(const char *name)
      : MemObj(name)
        ,readHits("readHits")
        ,readMisses("readMisses")
        ,writeHits("writeHits")
        ,writeMisses("writeMisses")
        ,writeOverflows("writeOverflows")
    {
      GError *error = NULL;
      size = g_key_file_get_integer(config->keyfile, name, "size", NULL);
      if(error != NULL) g_error (error->message);
      hitDelay = g_key_file_get_integer(config->keyfile, name, "hitDelay", NULL);
      if(error != NULL) g_error (error->message);
    }

    ~WriteBuffer()
    {
    }

    /** Accesses memory object with memory request.  Adds the hitDelay to
     * the memory request latency and updates statistics depending on read, write, or writeBack.
     *
     * @param mreq - The memory request
     */
    virtual void access(MemRequest *mreq) {
      // Remove all pending stores in write buffer that would have expired by now
      for (auto it = buffer.cbegin(); it != buffer.cend(); ) {
        if (it->second.pendingUntil < cycle_number) {
          buffer.erase(it++);
        }
        else {
          ++it;
        }
      }

      switch(mreq->getMemOperation()){
        case MemRead:
          if (buffer.find(mreq->getAddr()) != buffer.end()) {
            readHits.inc();
            // The only latency added is the access delay of write buffer.
            mreq->addLatency(hitDelay);
            if(verbose) printf("%s->access(%s, addr: 0x%08x, latency: %u)\n", getName().c_str(), mreq->getMemOperationString().c_str(), mreq->getAddr(), mreq->getLatency());
          } else {
            readMisses.inc();
            // Entry does not exist so search lower level.
            // No delay added since write buffer access is concurrent with lower level access.
            getLowerLevelMemObj()->access(mreq);
          }
          break;
        case MemWrite:
          if (buffer.find(mreq->getAddr()) != buffer.end()) {
            writeHits.inc();
            // The only latency added is the access delay of write buffer.
            mreq->addLatency(hitDelay);
            if(verbose) printf("%s->access(%s, addr: 0x%08x, latency: %u)\n", getName().c_str(), mreq->getMemOperationString().c_str(), mreq->getAddr(), mreq->getLatency());
            // Send request to lower levels after marking as buffered write.
            mreq->mutateToBufferedWrite();
            getLowerLevelMemObj()->access(mreq);
            // Update the expiration time of the pending store.
            buffer[mreq->getAddr()].pendingUntil = cycle_number + mreq->getLatency() + mreq->getPendingCycles() - 1;
          } else {
            writeMisses.inc();
            if (buffer.size() < size) {
              // The only latency added is the access delay of write buffer.
              mreq->addLatency(hitDelay);
              if(verbose) printf("%s->access(%s, addr: 0x%08x, latency: %u)\n", getName().c_str(), mreq->getMemOperationString().c_str(), mreq->getAddr(), mreq->getLatency());
              // Send request to lower levels after marking as buffered write.
              mreq->mutateToBufferedWrite();
              getLowerLevelMemObj()->access(mreq);
              // Populate the write buffer with pending store.
              buffer[mreq->getAddr()].pendingUntil = cycle_number + mreq->getLatency() + mreq->getPendingCycles() - 1;
            } else {
              writeOverflows.inc();
              // Write buffer is full so send to lower levels.
              getLowerLevelMemObj()->access(mreq);
            }
          }
          break;
        case MemWriteBack:
          assert(0);
          break;
        default:
          assert(0);
          break;
      }
    }

    /** Returns a string that describes the WriteBuffer */
    virtual std::string toString() const {
      std::string ret;
      ret += "[" + getName() + "]\n";
      ret += "device type = dram\n";
      ret += "hit time = " + std::to_string(hitDelay) + "\n";
      return ret;
    }

    /** Returns a string that summarizes access statistics */
    std::string getStatString() const {
      std::string ret;
      ret += getName() + ":";
      ret += readHits.toString() + ":";
      ret += readMisses.toString() + ":";
      ret += writeHits.toString() + ":";
      ret += writeMisses.toString() + ":";
      ret += writeOverflows.toString();
      return ret;
    }

    /** Returns an empty string.  Meaningful only for cache objects. */
    std::string getContentString() const {
      std::string ret;
      ret += "[" + getName() + "]\n";
      for (auto it = buffer.cbegin(); it != buffer.cend(); it++) {
        std::stringstream tagInHex;
        tagInHex << "0x" << std::hex << it->first;
        ret += "tag=" + tagInHex.str() + ":pendingUntil:" + std::to_string(it->second.pendingUntil) + "\n";
      }
      return ret;
    }
};

#endif
