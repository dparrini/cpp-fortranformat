# C++ Fortran Format

This is an under development module that provides a `printfor` function similar to Fortran's `print`
function, with a format specification string:

```cpp
printfor("(I3, 2(1X, F4.2))", 10, 3.1416, 3.333);
// output: 
//  10 3.14 3.33
```

In this version, only `Iw`, `Fw.d`, `Lw`, `A[w]`, `nX`, `nH`, and `''`
[edit descriptors](http://www.fortran.com/fortran/F77_std/rjcnf0001-sh-13.html#sh-13.5.6) are supported. Both grouping `()` and repeat specification are supported also.
Currently, the output goes directly to the output stream (stdout). 

Future support for generic C++ streams and strings are planned after some improvements over available edit descriptors.

This work is inspired by [py-fortranformat](https://bitbucket.org/brendanarnold/py-fortranformat/wiki/Home).


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
    printfor("(A, 2(I3, I3)", "test: ", 1, 2, 3, 4);
}
```

Output:

```
test:   1  2  3  4
```

## Supported Features

Some descriptors are fully supported (`Iw`), while others are partially. This means
that the formatting in some cases may not reflect expected Fortran's. The following tables
summarize the availability of each edit descriptor.

Descriptors specified as "Recognized" are read from the format string but its 
output is basic and don't reflect expected Fortran's. Such cases are `Dw.d` and
`Gw.d`, which output were made equal to `Fw.d` (which is incorrect).

### Repeatable edit descriptors


| Edit Descriptor | Available? |
|:----------------|:----------:|
| Iw              |    Yes     |
| Iw.m            |    No      |
| Fw.d            |    Yes     |
| Ew.d            | Recognized |
| Ew.dEe          |    No      |
| Dw.d            | Recognized |
| Gw.d            | Recognized |
| Gw.dEe          |    No      |
| Lw              |    Yes     |
| A               |    Yes     |
| Aw              |    Yes     |


### Nonrepeatable edit descriptors


| Edit Descriptor      | Available? |  Afftects  |
|:---------------------|:----------:|:----------:|
| 'h h ... h ' 1 2 n   |    Partial | Output     |
| nHh h ... h 1 2 n    |    Partial | Output     |
| Tc                   |    No      | Both       |
| TLc                  |    No      | Both       |
| TRc                  |    No      | Both       |
| nX                   |    Yes     | Output     |
| /                    |    No      | Both       |
| :                    |    No      | Both       |
| S                    |    No      | Output     |
| SP                   |    No      | Output     |
| SS                   |    No      | Output     |
| kP                   |    No      | Both       |
| BN                   |    No      | Input      |
| BZ                   |    No      | Input      |


### Known Issues

#### `Fw.d`

- Does not handle `-0.0` as expected.

#### `'string'` and `nH`

- Doesn't support escaping `''`.

#### `w` zeroed or omitted

- No error is thrown when `w=0`.
- If omitted, no error is thrown.


## Documentation

https://github.com/dparrini/cpp-fortranformat

## Support

Feel free to report any bugs you find. You are welcome to fork and submit pull requests.

## License

The module is available at [GitHub](https://github.com/dparrini/cpp-fortranformat) under the MIT license.