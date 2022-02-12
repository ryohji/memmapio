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

#pragma once

#include <memory>

typedef void* HANDLE;

namespace reoz {
	namespace io {

		struct memmap;
		typedef std::shared_ptr<memmap> memmap_ptr;

		typedef std::shared_ptr<void> address_ptr;

		namespace detail {
			memmap* make_memmap(HANDLE, uint64_t, bool);
			void free_memmap(memmap*);
			uint32_t get_page_count(memmap*);
			uint32_t get_page_size(memmap*);
			void* get_address(memmap*, uint32_t, uint32_t);
			void free_address(void*);
		}

		/* Make memmap object, to map file into process memory address space.
		*
		* Args:
		*   file: HANDLE to the file. To create virtual memory space, specify INVALID_HANDLE_VALUE.
		*   size: the size of file mapped into the memory in bytes. should not be zero.
		*   writable: true if mapping need to be able to writable, or false to be read-only.
		*/
		inline memmap_ptr make_memmap(HANDLE file, uint64_t size, bool writable) {
			return memmap_ptr(detail::make_memmap(file, size, writable), detail::free_memmap);
		}

		/* Get mapped address of shared memory.
		* 
		* Args:
		*   memmap: mapping object shared_ptr.
		*/
		inline address_ptr get_address(memmap_ptr memmap) {
			return address_ptr(detail::get_address(memmap.get(), 0, 0), detail::free_address);
		}

		/* Get mapped address of shared memory.
		* 
		* Args:
		*   memmap: mapping object shared_ptr.
		*   size: size to map. should not exceeds the size on making a call of `make_memmap`. 0, to map end of the mapping.
		*   page_number: starting offset to map by page number.
		*/
		inline address_ptr get_address(memmap_ptr memmap, uint32_t size, uint32_t page_number) {
			return address_ptr(detail::get_address(memmap.get(), size, page_number), detail::free_address);
		}

		/* Get page count of memmap. */
		inline uint32_t get_page_count(memmap_ptr memmap) {
			return detail::get_page_count(memmap.get());
		}

		/* Get page size in bytes. */
		inline uint32_t get_page_size(memmap_ptr memmap) {
			return detail::get_page_size(memmap.get());
		}

	} // namespace io
} //namespace reoz
