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

plPythonIntConstantPair gatCameraIntPairs[] = {

    // plCameraProjectionType
    PL_ADD_INT_CONSTANT(PL_CAMERA_PROJECTION_TYPE_PERSPECTIVE),
    PL_ADD_INT_CONSTANT(PL_CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC),

    // plCameraDepthMode
    PL_ADD_INT_CONSTANT(PL_CAMERA_DEPTH_MODE_STANDARD),
    PL_ADD_INT_CONSTANT(PL_CAMERA_DEPTH_MODE_REVERSE_Z)
};

static inline void
pl__get_camera_perspective_desc(PyObject* ptPythonDesc, plCameraPerspectiveDesc* ptDescOut)
{
    {
        PyObject* ptPythonOption = PyObject_GetAttrString(ptPythonDesc, "eDepthMode");
        ptDescOut->eDepthMode = PyLong_AsLong(ptPythonOption);
        Py_DECREF(ptPythonOption);
    }
    {
        PyObject* ptPythonOption = PyObject_GetAttrString(ptPythonDesc, "fYFov");
        ptDescOut->fYFov = (float)PyFloat_AsDouble(ptPythonOption);
        Py_DECREF(ptPythonOption);
    }
    {
        PyObject* ptPythonOption = PyObject_GetAttrString(ptPythonDesc, "fAspectRatio");
        ptDescOut->fAspectRatio = (float)PyFloat_AsDouble(ptPythonOption);
        Py_DECREF(ptPythonOption);
    }
    {
        PyObject* ptPythonOption = PyObject_GetAttrString(ptPythonDesc, "fNearZ");
        ptDescOut->fNearZ = (float)PyFloat_AsDouble(ptPythonOption);
        Py_DECREF(ptPythonOption);
    }
    {
        PyObject* ptPythonOption = PyObject_GetAttrString(ptPythonDesc, "fFarZ");
        ptDescOut->fFarZ = (float)PyFloat_AsDouble(ptPythonOption);
        Py_DECREF(ptPythonOption);
    }
}

static inline void
pl__get_camera_ortho_desc(PyObject* ptPythonDesc, plCameraOrthographicDesc* ptDescOut)
{
    {
        PyObject* ptPythonOption = PyObject_GetAttrString(ptPythonDesc, "eDepthMode");
        ptDescOut->eDepthMode = PyLong_AsLong(ptPythonOption);
        Py_DECREF(ptPythonOption);
    }
    {
        PyObject* ptPythonOption = PyObject_GetAttrString(ptPythonDesc, "fWidth");
        ptDescOut->fWidth = (float)PyFloat_AsDouble(ptPythonOption);
        Py_DECREF(ptPythonOption);
    }
    {
        PyObject* ptPythonOption = PyObject_GetAttrString(ptPythonDesc, "fHeight");
        ptDescOut->fHeight = (float)PyFloat_AsDouble(ptPythonOption);
        Py_DECREF(ptPythonOption);
    }
    {
        PyObject* ptPythonOption = PyObject_GetAttrString(ptPythonDesc, "fNearZ");
        ptDescOut->fNearZ = (float)PyFloat_AsDouble(ptPythonOption);
        Py_DECREF(ptPythonOption);
    }
    {
        PyObject* ptPythonOption = PyObject_GetAttrString(ptPythonDesc, "fFarZ");
        ptDescOut->fFarZ = (float)PyFloat_AsDouble(ptPythonOption);
        Py_DECREF(ptPythonOption);
    }
}

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
camera_ecs_run_ecs(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "library",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
	if (!pl_parse_args("O", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPyLibrary))
		return NULL;

    plComponentLibrary* ptCompLibrary = PyCapsule_GetPointer(ptPyLibrary, "plComponentLibrary");

    gptCameraEcs->run_ecs(ptCompLibrary);
    Py_RETURN_NONE;
}

PyObject*
camera_ecs_create_perspective(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyLibrary = NULL;
    PyObject* ptPyDesc = NULL;

    static const char* apcKeywords[] = {
        "library",
        "name",
        "desc",
        NULL,
    };

    const char* pcName = NULL;
	if (!pl_parse_args("OsO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyLibrary, &pcName, &ptPyDesc))
		return NULL;

    plComponentLibrary* ptCompLibrary = PyCapsule_GetPointer(ptPyLibrary, "plComponentLibrary");

    plCameraPerspectiveDesc tDesc = {0};
    pl__get_camera_perspective_desc(ptPyDesc, &tDesc);

    plEntity tCamera = gptCameraEcs->create_perspective(ptCompLibrary, pcName, &tDesc, NULL);

    return pl_entity_to_py(gptCameraEcs->get_ecs_type_key(), tCamera);
}

PyObject*
camera_set_position(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;
    PyObject* ptPyDelta = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "position",
        NULL,
    };

	if (!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &ptPyDelta))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;

    plVec3 tDelta = {0};
    pl_vec3_from_py(ptPyDelta, &tDelta);
    gptCamera->set_position(ptCamera, (plVec3d){(double)tDelta.x, (double)tDelta.y, (double)tDelta.z});
    Py_RETURN_NONE;
}

