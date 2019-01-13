# C++ Fortran Format

## Usage

Something like this:

```f90
program formt
implicit none
    print "(A, 2(I3, I3)", 1, 2, 3, 4
end program formt
```

Becomes this:

```cpp
#include "fortranformat.hpp"

void main()
{
    writer("(A, 2(I3, I3)", "test: ", 1, 2, 3, 4);
}
```

Output:

```
test:   1  2  3  4
```

## Supported Features


### Repeatable edit descriptors


| Edit Descriptor | Available? |
|:----------------|:----------:|
| Iw              |    Yes     |
| Iw.m            |    No      |
| Fw.d            |    Yes     |
| Ew.d            |    No      |
| Ew.dEe          |    No      |
| Dw.d            |    No      |
| Gw.d            |    No      |
| Gw.dEe          |    No      |
| Lw              |    Yes     |
| A               |    Yes     |
| Aw              |    Yes     |


### Nonrepeatable edit descriptors


| Edit Descriptor      | Available? |
|:---------------------|:----------:|
| 'h h ... h ' 1 2 n   |    Partial |
| nHh h ... h 1 2 n    |    Partial |
| Tc                   |    No      |
| TLc                  |    No      |
| TRc                  |    No      |
| nX                   |    Yes     |
| /                    |    No      |
| :                    |    No      |
| S                    |    No      |
| SP                   |    No      |
| SS                   |    No      |
| kP                   |    No      |
| BN                   |    No      |
| BZ                   |    No      |


## Issues

### `Fw.d`

- If `d=0`, the actual implementation prints the number without the dot, while
the Fortran implementation prints with the number with the dot but no decimal
numbers.

### `'string'` and `nH`

- Doesn't support escaping `''`.