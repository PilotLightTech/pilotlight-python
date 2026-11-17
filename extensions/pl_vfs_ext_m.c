/*
   pl_vfs_ext_m.c
     * plVfsI (virtual file system)
     * differences from the C api:
         - file handles are ints (plVfsFileHandle.uData)
         - read_file/read_file_stream return bytes; write_file/
           write_file_stream take any bytes-like object
         - mount_*, unmount & delete_file return bool (PL_VFS_RESULT_SUCCESS)
         - read_all/write_all (python additions) open, read/write & close a
           file in one call
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] helpers
// [SECTION] mounting
// [SECTION] files
// [SECTION] streams
// [SECTION] python additions
// [SECTION] method table
// [SECTION] constants
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] helpers
//-----------------------------------------------------------------------------

#define PL_VFS_KW METH_VARARGS | METH_KEYWORDS | METH_STATIC

// single "handle" argument
static bool
pl__vfs_parse_handle(PyObject* args, PyObject* kwargs, plVfsFileHandle* ptHandleOut)
{
    static const char* apcKeywords[] = {"handle", NULL};
    unsigned long long ulData = 0;
    if(!pl_parse_args("K", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ulData))
        return false;
    ptHandleOut->uData = ulData;
    return true;
}

// single "file" argument
static bool
pl__vfs_parse_file(PyObject* args, PyObject* kwargs, const char** ppcFileOut)
{
    static const char* apcKeywords[] = {"file", NULL};
    return pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, ppcFileOut);
}

// reads a whole (open) file into a bytes object; None if it can't be read
static PyObject*
pl__vfs_read_whole_file(plVfsFileHandle tHandle)
{
    size_t szSize = 0;
    if(gptVfs->read_file(tHandle, NULL, &szSize) != PL_VFS_RESULT_SUCCESS)
        Py_RETURN_NONE;
    PyObject* ptBytes = PyBytes_FromStringAndSize(NULL, (Py_ssize_t)szSize);
    if(ptBytes == NULL)
        return NULL;
    if(szSize > 0 && gptVfs->read_file(tHandle, PyBytes_AS_STRING(ptBytes), &szSize) != PL_VFS_RESULT_SUCCESS)
    {
        Py_DECREF(ptBytes);
        Py_RETURN_NONE;
    }
    return ptBytes;
}

//-----------------------------------------------------------------------------
// [SECTION] mounting
//-----------------------------------------------------------------------------

static PyObject*
vfs_mount_directory(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"directory", "physical_directory", "flags", NULL};
    const char* pcDirectory = NULL;
    const char* pcPhysicalDirectory = NULL;
    int iFlags = PL_VFS_MOUNT_FLAGS_NONE;
    if(!pl_parse_args("ss|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcDirectory, &pcPhysicalDirectory, &iFlags))
        return NULL;
    return PyBool_FromLong(gptVfs->mount_directory(pcDirectory, pcPhysicalDirectory, iFlags) == PL_VFS_RESULT_SUCCESS);
}

static PyObject*
vfs_mount_pak(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"directory", "pakFilePath", "flags", NULL};
    const char* pcDirectory = NULL;
    const char* pcPakFilePath = NULL;
    int iFlags = PL_VFS_MOUNT_FLAGS_NONE;
    if(!pl_parse_args("ss|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcDirectory, &pcPakFilePath, &iFlags))
        return NULL;
    return PyBool_FromLong(gptVfs->mount_pak(pcDirectory, pcPakFilePath, iFlags) == PL_VFS_RESULT_SUCCESS);
}

static PyObject*
vfs_mount_memory(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"directory", "flags", NULL};
    const char* pcDirectory = NULL;
    int iFlags = PL_VFS_MOUNT_FLAGS_NONE;
    if(!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcDirectory, &iFlags))
        return NULL;
    return PyBool_FromLong(gptVfs->mount_memory(pcDirectory, iFlags) == PL_VFS_RESULT_SUCCESS);
}

static PyObject*
vfs_unmount(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"directory", NULL};
    const char* pcDirectory = NULL;
    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcDirectory))
        return NULL;
    return PyBool_FromLong(gptVfs->unmount(pcDirectory) == PL_VFS_RESULT_SUCCESS);
}

//-----------------------------------------------------------------------------
// [SECTION] files
//-----------------------------------------------------------------------------

static PyObject*
vfs_does_file_exist(PyObject* self, PyObject* args, PyObject* kwargs)
{
    const char* pcFile = NULL;
    if(!pl__vfs_parse_file(args, kwargs, &pcFile))
        return NULL;
    return PyBool_FromLong(gptVfs->does_file_exist(pcFile));
}

static PyObject*
vfs_get_file_size_str(PyObject* self, PyObject* args, PyObject* kwargs)
{
    const char* pcFile = NULL;
    if(!pl__vfs_parse_file(args, kwargs, &pcFile))
        return NULL;
    return PyLong_FromSize_t(gptVfs->get_file_size_str(pcFile));
}

static PyObject*
vfs_register_file(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"file", "mustExist", NULL};
    const char* pcFile = NULL;
    int iMustExist = 0;
    if(!pl_parse_args("s|p", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcFile, &iMustExist))
        return NULL;
    return PyLong_FromUnsignedLongLong(gptVfs->register_file(pcFile, iMustExist != 0).uData);
}

static PyObject*
vfs_open_file(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"file", "mode", NULL};
    const char* pcFile = NULL;
    int iMode = PL_VFS_FILE_MODE_READ;
    if(!pl_parse_args("s|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcFile, &iMode))
        return NULL;
    return PyLong_FromUnsignedLongLong(gptVfs->open_file(pcFile, iMode).uData);
}

static PyObject*
vfs_close_file(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plVfsFileHandle tHandle = {0};
    if(!pl__vfs_parse_handle(args, kwargs, &tHandle))
        return NULL;
    gptVfs->close_file(tHandle);
    Py_RETURN_NONE;
}

static PyObject*
vfs_delete_file(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plVfsFileHandle tHandle = {0};
    if(!pl__vfs_parse_handle(args, kwargs, &tHandle))
        return NULL;
    return PyBool_FromLong(gptVfs->delete_file(tHandle) == PL_VFS_RESULT_SUCCESS);
}

static PyObject*
vfs_is_file_open(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plVfsFileHandle tHandle = {0};
    if(!pl__vfs_parse_handle(args, kwargs, &tHandle))
        return NULL;
    return PyBool_FromLong(gptVfs->is_file_open(tHandle));
}

static PyObject*
vfs_get_real_path(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plVfsFileHandle tHandle = {0};
    if(!pl__vfs_parse_handle(args, kwargs, &tHandle))
        return NULL;
    const char* pcPath = gptVfs->get_real_path(tHandle);
    if(pcPath == NULL)
        Py_RETURN_NONE;
    return PyUnicode_FromString(pcPath);
}

static PyObject*
vfs_is_file_valid(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plVfsFileHandle tHandle = {0};
    if(!pl__vfs_parse_handle(args, kwargs, &tHandle))
        return NULL;
    return PyBool_FromLong(gptVfs->is_file_valid(tHandle));
}

// returns the file's contents as bytes (None on failure)
static PyObject*
vfs_read_file(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plVfsFileHandle tHandle = {0};
    if(!pl__vfs_parse_handle(args, kwargs, &tHandle))
        return NULL;
    return pl__vfs_read_whole_file(tHandle);
}

// returns the number of bytes written
static PyObject*
vfs_write_file(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"handle", "data", NULL};
    unsigned long long ulData = 0;
    Py_buffer tData = {0};
    if(!pl_parse_args("Ky*", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ulData, &tData))
        return NULL;
    plVfsFileHandle tHandle = {.uData = ulData};
    const size_t szWritten = gptVfs->write_file(tHandle, tData.buf, (size_t)tData.len);
    PyBuffer_Release(&tData);
    return PyLong_FromSize_t(szWritten);
}

//-----------------------------------------------------------------------------
// [SECTION] streams
//-----------------------------------------------------------------------------

static PyObject*
vfs_get_file_stream_position(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plVfsFileHandle tHandle = {0};
    if(!pl__vfs_parse_handle(args, kwargs, &tHandle))
        return NULL;
    return PyLong_FromSize_t(gptVfs->get_file_stream_position(tHandle));
}

static PyObject*
vfs_reset_file_stream_position(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plVfsFileHandle tHandle = {0};
    if(!pl__vfs_parse_handle(args, kwargs, &tHandle))
        return NULL;
    gptVfs->reset_file_stream_position(tHandle);
    Py_RETURN_NONE;
}

static PyObject*
vfs_set_file_stream_position(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"handle", "position", NULL};
    unsigned long long ulData = 0;
    Py_ssize_t szPosition = 0;
    if(!pl_parse_args("Kn", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ulData, &szPosition))
        return NULL;
    if(szPosition < 0)
    {
        PyErr_SetString(PyExc_ValueError, "position must be >= 0");
        return NULL;
    }
    plVfsFileHandle tHandle = {.uData = ulData};
    return PyBool_FromLong(gptVfs->set_file_stream_position(tHandle, (size_t)szPosition));
}

static PyObject*
vfs_increment_file_stream_position(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"handle", "amount", NULL};
    unsigned long long ulData = 0;
    Py_ssize_t szAmount = 0;
    if(!pl_parse_args("Kn", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ulData, &szAmount))
        return NULL;
    if(szAmount < 0)
    {
        PyErr_SetString(PyExc_ValueError, "amount must be >= 0");
        return NULL;
    }
    plVfsFileHandle tHandle = {.uData = ulData};
    return PyBool_FromLong(gptVfs->increment_file_stream_position(tHandle, (size_t)szAmount));
}

// returns bytes holding the complete elements read (len = elements * elementSize)
static PyObject*
vfs_read_file_stream(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"handle", "elementSize", "elementCount", NULL};
    unsigned long long ulData = 0;
    Py_ssize_t szElementSize = 0;
    Py_ssize_t szElementCount = 0;
    if(!pl_parse_args("Knn", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ulData, &szElementSize, &szElementCount))
        return NULL;
    if(szElementSize <= 0 || szElementCount < 0 || szElementCount > PY_SSIZE_T_MAX / szElementSize)
    {
        PyErr_SetString(PyExc_ValueError, "invalid elementSize/elementCount");
        return NULL;
    }
    PyObject* ptBytes = PyBytes_FromStringAndSize(NULL, szElementSize * szElementCount);
    if(ptBytes == NULL)
        return NULL;
    plVfsFileHandle tHandle = {.uData = ulData};
    const size_t szRead = gptVfs->read_file_stream(tHandle, (size_t)szElementSize, (size_t)szElementCount, PyBytes_AS_STRING(ptBytes));
    if(_PyBytes_Resize(&ptBytes, (Py_ssize_t)szRead * szElementSize) < 0)
        return NULL;
    return ptBytes;
}

// data length must be a multiple of elementSize; returns elements written
static PyObject*
vfs_write_file_stream(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"handle", "elementSize", "data", NULL};
    unsigned long long ulData = 0;
    Py_ssize_t szElementSize = 0;
    Py_buffer tData = {0};
    if(!pl_parse_args("Kny*", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ulData, &szElementSize, &tData))
        return NULL;
    if(szElementSize <= 0 || tData.len % szElementSize != 0)
    {
        PyBuffer_Release(&tData);
        PyErr_SetString(PyExc_ValueError, "len(data) must be a multiple of elementSize");
        return NULL;
    }
    plVfsFileHandle tHandle = {.uData = ulData};
    const size_t szWritten = gptVfs->write_file_stream(tHandle, (size_t)szElementSize, (size_t)(tData.len / szElementSize), tData.buf);
    PyBuffer_Release(&tData);
    return PyLong_FromSize_t(szWritten);
}

//-----------------------------------------------------------------------------
// [SECTION] python additions
//-----------------------------------------------------------------------------

// open, read & close; None if the file doesn't exist or can't be read
static PyObject*
vfs_read_all(PyObject* self, PyObject* args, PyObject* kwargs)
{
    const char* pcFile = NULL;
    if(!pl__vfs_parse_file(args, kwargs, &pcFile))
        return NULL;
    if(!gptVfs->does_file_exist(pcFile))
        Py_RETURN_NONE;
    plVfsFileHandle tHandle = gptVfs->open_file(pcFile, PL_VFS_FILE_MODE_READ);
    if(!gptVfs->is_file_valid(tHandle))
        Py_RETURN_NONE;
    PyObject* ptResult = pl__vfs_read_whole_file(tHandle);
    gptVfs->close_file(tHandle);
    return ptResult;
}

// open (truncate), write & close; True if every byte was written
static PyObject*
vfs_write_all(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"file", "data", NULL};
    const char* pcFile = NULL;
    Py_buffer tData = {0};
    if(!pl_parse_args("sy*", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcFile, &tData))
        return NULL;
    plVfsFileHandle tHandle = gptVfs->open_file(pcFile, PL_VFS_FILE_MODE_WRITE);
    bool bResult = false;
    if(gptVfs->is_file_valid(tHandle))
    {
        bResult = gptVfs->write_file(tHandle, tData.buf, (size_t)tData.len) == (size_t)tData.len;
        gptVfs->close_file(tHandle);
    }
    PyBuffer_Release(&tData);
    return PyBool_FromLong(bResult);
}

//-----------------------------------------------------------------------------
// [SECTION] method table
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplVfsI[] =
{
    {"mount_directory",                (PyCFunction)vfs_mount_directory,                PL_VFS_KW, NULL},
    {"mount_pak",                      (PyCFunction)vfs_mount_pak,                      PL_VFS_KW, NULL},
    {"mount_memory",                   (PyCFunction)vfs_mount_memory,                   PL_VFS_KW, NULL},
    {"unmount",                        (PyCFunction)vfs_unmount,                        PL_VFS_KW, NULL},
    {"does_file_exist",                (PyCFunction)vfs_does_file_exist,                PL_VFS_KW, NULL},
    {"get_file_size_str",              (PyCFunction)vfs_get_file_size_str,              PL_VFS_KW, NULL},
    {"register_file",                  (PyCFunction)vfs_register_file,                  PL_VFS_KW, NULL},
    {"open_file",                      (PyCFunction)vfs_open_file,                      PL_VFS_KW, NULL},
    {"close_file",                     (PyCFunction)vfs_close_file,                     PL_VFS_KW, NULL},
    {"delete_file",                    (PyCFunction)vfs_delete_file,                    PL_VFS_KW, NULL},
    {"is_file_open",                   (PyCFunction)vfs_is_file_open,                   PL_VFS_KW, NULL},
    {"get_real_path",                  (PyCFunction)vfs_get_real_path,                  PL_VFS_KW, NULL},
    {"is_file_valid",                  (PyCFunction)vfs_is_file_valid,                  PL_VFS_KW, NULL},
    {"read_file",                      (PyCFunction)vfs_read_file,                      PL_VFS_KW, NULL},
    {"write_file",                     (PyCFunction)vfs_write_file,                     PL_VFS_KW, NULL},
    {"get_file_stream_position",       (PyCFunction)vfs_get_file_stream_position,       PL_VFS_KW, NULL},
    {"reset_file_stream_position",     (PyCFunction)vfs_reset_file_stream_position,     PL_VFS_KW, NULL},
    {"set_file_stream_position",       (PyCFunction)vfs_set_file_stream_position,       PL_VFS_KW, NULL},
    {"increment_file_stream_position", (PyCFunction)vfs_increment_file_stream_position, PL_VFS_KW, NULL},
    {"read_file_stream",               (PyCFunction)vfs_read_file_stream,               PL_VFS_KW, NULL},
    {"write_file_stream",              (PyCFunction)vfs_write_file_stream,              PL_VFS_KW, NULL},
    {"read_all",                       (PyCFunction)vfs_read_all,                       PL_VFS_KW, NULL},
    {"write_all",                      (PyCFunction)vfs_write_all,                      PL_VFS_KW, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plVfsI)

//-----------------------------------------------------------------------------
// [SECTION] constants
//-----------------------------------------------------------------------------

plPythonIntConstantPair gatVfsIntPairs[] = {
    PL_ADD_INT_CONSTANT(PL_VFS_FILE_MODE_READ),
    PL_ADD_INT_CONSTANT(PL_VFS_FILE_MODE_WRITE),
    PL_ADD_INT_CONSTANT(PL_VFS_FILE_MODE_APPEND),
    PL_ADD_INT_CONSTANT(PL_VFS_FILE_MODE_READ_WRITE),
    PL_ADD_INT_CONSTANT(PL_VFS_FILE_MODE_READ_APPEND),
    PL_ADD_INT_CONSTANT(PL_VFS_MOUNT_FLAGS_NONE),
};
