/*
   pl_py_shared_rename.h
     * Pilot Light's single header libraries (pl_string.h, pl_memory.h,
       stb_sprintf.h) are implemented by every extension that uses them since
       extensions are normally separate shared libraries. The python module
       links several of those translation units into a single binary, so each
       secondary unit includes this header first to give its copy of those
       symbols a unique name (the unity extension unit keeps the real names).
     * define PL_PY_UNIT before including (defaults to "pl_py_host")
*/

#ifndef PL_PY_SHARED_RENAME_H
#define PL_PY_SHARED_RENAME_H

#ifndef PL_PY_UNIT
    #define PL_PY_UNIT pl_py_host
#endif

#define PL__PY_CAT2(a, b) a##_##b
#define PL__PY_CAT(a, b) PL__PY_CAT2(a, b)
#define PL__PY_RENAME(x) PL__PY_CAT(PL_PY_UNIT, x)

#define pl_str_hash_data                         PL__PY_RENAME(pl_str_hash_data)
#define pl_str_hash                              PL__PY_RENAME(pl_str_hash)
#define pl_str_get_file_extension                PL__PY_RENAME(pl_str_get_file_extension)
#define pl_str_get_file_name                     PL__PY_RENAME(pl_str_get_file_name)
#define pl_str_get_file_name_only                PL__PY_RENAME(pl_str_get_file_name_only)
#define pl_str_get_directory                     PL__PY_RENAME(pl_str_get_directory)
#define pl_str_concatenate                       PL__PY_RENAME(pl_str_concatenate)
#define pl_str_equal                             PL__PY_RENAME(pl_str_equal)
#define pl_str_contains                          PL__PY_RENAME(pl_str_contains)
#define pl_text_char_from_utf8                   PL__PY_RENAME(pl_text_char_from_utf8)
#define pl__temp_allocator_sprintf_va            PL__PY_RENAME(pl__temp_allocator_sprintf_va)
#define pl_aligned_alloc                         PL__PY_RENAME(pl_aligned_alloc)
#define pl_aligned_free                          PL__PY_RENAME(pl_aligned_free)
#define pl_general_allocator_aligned_alloc       PL__PY_RENAME(pl_general_allocator_aligned_alloc)
#define pl_general_allocator_aligned_free        PL__PY_RENAME(pl_general_allocator_aligned_free)
#define pl_general_allocator_alloc               PL__PY_RENAME(pl_general_allocator_alloc)
#define pl_general_allocator_free                PL__PY_RENAME(pl_general_allocator_free)
#define pl_general_allocator_init                PL__PY_RENAME(pl_general_allocator_init)
#define pl_general_allocator_realloc             PL__PY_RENAME(pl_general_allocator_realloc)
#define pl_pool_allocator_alloc                  PL__PY_RENAME(pl_pool_allocator_alloc)
#define pl_pool_allocator_free                   PL__PY_RENAME(pl_pool_allocator_free)
#define pl_pool_allocator_init                   PL__PY_RENAME(pl_pool_allocator_init)
#define pl_stack_allocator_aligned_alloc         PL__PY_RENAME(pl_stack_allocator_aligned_alloc)
#define pl_stack_allocator_aligned_alloc_bottom  PL__PY_RENAME(pl_stack_allocator_aligned_alloc_bottom)
#define pl_stack_allocator_aligned_alloc_top     PL__PY_RENAME(pl_stack_allocator_aligned_alloc_top)
#define pl_stack_allocator_alloc                 PL__PY_RENAME(pl_stack_allocator_alloc)
#define pl_stack_allocator_alloc_bottom          PL__PY_RENAME(pl_stack_allocator_alloc_bottom)
#define pl_stack_allocator_alloc_top             PL__PY_RENAME(pl_stack_allocator_alloc_top)
#define pl_stack_allocator_bottom_marker         PL__PY_RENAME(pl_stack_allocator_bottom_marker)
#define pl_stack_allocator_free_bottom_to_marker PL__PY_RENAME(pl_stack_allocator_free_bottom_to_marker)
#define pl_stack_allocator_free_to_marker        PL__PY_RENAME(pl_stack_allocator_free_to_marker)
#define pl_stack_allocator_free_top_to_marker    PL__PY_RENAME(pl_stack_allocator_free_top_to_marker)
#define pl_stack_allocator_init                  PL__PY_RENAME(pl_stack_allocator_init)
#define pl_stack_allocator_marker                PL__PY_RENAME(pl_stack_allocator_marker)
#define pl_stack_allocator_reset                 PL__PY_RENAME(pl_stack_allocator_reset)
#define pl_stack_allocator_top_marker            PL__PY_RENAME(pl_stack_allocator_top_marker)
#define pl_temp_allocator_alloc                  PL__PY_RENAME(pl_temp_allocator_alloc)
#define pl_temp_allocator_free                   PL__PY_RENAME(pl_temp_allocator_free)
#define pl_temp_allocator_reset                  PL__PY_RENAME(pl_temp_allocator_reset)
#define pl_temp_allocator_sprintf                PL__PY_RENAME(pl_temp_allocator_sprintf)
#define stbsp_vsprintf                           PL__PY_RENAME(stbsp_vsprintf)
#define stbsp_vsnprintf                          PL__PY_RENAME(stbsp_vsnprintf)
#define stbsp_sprintf                            PL__PY_RENAME(stbsp_sprintf)
#define stbsp_snprintf                           PL__PY_RENAME(stbsp_snprintf)
#define stbsp_vsprintfcb                         PL__PY_RENAME(stbsp_vsprintfcb)
#define stbsp_set_separators                     PL__PY_RENAME(stbsp_set_separators)

#endif // PL_PY_SHARED_RENAME_H
