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

typedef struct _pyplVfsI
{
    PyObject_HEAD
} pyplVfsI;

PyObject*
mount_directory(PyObject* self, PyObject* args, PyObject* kwargs)
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

static PyMethodDef gatplVfsICommands[] =
{
    PL_PYTHON_METHOD(mount_directory, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL),
    {NULL, NULL, 0, NULL}
};

static PyType_Slot gatplVfsISlots[] = {
    // {Py_tp_init, (void*)pl_io_init},
    {Py_tp_methods, (void*)gatplVfsICommands},
    {0, 0}
};

static PyType_Spec plVfsISpec = {
    "pilotlight.plVfsI",
    sizeof(pyplVfsI),
    0,
    Py_TPFLAGS_DEFAULT,
    gatplVfsISlots
};