/*
   pl_transform_ext_m.c
     * plTransformI
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

PL_PY_BIND_VOID(transform_register_ecs_components,      gptTransform->register_ecs_components())
PL_PY_BIND_KEY(transform_get_ecs_type_key_transform,    gptTransform->get_ecs_type_key_transform())
PL_PY_BIND_KEY(transform_get_ecs_type_key_hierarchy,    gptTransform->get_ecs_type_key_hierarchy())
PL_PY_BIND_LIBRARY(transform_run_transform_update_system, gptTransform->run_transform_update_system(ptLibrary))
PL_PY_BIND_LIBRARY(transform_run_hierarchy_update_system, gptTransform->run_hierarchy_update_system(ptLibrary))

//-----------------------------------------------------------------------------
// [SECTION] method tables
//-----------------------------------------------------------------------------

// returns the new entity (same tuple form as plEcsI.create_entity)
static PyObject*
transform_create_transform(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"library", "name", NULL};
    PyObject* ptPyLibrary = NULL;
    const char* pcName = NULL;
    if(!pl_parse_args("O|z", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptPyLibrary, &pcName))
        return NULL;
    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;
    plEntity tEntity = gptTransform->create_transform(ptLibrary, pcName, NULL);
    return pl_entity_to_py(0, tEntity);
}

static PyObject*
transform_attach_component(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"library", "entity", "parent", NULL};
    PyObject* ptPyLibrary = NULL;
    PyObject* ptPyEntity = NULL;
    PyObject* ptPyParent = NULL;
    if(!pl_parse_args("OOO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptPyLibrary, &ptPyEntity, &ptPyParent))
        return NULL;
    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;
    gptTransform->attach_component(ptLibrary, pl_get_entity_from_python(ptPyEntity).tEntity, pl_get_entity_from_python(ptPyParent).tEntity);
    if(PyErr_Occurred())
        return NULL;
    Py_RETURN_NONE;
}

static PyObject*
transform_detach_component(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"library", "entity", NULL};
    PyObject* ptPyLibrary = NULL;
    PyObject* ptPyEntity = NULL;
    if(!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptPyLibrary, &ptPyEntity))
        return NULL;
    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;
    gptTransform->detach_component(ptLibrary, pl_get_entity_from_python(ptPyEntity).tEntity);
    if(PyErr_Occurred())
        return NULL;
    Py_RETURN_NONE;
}

static PyObject*
transform_compute_parent_transform(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"library", "entity", NULL};
    PyObject* ptPyLibrary = NULL;
    PyObject* ptPyEntity = NULL;
    if(!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptPyLibrary, &ptPyEntity))
        return NULL;
    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;
    const plEntity tEntity = pl_get_entity_from_python(ptPyEntity).tEntity;
    if(PyErr_Occurred())
        return NULL;
    return pl_mat4_to_py(gptTransform->compute_parent_transform(ptLibrary, tEntity));
}

static PyObject*
transform_set_world_transform(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"library", "entity", "transform", NULL};
    PyObject* ptPyLibrary = NULL;
    PyObject* ptPyEntity = NULL;
    PyObject* ptPyTransform = NULL;
    if(!pl_parse_args("OOO", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &ptPyLibrary, &ptPyEntity, &ptPyTransform))
        return NULL;
    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;
    plMat4 tTransform = {0};
    if(!pl_mat4_from_py(ptPyTransform, &tTransform))
        return NULL;
    const plEntity tEntity = pl_get_entity_from_python(ptPyEntity).tEntity;
    if(PyErr_Occurred())
        return NULL;
    gptTransform->set_world_transform(ptLibrary, tEntity, &tTransform);
    Py_RETURN_NONE;
}

static PyMethodDef gatCommandsplTransformI[] =
{
    {"create_transform", (PyCFunction)transform_create_transform, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"attach_component", (PyCFunction)transform_attach_component, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"detach_component", (PyCFunction)transform_detach_component, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"compute_parent_transform", (PyCFunction)transform_compute_parent_transform, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_world_transform", (PyCFunction)transform_set_world_transform, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"register_ecs_components",     (PyCFunction)transform_register_ecs_components,     METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_transform",  (PyCFunction)transform_get_ecs_type_key_transform,  METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_hierarchy",  (PyCFunction)transform_get_ecs_type_key_hierarchy,  METH_NOARGS | METH_STATIC, NULL},
    {"run_transform_update_system", (PyCFunction)transform_run_transform_update_system, METH_O | METH_STATIC, NULL},
    {"run_hierarchy_update_system", (PyCFunction)transform_run_hierarchy_update_system, METH_O | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plTransformI)
