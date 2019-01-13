# C++ Fortran Format

## Usage

Something like this:

```f90
program formt
implicit none
    print "(A, 2(I3, I3)", "test", 1, 2, 3, 4
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

Some descriptors are fully supported (`Iw`), while others are Partial. This means
that the formating in some cases may not reflect expected Fortran's. 

Descriptors specified as "Recognized" are read from the format string but its 
output is basic and don't reflect expected Fortran's. Such cases are `Dw.d` and
`Gw.d`, which output were made equal to `Fw.d` (which is incorrect).

### Repeatable edit descriptors


| Edit Descriptor | Available? |
|:----------------|:----------:|
| Iw              |    Yes     |
| Iw.m            |    No      |
| Fw.d            |    Partial |
| Ew.d            | Recognized |
| Ew.dEe          |    No      |
| Dw.d            | Recognized |
| Gw.d            | Recognized |
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

- Does not handle negative numbers as expected.

### `'string'` and `nH`

- Doesn't support escaping `''`.