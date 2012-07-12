/*
 * Copyright (c) 2004-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "mem_align.h"
#include "Tconfig.h"

void* aligned_malloc(size_t size, size_t alignment)
{
    return _aligned_malloc(size, alignment ? alignment : Tconfig::cache_line);
}

void* aligned_realloc(void *ptr, size_t size, size_t alignment)
{
    if (!ptr) {
        return aligned_malloc(size, alignment);
    } else if (size == 0) {
        aligned_free(ptr);
        return NULL;
    } else {
        return _aligned_realloc(ptr, size, alignment ? alignment : Tconfig::cache_line);
    }
}

void aligned_free(void *mem_ptr)
{
    if (mem_ptr) {
        _aligned_free(mem_ptr);
    }
}

void* aligned_calloc(size_t size1, size_t size2, size_t alignment)
{
    size_t size = size1 * size2;
    void *ret = aligned_malloc(size, alignment);
    if (ret) {
        memset(ret, 0, size);
    }
    return ret;
}

template <class T> T* aligned_calloc3(size_t width, size_t height, size_t pading, size_t alignment)
{
    size_t size = (width * height + pading) * sizeof(T);
    T *ret = (T*)_aligned_malloc(size, alignment);
    if (ret) {
        memset(ret, 0, size);
    }
    return ret;
}

template uint8_t* aligned_calloc3<uint8_t>(size_t width, size_t height, size_t pading, size_t alignment);
template uint16_t* aligned_calloc3<uint16_t>(size_t width, size_t height, size_t pading, size_t alignment);
