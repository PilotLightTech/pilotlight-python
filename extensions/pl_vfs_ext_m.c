/*
   pl_vfs_ext_m.c
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] implementations
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] implementations
//-----------------------------------------------------------------------------

PyObject*
vfs_mount_directory(PyObject* self, PyObject* args, PyObject* kwargs)
{

    static const char* apcKeywords[] = {
        "directory",
        "physical_directory",
        NULL,
    };

    const char* pcDirectory = NULL;
    const char* pcPhysicalDirectory = NULL;

	if (!pl_parse("ss|", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcDirectory, &pcPhysicalDirectory))
		return NULL;

    gptVfs->mount_directory(pcDirectory, pcPhysicalDirectory, PL_VFS_MOUNT_FLAGS_NONE);
    Py_RETURN_NONE;
}
