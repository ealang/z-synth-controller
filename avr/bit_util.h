#ifndef BIT_UTIL_H
#define BIT_UTIL_H

#define BIT_CLEAR(VAR, PIN) (VAR &= ~(1 << PIN))
#define BIT_SET(VAR, PIN) (VAR |= (1 << PIN))
#define BIT_WRITE(VAR, PIN, VALUE) (VAR = (VAR & ~(1 << PIN)) | ((VALUE & 1) << PIN))
#define MASK_CLEAR(VAR, MASK) (VAR &= ~MASK)
#define MASK_SET(VAR, MASK) (VAR |= MASK)

#define MASK(bit) (1 << bit)

#endif
