[На русском 🇷🇺](README_RUS.md)

# Decimal Library

Implementation of a decimal library in C that provides operations for working with custom decimal type numbers.

## Features

### Decimal Operations
- Arithmetic operations (addition, subtraction, multiplication, division)
- Comparison operations
- Type conversions (from/to int, float)
- Rounding operations (floor, round, truncate)
- Negate operation

## Implementation Details

### Decimal Structure

The decimal type is implemented as a structure containing a four-element array of 32-bit integers:
```c
typedef struct {
    int bits[4];
} s21_decimal;
```

### Binary Representation

- bits[0-2]: Store the 96-bit mantissa (low, middle, and high bits)
- bits[3]: Contains service information
  - Bits 0-15: Unused (must be zero)
  - Bits 16-23: Scale factor (power of 10)
  - Bits 24-30: Unused (must be zero)
  - Bit 31: Sign (0 for positive, 1 for negative)

### Value Range

Supports numbers from:
- Minimum: -79,228,162,514,264,337,593,543,950,335
- Maximum: +79,228,162,514,264,337,593,543,950,335

With decimal places controlled by the scale factor (up to 28 decimal places).

## Build and usage

### Building the Library

```bash
make s21_decimal.a
```

### Using

1. Include the header file:
```c
#include "s21_decimal.h"
```

2. Compile your project with the library:
```bash
gcc your_file.c s21_decimal.a -o your_program
```

## Testing

The project includes unit tests written using the check library.

```bash
make test
```

For test coverage report:
```bash
make gcov_report
```

## License
This project is licensed under the Apache License, Version 2.0. See LICENSE file for details.