PyObject*
camera_set_rotation(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;
    PyObject* ptPyRotation = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "rotation",
        NULL,
    };

	if (!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &ptPyRotation))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;

    plVec4 tRotation = {0};
    pl_vec4_from_py(ptPyRotation, &tRotation);
    gptCamera->set_rotation(ptCamera, tRotation);
    Py_RETURN_NONE;
}

PyObject*
camera_set_transform(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;
    PyObject* ptPyPosition = NULL;
    PyObject* ptPyRotation = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "position",
        "rotation",
        NULL,
    };

	if (!pl_parse_args("OOO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &ptPyPosition, &ptPyRotation))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;

    plVec4 tRotation = {0};
    pl_vec4_from_py(ptPyRotation, &tRotation);

    plVec3 tPosition = {0};
    pl_vec3_from_py(ptPyPosition, &tPosition);
    gptCamera->set_transform(ptCamera, (plVec3d){(double)tPosition.x, (double)tPosition.y, (double)tPosition.z}, tRotation);
    Py_RETURN_NONE;
}

PyObject*
camera_look_at(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;
    PyObject* ptPyEye = NULL;
    PyObject* ptPyTarget = NULL;
    PyObject* ptPyUp = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "eye",
        "target",
        "up",
        NULL,
    };

	if (!pl_parse_args("OOOO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &ptPyEye, &ptPyTarget, &ptPyUp))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;

    plVec3 tEye = {0};
    pl_vec3_from_py(ptPyEye, &tEye);

    plVec3 tTarget = {0};
    pl_vec3_from_py(ptPyTarget, &tTarget);

    plVec3 tUp = {0};
    pl_vec3_from_py(ptPyUp, &tUp);


    gptCamera->look_at(ptCamera,
        (plVec3d){(double)tEye.x, (double)tEye.y, (double)tEye.z},
        (plVec3d){(double)tTarget.x, (double)tTarget.y, (double)tTarget.z},
        tUp);
    Py_RETURN_NONE;
}

PyObject*
camera_translate(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;
    PyObject* ptPyDelta = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "delta",
        NULL,
    };

	if (!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &ptPyDelta))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;

    plVec3 tDelta = {0};
    pl_vec3_from_py(ptPyDelta, &tDelta);
    gptCamera->translate(ptCamera, (plVec3d){(double)tDelta.x, (double)tDelta.y, (double)tDelta.z});
    Py_RETURN_NONE;
}

PyObject*
camera_translate_local(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;
    PyObject* ptPyDelta = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "delta",
        NULL,
    };

	if (!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &ptPyDelta))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;

    plVec3 tDelta = {0};
    pl_vec3_from_py(ptPyDelta, &tDelta);
    gptCamera->translate_local(ptCamera, (plVec3d){(double)tDelta.x, (double)tDelta.y, (double)tDelta.z});
    Py_RETURN_NONE;
}

PyObject*
camera_rotate_euler(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "fPitch",
        "fYaw",
        "fRoll",
        NULL,
    };

    float fPitch = 0.0f;
    float fYaw = 0.0f;
    float fRoll = 0.0f;
	if (!pl_parse_args("Offf", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &fPitch, &fYaw, &fRoll))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;
    gptCamera->rotate_euler(ptCamera, fPitch, fYaw, fRoll);
    Py_RETURN_NONE;
}

PyObject*
camera_rotate_euler_local(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "fPitch",
        "fYaw",
        "fRoll",
        NULL,
    };

    float fPitch = 0.0f;
    float fYaw = 0.0f;
    float fRoll = 0.0f;
	if (!pl_parse_args("Offf", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &fPitch, &fYaw, &fRoll))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;
    gptCamera->rotate_euler_local(ptCamera, fPitch, fYaw, fRoll);
    Py_RETURN_NONE;
}

PyObject*
camera_set_euler(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "fPitch",
        "fYaw",
        "fRoll",
        NULL,
    };

    float fPitch = 0.0f;
    float fYaw = 0.0f;
    float fRoll = 0.0f;
	if (!pl_parse_args("Offf", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &fPitch, &fYaw, &fRoll))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;
    gptCamera->set_euler(ptCamera, fPitch, fYaw, fRoll);
    Py_RETURN_NONE;
}

