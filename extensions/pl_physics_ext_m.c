/*
   pl_physics_ext_m.c
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] implementations
// [SECTION] method tables
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] implementations
//-----------------------------------------------------------------------------

PL_PY_BIND_VOID(physics_initialize,              gptPhysics->initialize((plPhysicsEngineSettings){0}))
PL_PY_BIND_VOID(physics_cleanup,                 gptPhysics->cleanup())
PL_PY_BIND_VOID(physics_reset,                   gptPhysics->reset())
PL_PY_BIND_VOID(physics_register_ecs_components, gptPhysics->register_ecs_components())
PL_PY_BIND_VOID(physics_wake_up_all,             gptPhysics->wake_up_all())
PL_PY_BIND_VOID(physics_sleep_all,               gptPhysics->sleep_all())
PL_PY_BIND_KEY(physics_get_ecs_type_key_rigid_body_physics, gptPhysics->get_ecs_type_key_rigid_body_physics())
PL_PY_BIND_KEY(physics_get_ecs_type_key_force_field,        gptPhysics->get_ecs_type_key_force_field())

static PyObject*
physics_update(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "deltaTime",
        "library",
        "drawlist",
        NULL,
    };

    float fDeltaTime = 0.0f;
    PyObject* ptPyLibrary = NULL;
    PyObject* ptPyDrawlist = NULL;
	if (!pl_parse_args("fO|O", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &fDeltaTime, &ptPyLibrary, &ptPyDrawlist))
		return NULL;

    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;

    plDrawList3D* ptDrawlist = NULL;
    if(ptPyDrawlist && ptPyDrawlist != Py_None)
        ptDrawlist = PyCapsule_GetPointer(ptPyDrawlist, "plDrawList3D");

    gptPhysics->update(fDeltaTime, ptLibrary, ptDrawlist);
    Py_RETURN_NONE;
}

//-----------------------------------------------------------------------------
// [SECTION] method tables
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplPhysicsI[] =
{
    {"initialize",              (PyCFunction)physics_initialize,              METH_NOARGS | METH_STATIC, NULL},
    {"cleanup",                 (PyCFunction)physics_cleanup,                 METH_NOARGS | METH_STATIC, NULL},
    {"reset",                   (PyCFunction)physics_reset,                   METH_NOARGS | METH_STATIC, NULL},
    {"register_ecs_components", (PyCFunction)physics_register_ecs_components, METH_NOARGS | METH_STATIC, NULL},
    {"wake_up_all",             (PyCFunction)physics_wake_up_all,             METH_NOARGS | METH_STATIC, NULL},
    {"sleep_all",               (PyCFunction)physics_sleep_all,               METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_rigid_body_physics", (PyCFunction)physics_get_ecs_type_key_rigid_body_physics, METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_force_field",        (PyCFunction)physics_get_ecs_type_key_force_field,        METH_NOARGS | METH_STATIC, NULL},
    {"update",                  (PyCFunction)physics_update,                  METH_VARARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plPhysicsI)
