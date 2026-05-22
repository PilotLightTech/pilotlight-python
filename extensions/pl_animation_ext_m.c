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
animation_register_ecs_system(PyObject* self)
{
 
    gptAnimation->register_ecs_system();
    Py_RETURN_NONE;
}

PyObject*
animation_run_animation_update_system(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "library",
        "deltaTime",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
    float fDeltaTime = 0.0f;
	if (!pl_parse_args("Of", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPyLibrary, &fDeltaTime))
		return NULL;

    plComponentLibrary* ptCompLibrary = PyCapsule_GetPointer(ptPyLibrary, "plComponentLibrary");

    gptAnimation->run_animation_update_system(ptCompLibrary, fDeltaTime);
    Py_RETURN_NONE;
}

PyObject*
animation_run_inverse_kinematics_update_system(PyObject* self, PyObject* args)
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

    gptAnimation->run_inverse_kinematics_update_system(ptCompLibrary);
    Py_RETURN_NONE;
}

static PyMethodDef gatCommandsplAnimationI[] =
{
    {"register_ecs_system", (PyCFunction)animation_register_ecs_system, METH_NOARGS | METH_STATIC, NULL},
    {"run_animation_update_system", (PyCFunction)animation_run_animation_update_system, METH_VARARGS | METH_STATIC, NULL},
    {"run_inverse_kinematics_update_system", (PyCFunction)animation_run_inverse_kinematics_update_system, METH_VARARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plAnimationI)
