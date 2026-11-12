/*
   pl_animation_ext_m.c
     * plAnimationI
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] plAnimationI
// [SECTION] method tables
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] plAnimationI
//-----------------------------------------------------------------------------

PL_PY_BIND_VOID(animation_register_ecs_components,   gptAnimation->register_ecs_components())
PL_PY_BIND_VOID(animation_register_asset_types,      gptAnimation->register_asset_types())
PL_PY_BIND_KEY(animation_get_asset_type_key,         gptAnimation->get_asset_type_key())
PL_PY_BIND_KEY(animation_get_ecs_type_key_animation, gptAnimation->get_ecs_type_key_animation())
PL_PY_BIND_KEY(animation_get_ecs_type_key_humanoid,  gptAnimation->get_ecs_type_key_humanoid())

static PyObject*
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

    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;

    gptAnimation->run_animation_update_system(ptLibrary, fDeltaTime);
    Py_RETURN_NONE;
}

//-----------------------------------------------------------------------------
// [SECTION] method tables
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplAnimationI[] =
{
    {"register_ecs_components",     (PyCFunction)animation_register_ecs_components,     METH_NOARGS | METH_STATIC, NULL},
    {"register_asset_types",        (PyCFunction)animation_register_asset_types,        METH_NOARGS | METH_STATIC, NULL},
    {"get_asset_type_key",          (PyCFunction)animation_get_asset_type_key,          METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_animation",  (PyCFunction)animation_get_ecs_type_key_animation,  METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_humanoid",   (PyCFunction)animation_get_ecs_type_key_humanoid,   METH_NOARGS | METH_STATIC, NULL},
    {"run_animation_update_system", (PyCFunction)animation_run_animation_update_system, METH_VARARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plAnimationI)
