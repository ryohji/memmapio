# memmapio

Wrapper to enable you to use Win32 FileMapping API easily.  
You can use the file in a basis of memory mapped I/O.


## Example 1: map the file into the memory to read

```c++
#include "memmapio.h"
#include <Windows.h>

...

HANDLE hFile = ::CreateFile(TEXT("path/to/the file"), GENERIC_READ, 0, NULL,
	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
DWORD sizeHi, sizeLo = ::GetFileSize(hFile, &sizeHi);
size_t nSize = static_cast<size_t>(sizeHi) << 32 + sizeLo;

auto mmap = reoz::io::make_memmap(hFile, nSize, false); // read-only mapping
auto addr = reoz::io::get_address(mmap); // this addr points file content

void* begin = addr.get();
void* end = static_cast<void*>(static_cast<char*>(begin) + nSize);

// Here, you can access file content [begin, end).
```

Note that FileMapping resources are managed by the std::shared_ptr,
so you just only keep `mmap` and `addr` object in the scope you use.


## Example 2: make virtual address space for a temporary work

```c++
#include "memmapio.h"
#include <Windows.h>

...

// Make 1 kB size read/write temporary working space
auto mmap = reoz::io::make_memmap(INVALIDE_HANDLE_VALUE, 1024, true);
auto addr = reoz::io::get_address(mmap); // this addr points an anonymouse paging file

// You can use `addr.get()` to `addr.get() + 1024` bytes space.
::snprintf(addr.get(), 1024, "...", ...);
```

## Note

This is a Visual Studio 2019 solution/projects.

You can make a library or a linkable object from source files under the  `memmap` directory.

If you want to execute unit tests, please restore NuGet packages using "NuGet Packaging
Manager" under the "Tool" menu.
