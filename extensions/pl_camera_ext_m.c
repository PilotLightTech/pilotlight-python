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