PyObject*
camera_set_y_fov(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "yfov",
        NULL,
    };

    float fYFov = 0.0f;
	if (!pl_parse_args("Of", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &fYFov))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;
    gptCamera->set_y_fov(ptCamera, fYFov);
    Py_RETURN_NONE;
}

PyObject*
camera_set_viewport(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "width",
        "height",
        NULL,
    };

    float fWidth = 0.0f;
    float fHeight = 0.0f;
	if (!pl_parse_args("Off", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &fWidth, &fHeight))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;
    gptCamera->set_viewport(ptCamera, fWidth, fHeight);
    Py_RETURN_NONE;
}

PyObject*
camera_set_clip_planes(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "nearZ",
        "farZ",
        NULL,
    };

    float fNearZ = 0.0f;
    float fFarZ = 0.0f;
	if (!pl_parse_args("Off", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &fNearZ, &fFarZ))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;
    gptCamera->set_clip_planes(ptCamera, fNearZ, fFarZ);
    Py_RETURN_NONE;
}

PyObject*
camera_set_depth_mode(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "mode",
        NULL,
    };

    plCameraDepthMode tMode = 0;
	if (!pl_parse_args("Oi", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &tMode))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;
    gptCamera->set_depth_mode(ptCamera, tMode);
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

	if (!pl_parse_args("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;
    gptCamera->update(ptCamera);
    Py_RETURN_NONE;
}

PyObject*
camera_set_perspective(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;
    PyObject* ptPyDesc = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "desc",
        NULL,
    };

	if (!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &ptPyDesc))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;

    plCameraPerspectiveDesc tDesc = {0};
    pl__get_camera_perspective_desc(ptPyDesc, &tDesc);

    gptCamera->set_perspective(ptCamera, &tDesc);

    Py_RETURN_NONE;
}

PyObject*
camera_set_orthographic(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;
    PyObject* ptPyDesc = NULL;

    static const char* apcKeywords[] = {
        "camera",
        "desc",
        NULL,
    };

	if (!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera, &ptPyDesc))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;
    plCameraOrthographicDesc tDesc = {0};
    pl__get_camera_ortho_desc(ptPyDesc, &tDesc);

    gptCamera->set_orthographic(ptCamera, &tDesc);

    Py_RETURN_NONE;
}

PyObject*
camera_init(PyObject* self, PyObject* args, PyObject* kwargs)
{
 
    PyObject* ptPyCamera = NULL;

    static const char* apcKeywords[] = {
        "camera",
        NULL,
    };

	if (!pl_parse_args("O", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyCamera))
		return NULL;

    pyplCamera* ptPyTypeCamera = (pyplCamera*)ptPyCamera;
    plCamera* ptCamera = ptPyTypeCamera->ptCamera;

    gptCamera->init(ptCamera);
    Py_RETURN_NONE;
}

static PyMethodDef gatCommandsplCameraI[] =
{
    {"init", (PyCFunction)camera_init, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"look_at", (PyCFunction)camera_look_at, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_depth_mode", (PyCFunction)camera_set_depth_mode, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_position", (PyCFunction)camera_set_position, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_rotation", (PyCFunction)camera_set_rotation, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_transform", (PyCFunction)camera_set_transform, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"translate", (PyCFunction)camera_translate, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"translate_local", (PyCFunction)camera_translate_local, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"rotate_euler", (PyCFunction)camera_rotate_euler, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"rotate_euler_local", (PyCFunction)camera_rotate_euler_local, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_set_euler", (PyCFunction)camera_set_euler, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_y_fov", (PyCFunction)camera_set_y_fov, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_viewport", (PyCFunction)camera_set_viewport, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_clip_planes", (PyCFunction)camera_set_clip_planes, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"update", (PyCFunction)camera_update, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_perspective", (PyCFunction)camera_set_perspective, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_orthographic", (PyCFunction)camera_set_orthographic, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyMethodDef gatCommandsplCameraEcsI[] =
{
    {"register_ecs_system", (PyCFunction)camera_register_ecs_system, METH_NOARGS | METH_STATIC, NULL},
    {"create_perspective", (PyCFunction)camera_ecs_create_perspective, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_ecs_type_key", (PyCFunction)camera_get_ecs_type_key, METH_NOARGS | METH_STATIC, NULL},
    {"run_ecs", (PyCFunction)camera_ecs_run_ecs, METH_VARARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plCameraI)
PL_NEW_PYTHON_API(plCameraEcsI)