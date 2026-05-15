/*
   pl_ecs_ext_m.c
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] binding apis
// [SECTION] implementations
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] implementations
//-----------------------------------------------------------------------------

typedef struct _pyplCameraI
{
    PyObject_HEAD
} pyplCameraI;

PyObject*
camera_register_ecs_system(PyObject* self)
{
 
    gptCamera->register_ecs_system();
    Py_RETURN_NONE;
}

PyObject*
camera_get_ecs_type_key(PyObject* self)
{
 
    plEcsTypeKey tKey = gptCamera->get_ecs_type_key();
    return PyLong_FromUInt32(tKey);
}

PyObject*
camera_create_perspective(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyLibrary = NULL;
    PyObject* ptPyPos = NULL;

    static const char* apcKeywords[] = {
        "library",
        "name",
        "pos",
        "yfov",
        "aspect",
        "nearZ",
        "farZ",
        "reverseZ",
        NULL,
    };

    const char* pcName = NULL;
    float fYFov = 0.0f;
    float fAspect = 1.0f;
    float fNearZ = 0.0f;
    float fFarZ = 0.0f;
    int bReverseZ = false;
	if (!pl_parse("OsOffffp", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyLibrary, &pcName, &ptPyPos, &fYFov, &fAspect, &fNearZ, &fFarZ, &bReverseZ))
		return NULL;

    plComponentLibrary* ptCompLibrary = PyCapsule_GetPointer(ptPyLibrary, "plComponentLibrary");

    plEntity tCamera = gptCamera->create_perspective(
        ptCompLibrary,
        pcName,
        pl_get_dvec3_from_python(ptPyPos),
        fYFov,
        fAspect,
        fNearZ,
        fFarZ,
        bReverseZ,
        NULL);

    return Py_BuildValue("(III)", gptCamera->get_ecs_type_key(), tCamera.uIndex, tCamera.uGeneration);
}

PyObject*
camera_set_fov(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "yfov",
        NULL,
    };

    float fYFov = 0.0f;
	if (!pl_parse("Of", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &fYFov))
		return NULL;

    plCamera* ptCamera = PyCapsule_GetPointer(ptPyCamera, "plEntityComponent");
    gptCamera->set_fov(ptCamera, fYFov);
    Py_RETURN_NONE;
}

PyObject*
camera_update(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;

    static const char* apcKeywords[] = {
        "camera",
        NULL,
    };

	if (!pl_parse("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera))
		return NULL;

    plCamera* ptCamera = PyCapsule_GetPointer(ptPyCamera, "plEntityComponent");
    gptCamera->update(ptCamera);
    Py_RETURN_NONE;
}

static PyMethodDef gatplCameraICommands[] =
{
    {"register_ecs_system", (PyCFunction)camera_register_ecs_system, METH_NOARGS | METH_STATIC, NULL},
    {"create_perspective", (PyCFunction)camera_create_perspective, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_fov", (PyCFunction)camera_set_fov, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"update", (PyCFunction)camera_update, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_ecs_type_key", (PyCFunction)camera_get_ecs_type_key, METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyType_Slot gatplCameraISlots[] = {
    {Py_tp_methods, (void*)gatplCameraICommands},
    {0, 0}
};

static PyType_Spec plCameraISpec = {
    "pilotlight.plCameraI",
    sizeof(pyplCameraI),
    0,
    Py_TPFLAGS_DEFAULT,
    gatplCameraISlots
};