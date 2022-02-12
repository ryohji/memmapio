/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org>
*/

#include "memmapio.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace {
	inline uint32_t hi(uint64_t v) { return static_cast<uint32_t>(v >> 32); }

	inline uint32_t lo(uint64_t v) { return static_cast<uint32_t>(v); }

	inline uint32_t protection(bool writable) { return writable ? PAGE_READWRITE : PAGE_READONLY; }
}

static const uint32_t page_size = [] {
	SYSTEM_INFO ifo;
	::GetSystemInfo(&ifo);
	return ifo.dwAllocationGranularity;
}();

struct reoz::io::memmap {
	const HANDLE handle;
	const uint32_t access;
	const uint64_t size;

	memmap(HANDLE file, uint64_t size, bool writable)
		: handle(::CreateFileMappingW(file, nullptr, protection(writable), hi(size), lo(size), nullptr))
		, access(writable ? FILE_MAP_WRITE : FILE_MAP_READ)
		, size(size)
	{}

	~memmap() {
		::CloseHandle(handle);
	}
};

reoz::io::memmap* reoz::io::detail::make_memmap(HANDLE file, uint64_t size, bool writable) {
	return new memmap(file, size, writable);
}

void reoz::io::detail::free_memmap(memmap* memmap) {
	delete memmap;
}

void* reoz::io::detail::get_address(memmap* memmap, uint32_t size, uint32_t page_number) {
	uint64_t offset = static_cast<uint64_t>(page_number) * page_size;
	return ::MapViewOfFile(memmap->handle, memmap->access, hi(offset), lo(offset), size);
}

void reoz::io::detail::free_address(void* p) {
	::UnmapViewOfFile(p);
}

uint32_t reoz::io::detail::get_page_count(memmap* memmap) {
	return static_cast<uint32_t>((memmap->size + page_size - 1) / page_size);
}

uint32_t reoz::io::detail::get_page_size(memmap* memmap) {
	return page_size;
}
