/*
MIT License

Copyright (c) 2018 Brian T. Park

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ACE_COMMON_FCSTRING_H
#define ACE_COMMON_FCSTRING_H

// Much of this copied from AUnit/src/aunit/FCString.h and Compare.h.

#include <stddef.h> // size_t
#include <stdint.h> // uint8_t

class Print;
class __FlashStringHelper;

namespace ace_common {

/**
 * A union of (const char*) and (const __FlashStringHelper*) with a
 * discriminator. This allows us to treat these 2 strings like a single object.
 *
 * I deliberately decided not to inherit from Printable. While it is convenient
 * to be able to call Print::print() with an instance of this class, the cost
 * is 2 (AVR) or 4 (Teensy-ARM or ESP8266) extra bytes of static memory for the
 * v-table pointer for each instance. Instead, use the `printTo(Print&)`
 * method.
 */
class FCString {
  public:
    /** Identifies the object as holding a c-string. getCString() is valid. */
    static const uint8_t kCStringType = 0;

    /** Identifies the object as holding an f-string. getFString() is valid. */
    static const uint8_t kFStringType = 1;

    /** Default constructor initializes to a nullptr of kCStringType. */
    FCString() {}

    /** Construct with a c-string. */
    explicit FCString(const char* s):
        mStringType(kCStringType) {
      mString.cstring = s;
    }

    /** Construct with a flash string. */
    explicit FCString(const __FlashStringHelper* s):
        mStringType(kFStringType) {
      mString.fstring = s;
    }

    /** Return if this is a null string. */
    bool isNull() const { return mString.cstring == nullptr; }

    /** Get the internal type of string. */
    uint8_t getType() const { return mStringType; }

    /** Get the c-string pointer. */
    const char* getCString() const { return mString.cstring; }

    /** Get the flash string pointer. */
    const __FlashStringHelper* getFString() const { return mString.fstring; }

    /**
     * Convenience method for printing an FCString to printer. This is
     * identical to the method in Printable but we don't derive from Printable
     * because we don't want to want to pay the memory cost of the vtable
     * pointer for such a small class.
     */
    size_t printTo(Print& printer) const;

    /** Compare to another FCString. */
    int compareTo(const FCString& that) const;

  private:
    // NOTE: It might be possible just use a (void *) instead of a union.
    union {
      const char* cstring;
      const __FlashStringHelper* fstring;
    } mString = { nullptr };

    uint8_t mStringType = kCStringType;
};

}

#endif
