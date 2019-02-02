# C++ Fortran Format

This is an under development module that provides a `printfor` function similar to Fortran's `print`
function, with a format specification string:

```cpp
printfor("(I3, SP, 2(1X, F5.2), 2X, SS, G9.3E3)", 10, 3.1416, 3.333, 2.7545E-12);
//output: 
// 10 +3.14 +3.33  .275E-011
```

The goal for this project is to offer C++ programmers a way to get the same output results as they would get by using FORMAT within a Fortran program. 

In this version, `Iw.m`, `Fw.d`, `Dw.d`, `Ew.d`, `Ew.dEe`, `Gw.d`, `Gw.dEe`, `Lw`, `A[w]`, `SS`, `SP`, `nX`, `/`, `nH`, and `''` [edit descriptors](http://www.fortran.com/fortran/F77_std/rjcnf0001-sh-13.html#sh-13.5.6) are supported. Both grouping `()` and repeat specification are supported also. See [Supported Features](#supported-features) and [Known Issues](#known-issues) for limitations. 
Currently, the output goes directly to the output stream (stdout) or to an user specified output stream (with `std::ostream` base class). 


This work is inspired by [py-fortranformat](https://bitbucket.org/brendanarnold/py-fortranformat/wiki/Home). Also, [Acutest](https://github.com/mity/acutest) is used as unit testing facility.


## Usage

Something like this:

```f90
program formt
implicit none
    print "(A, 2(I3, I3))", "test: ", 1, 2, 3, 4
end program formt
```

Becomes this:

```cpp
#include "fortranformat.hpp"

void main()
{
    printfor("(A, 2(I3, I3))", "test: ", 1, 2, 3, 4);
}
```

Output:

```
test:   1  2  3  4
```

The `printfor` is also overloaded to work with any output stream based on 
`std::ostream`:

```cpp
printfor(std::ostream& stream, char const* format, ...);
```

## Supported Features

Some descriptors are fully supported (`Iw.m`), while others are not (such as `S`, `TRc`, `kP`). This means that the formatting in some cases may not reflect expected Fortran's. The following tables
summarize the availability of each edit descriptor.

Some descriptors may be specified as "Recognized", where they are read from the format string but its 
output is basic and don't reflect expected Fortran's.


### Repeatable edit descriptors

| Edit Descriptor | Available? |
|:----------------|:----------:|
| Iw              |    Yes     |
| Iw.m            |    Yes     |
| Fw.d            |    Yes     |
| Ew.d            |    Yes     |
| Ew.dEe          |    Yes     |
| Dw.d            |    Yes     |
| Gw.d            |    Yes     |
| Gw.dEe          |    Yes     |
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
| /                    |    Yes     | Both       |
| :                    |    No      | Both       |
| S                    |    No      | Output     |
| SP                   |    Yes     | Output     |
| SS                   |    Yes     | Output     |
| kP                   |    No      | Both       |
| BN                   |    No      | Input      |
| BZ                   |    No      | Input      |


### Known Issues

- There is no check against malformed format string
- Does not support escaping quotes by repeating it, such as `''` or `""`, within `nH`.
- Does not escape single quotation marks `'` within double quotation marks strings and vice-versa.
- No error is thrown when `w=0`.
- No default values for omitted `w` and `d`.


## Documentation

https://github.com/dparrini/cpp-fortranformat

## Support

Feel free to report any bugs you find. You are welcome to fork and submit pull requests.

## License

The module is available at [GitHub](https://github.com/dparrini/cpp-fortranformat) under the MIT license.