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
physics_register_ecs_system(PyObject* self)
{
 
    gptPhysics->register_ecs_system();
    Py_RETURN_NONE;
}

PyObject*
physics_update(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "deltaTime",
        "library",
        NULL,
    };

    float fDeltaTime = 0.0f;
    PyObject* ptPyLibrary = NULL;
	if (!pl_parse_args("fO", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &fDeltaTime, &ptPyLibrary))
		return NULL;

    plComponentLibrary* ptCompLibrary = PyCapsule_GetPointer(ptPyLibrary, "plComponentLibrary");

    gptPhysics->update(fDeltaTime, ptCompLibrary);
    Py_RETURN_NONE;
}

static PyMethodDef gatCommandsplPhysicsI[] =
{
    {"register_ecs_system", (PyCFunction)physics_register_ecs_system, METH_NOARGS | METH_STATIC, NULL},
    {"update", (PyCFunction)physics_update, METH_VARARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plPhysicsI)