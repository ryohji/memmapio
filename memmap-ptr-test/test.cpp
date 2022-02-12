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

#include <gtest/gtest.h>


namespace {
	auto stream = std::ostringstream();

	class MemmapPtr : public testing::Test {
		// Sets up the test fixture.
		virtual void SetUp() { stream.str(""); }

		// Tears down the test fixture.
		virtual void TearDown() {}
	};
}

reoz::io::memmap* reoz::io::detail::make_memmap(HANDLE, uint64_t, bool) {
	stream << __FUNCTION__ << std::endl;
	return nullptr;
}

void reoz::io::detail::free_memmap(reoz::io::memmap*) {
	stream << __FUNCTION__ << std::endl;
}

uint32_t reoz::io::detail::get_page_count(reoz::io::memmap*) {
	stream << __FUNCTION__ << std::endl;
	return 0;
}

uint32_t reoz::io::detail::get_page_size(reoz::io::memmap*) {
	stream << __FUNCTION__ << std::endl;
	return 0;
}

void* reoz::io::detail::get_address(reoz::io::memmap*, uint32_t, uint32_t) {
	stream << __FUNCTION__ << std::endl;
	return nullptr;
}

void reoz::io::detail::free_address(void*) {
	stream << __FUNCTION__ << std::endl;
}

TEST_F(MemmapPtr, simple_life) {
	{
		auto mmap = reoz::io::make_memmap(0, 0, false);
		EXPECT_EQ(
			"reoz::io::detail::make_memmap\n", stream.str());
	}
	EXPECT_EQ(
		"reoz::io::detail::make_memmap\n"
		"reoz::io::detail::free_memmap\n", stream.str());
}

TEST_F(MemmapPtr, page_count_and_size) {
	auto mmap = reoz::io::make_memmap(0, 0, false);
	reoz::io::get_page_count(mmap);
	reoz::io::get_page_size(mmap);
	EXPECT_EQ(
		"reoz::io::detail::make_memmap\n"
		"reoz::io::detail::get_page_count\n"
		"reoz::io::detail::get_page_size\n", stream.str());
}

TEST_F(MemmapPtr, get_address) {
	{
		auto mmap = reoz::io::make_memmap(0, 0, false);
		auto addr = reoz::io::get_address(mmap);
		EXPECT_EQ(
			"reoz::io::detail::make_memmap\n"
			"reoz::io::detail::get_address\n", stream.str());
	}
	EXPECT_EQ(
		"reoz::io::detail::make_memmap\n"
		"reoz::io::detail::get_address\n"
		"reoz::io::detail::free_address\n"
		"reoz::io::detail::free_memmap\n", stream.str());
}

TEST_F(MemmapPtr, THIS_addr_CAN_BE_USED_SAFELY) {
	{
		auto addr = reoz::io::get_address(reoz::io::make_memmap(0, 0, false));
		EXPECT_EQ(
			"reoz::io::detail::make_memmap\n"
			"reoz::io::detail::get_address\n"
			"reoz::io::detail::free_memmap\n", stream.str());
		// addr object holds file mapping handle internally,
		// so you can safely use the pointer of `addr.get()` here.
	}
	EXPECT_EQ(
		"reoz::io::detail::make_memmap\n"
		"reoz::io::detail::get_address\n"
		"reoz::io::detail::free_memmap\n"
		"reoz::io::detail::free_address\n", stream.str());
}
