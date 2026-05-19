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

// typedef struct _pyplCameraEcsI
// {
//     PyObject_HEAD
// } pyplCameraEcsI;

typedef struct _plPyCamera
{
    PyObject_HEAD
    plCamera tCamera;
} plPyCamera;

PyObject*
camera_register_ecs_system(PyObject* self)
{
 
    gptCameraEcs->register_ecs_system();
    Py_RETURN_NONE;
}

PyObject*
camera_get_ecs_type_key(PyObject* self)
{
 
    plEcsTypeKey tKey = gptCameraEcs->get_ecs_type_key();
    return PyLong_FromUInt32(tKey);
}

PyObject*
camera_ecs_create_perspective(PyObject* self, PyObject* args, PyObject* kwargs)
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

    plEntity tCamera = gptCameraEcs->create_perspective(
        ptCompLibrary,
        pcName,
        pl_get_dvec3_from_python(ptPyPos),
        fYFov,
        fAspect,
        fNearZ,
        fFarZ,
        bReverseZ,
        NULL);

    return Py_BuildValue("(III)", gptCameraEcs->get_ecs_type_key(), tCamera.uIndex, tCamera.uGeneration);
}

PyObject*
camera_translate(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "dX",
        "dY",
        "dZ",
        NULL,
    };

    double dX = 0.0;
    double dY = 0.0;
    double dZ = 0.0;
	if (!pl_parse("Oddd", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &dX, &dY, &dZ))
		return NULL;

    plCamera* ptCamera = NULL;
    if(PyObject_TypeCheck(ptPyCamera, (PyTypeObject*)gptCameraType))
    {
        plPyCamera* ptPyTypeCamera = (plPyCamera*)ptPyCamera;
        ptCamera = &ptPyTypeCamera->tCamera;
    }   
    else
    {
        ptCamera = PyCapsule_GetPointer(ptPyCamera, "plEntityComponent");
    }
    gptCamera->translate(ptCamera, dX, dY, dZ);
    Py_RETURN_NONE;
}

PyObject*
camera_rotate(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "fPitch",
        "fYaw",
        NULL,
    };

    float fPitch = 0.0f;
    float fYaw = 0.0f;
	if (!pl_parse("Off", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &fPitch, &fYaw))
		return NULL;

    plCamera* ptCamera = NULL;
    if(PyObject_TypeCheck(ptPyCamera, (PyTypeObject*)gptCameraType))
    {
        plPyCamera* ptPyTypeCamera = (plPyCamera*)ptPyCamera;
        ptCamera = &ptPyTypeCamera->tCamera;
    }   
    else
    {
        ptCamera = PyCapsule_GetPointer(ptPyCamera, "plEntityComponent");
    }
    gptCamera->rotate(ptCamera, fPitch, fYaw);
    Py_RETURN_NONE;
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

    plCamera* ptCamera = NULL;
    if(PyObject_TypeCheck(ptPyCamera, (PyTypeObject*)gptCameraType))
    {
        plPyCamera* ptPyTypeCamera = (plPyCamera*)ptPyCamera;
        ptCamera = &ptPyTypeCamera->tCamera;
    }   
    else
    {
        ptCamera = PyCapsule_GetPointer(ptPyCamera, "plEntityComponent");
    }
    gptCamera->set_fov(ptCamera, fYFov);
    Py_RETURN_NONE;
}

PyObject*
camera_set_aspect(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "aspect",
        NULL,
    };

    float fAspect = 0.0f;
	if (!pl_parse("Of", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &fAspect))
		return NULL;

    plCamera* ptCamera = NULL;
    if(PyObject_TypeCheck(ptPyCamera, (PyTypeObject*)gptCameraType))
    {
        plPyCamera* ptPyTypeCamera = (plPyCamera*)ptPyCamera;
        ptCamera = &ptPyTypeCamera->tCamera;
    }   
    else
    {
        ptCamera = PyCapsule_GetPointer(ptPyCamera, "plEntityComponent");
    }
    gptCamera->set_aspect(ptCamera, fAspect);
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

    plCamera* ptCamera = NULL;
    if(PyObject_TypeCheck(ptPyCamera, (PyTypeObject*)gptCameraType))
    {
        plPyCamera* ptPyTypeCamera = (plPyCamera*)ptPyCamera;
        ptCamera = &ptPyTypeCamera->tCamera;
    }   
    else
    {
        ptCamera = PyCapsule_GetPointer(ptPyCamera, "plEntityComponent");
    }
    gptCamera->update(ptCamera);
    Py_RETURN_NONE;
}

PyObject*
camera_init_perspective(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;
    PyObject* ptPyPos = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "pos",
        "yfov",
        "aspect",
        "nearZ",
        "farZ",
        "reverseZ",
        NULL,
    };

    float fYFov = 0.0f;
    float fAspect = 1.0f;
    float fNearZ = 0.0f;
    float fFarZ = 0.0f;
    int bReverseZ = false;
	if (!pl_parse("OOffffp", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &ptPyPos, &fYFov, &fAspect, &fNearZ, &fFarZ, &bReverseZ))
		return NULL;

    plCamera* ptCamera = NULL;
    if(PyObject_TypeCheck(ptPyCamera, (PyTypeObject*)gptCameraType))
    {
        plPyCamera* ptPyTypeCamera = (plPyCamera*)ptPyCamera;
        ptCamera = &ptPyTypeCamera->tCamera;
    }   
    else
    {
        ptCamera = PyCapsule_GetPointer(ptPyCamera, "plEntityComponent");
    }

    gptCamera->init_perspective(
        ptCamera,
        pl_get_dvec3_from_python(ptPyPos),
        fYFov,
        fAspect,
        fNearZ,
        fFarZ,
        bReverseZ);

    Py_RETURN_NONE;
}

static PyMethodDef gatCommandsplCameraI[] =
{
    {"translate", (PyCFunction)camera_translate, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"rotate", (PyCFunction)camera_rotate, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_fov", (PyCFunction)camera_set_fov, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_aspect", (PyCFunction)camera_set_aspect, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"update", (PyCFunction)camera_update, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"init_perspective", (PyCFunction)camera_init_perspective, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyMethodDef gatCommandsplCameraEcsI[] =
{
    {"register_ecs_system", (PyCFunction)camera_register_ecs_system, METH_NOARGS | METH_STATIC, NULL},
    {"create_perspective", (PyCFunction)camera_ecs_create_perspective, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_ecs_type_key", (PyCFunction)camera_get_ecs_type_key, METH_NOARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plCameraI)
PL_NEW_PYTHON_API(plCameraEcsI)