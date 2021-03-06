/*******************************************************************************
 * Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
 *   
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#ifndef BUFFER_H_
#define BUFFER_H_

#include <io/humble/ferry/Ferry.h>
#include <io/humble/ferry/RefCounted.h>

namespace io { namespace humble { namespace ferry
{

/**
 * Allows Java code to get data from a native buffers, and optionally modify native memory directly.
 * <p> 
 * When accessing from Java, you can copy in and
 * out ranges of buffers.  You can do this by-copy
 * (which is safer but a little slower) or by-reference
 * where you <b>directly access underlying C++/native
 * memory from Java</b>.  Take special care if you decide
 * that native access is required.
 * </p>
 * <p>
 * When accessing from C++, you get direct access to
 * the underlying buffer.
 * </p>
 * <p>
 * To make an Buffer object that is not a Humble internal object,
 * pass in null for the RefCounted parameter.
 *
 * </p>
 */
class VS_API_FERRY Buffer : public io::humble::ferry::RefCounted
{
public:
#ifndef SWIG
  typedef void
  (*FreeFunc)(void * mem, void *closure);
  /**
   * A version of FreeFunc that assumes that mem is handled by a RefCounted
   * closure.
   */
  static void refCountedFreeFunc(void* mem, void* closure) {
    RefCounted* ref = static_cast<RefCounted*>(closure);
    (void) mem;
    VS_REF_RELEASE(ref);
  }

  /**
   * Returns up to length bytes, starting at offset in the
   * underlying buffer we're managing.
   * 
   * Note that with this method you are accessing the direct
   * memory, so be careful.
   * 
   * @param offset The offset (in bytes) into the buffer managed by this Buffer
   * @param length The requested length (in bytes) you want to access.  The buffer returned may
   *   actually be longer than length.
   * 
   * @return A pointer to the direct buffer, or 0 if the offset or length
   *   would not fit within the buffer we're managing.
   */
  virtual void*
  getBytes(int32_t offset, int32_t length)=0;

  /**
   * Allocate a new buffer by wrapping a native buffer.
   * To make an Buffer object that is not a Humble internal object,
   * pass in null for the <code>requestor</code> parameter.
   * 
   * @param requestor An optional value telling the Buffer class what object requested it.  This is used for debugging memory leaks; it's a marker for the FERRY object (e.g. IPacket) that actually requested the buffer.  If you're not an FERRY object, pass in null here.
   * @param bufToWrap Buffer to wrap
   * @param bufferSize The minimum buffer size you're requesting in bytes; a buffer with a larger size may be returned.
   * @param freeFunc A function that will be called when we decide to free the buffer
   * @param closure A value that will be passed, along with this, to freeFunc
   * 
   * @return A new buffer, or null on error.
   */
  static Buffer*
  make(RefCounted* requestor, void * bufToWrap, int32_t bufferSize,
      FreeFunc freeFunc, void * closure);

#endif // ! SWIG
  /**
   * Get the current maximum number of bytes that can
   * be safely placed in this buffer.
   * 
   * @return Maximum number of bytes this buffer can manage.
   */
  virtual int32_t
  getBufferSize()=0;

  /**
   * Allocate a new buffer of at least bufferSize.
   * 
   * @param requestor An optional value telling the Buffer class what object requested it.  This is used for debugging memory leaks; it's a marker for the FERRY object (e.g. IPacket) that actually requested the buffer.  If you're not an FERRY object, pass in null here.
   * @param bufferSize The minimum buffer size you're requesting in bytes; a buffer with a larger size may be returned.
   * 
   * @return A new buffer, or null on error.
   */
  static Buffer*
  make(io::humble::ferry::RefCounted* requestor, int32_t bufferSize);

  /**
   * Types of data that are in this buffer.
   */
  typedef enum {
    BUFFER_UINT8,
    BUFFER_SINT8,
    BUFFER_UINT16,
    BUFFER_SINT16,
    BUFFER_UINT32,
    BUFFER_SINT32,
    BUFFER_UINT64,
    BUFFER_SINT64,
    BUFFER_FLT32,
    BUFFER_DBL64,
    BUFFER_NB,
  } Type;
  
  /**
   * Get the type this buffer was created as.
   * <p>
   * A type is really just a hint.  Like
   * java.nio.ByteBuffer objects,
   * Buffer objects can be cast to and from any type.
   * </p>
   * @return the type  
   */
  virtual Type getType()=0;
  
  /**
   * Reset the buffer type to a new type.
   * <p>
   * This method does not do any data conversion, it
   * just changes the reported type (so changing from
   * Type#BUFFER_UINT8 to Type#BUFFER_SINT16
   * is really just a "cast" operation).
   * </p>
   * @param type the type to set to. 
   */
  virtual void setType(Type type)=0;
  
  /**
   * Returns the size, in bytes, of elements of given Type.
   * 
   * @return the size in bytes.
   */
  static int32_t getTypeSize(Type type);
  
  /**
   * Returns the size, in units of #getType() of
   * this buffer.
   * 
   * @return number of items of type #getType() that
   *   will fit in this buffer.
   */
  virtual int32_t getSize()=0;
  
  /**
   * Allocate a new buffer of at least bufferSize.
   * 
   * @param requestor An optional value telling the Buffer class what object requested it.  This is used for debugging memory leaks; it's a marker for the FERRY object (e.g. IPacket) that actually requested the buffer.  If you're not an FERRY object, pass in null here.
   * @param type The type of buffer.
   * @param numElements The minimum number of elements of the specified
   *                    type you will put in this buffer.
   * @param zero If true, we will guarantee the buffer contains
   *             only zeros.  If false, we will not (it is 
   *             faster to not, but then the buffer will have
   *             garbage-data in it).
   * 
   * @return A new buffer, or null on error.
   */
  static Buffer*
  make(io::humble::ferry::RefCounted* requestor,
      Type type, int32_t numElements, bool zero);


protected:
  Buffer();
  virtual
  ~Buffer();
};

}
}
}

#endif /*BUFFER_H_*/
