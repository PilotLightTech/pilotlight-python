/*
   pl_ecs_ext_m.c
     * plEcsI
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] plEcsI
// [SECTION] method tables
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] plEcsI
//-----------------------------------------------------------------------------

PL_PY_BIND_VOID(ecs_initialize,           gptEcs->initialize((plEcsInit){0}))
PL_PY_BIND_VOID(ecs_cleanup,              gptEcs->cleanup())
PL_PY_BIND_VOID(ecs_register_asset_types, gptEcs->register_asset_types())
PL_PY_BIND_KEY(ecs_get_asset_type_key,    gptEcs->get_asset_type_key())
PL_PY_BIND_KEY(ecs_get_ecs_type_key_tag,  gptEcs->get_ecs_type_key_tag())
PL_PY_BIND_LIBRARY(ecs_destroy_library,   gptEcs->destroy_library(ptLibrary))
PL_PY_BIND_LIBRARY(ecs_clear_changes,     gptEcs->clear_changes(ptLibrary))

static PyObject*
ecs_finalize(PyObject* self, PyObject* unused)
{
    // ecs components of unstable extensions used by gltf & terrain (see pl_internal_m.c)
    pl__internal_register_ecs_components();
    gptEcs->finalize();
    Py_RETURN_NONE;
}

static PyObject*
ecs_create_library(PyObject* self, PyObject* unused)
{
    return pl_library_to_py(gptEcs->create_library());
}

static PyObject*
ecs_create_entity(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "library",
        "name",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
    const char* pcName = NULL;
	if (!pl_parse_args("Os", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyLibrary, &pcName))
		return NULL;

    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;
    plEntity tEntity = gptEcs->create_entity(ptLibrary, pcName);
    return pl_entity_to_py(0, tEntity);
}

static PyObject*
ecs_remove_entity(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "library",
        "entity",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
    PyObject* ptPyEntity = NULL;
	if (!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyLibrary, &ptPyEntity))
		return NULL;

    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;
    gptEcs->remove_entity(ptLibrary, pl_get_entity_from_python(ptPyEntity).tEntity);
    Py_RETURN_NONE;
}

static PyObject*
ecs_is_entity_valid(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "library",
        "entity",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
    PyObject* ptPyEntity = NULL;
	if (!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyLibrary, &ptPyEntity))
		return NULL;

    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;
    return PyBool_FromLong(gptEcs->is_entity_valid(ptLibrary, pl_get_entity_from_python(ptPyEntity).tEntity));
}

static PyObject*
ecs_has_component(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "library",
        "key",
        "entity",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
    uint32_t uEcsKey = 0;
    PyObject* ptPyEntity = NULL;
	if (!pl_parse_args("OIO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyLibrary, &uEcsKey, &ptPyEntity))
		return NULL;

    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;
    return PyBool_FromLong(gptEcs->has_component(ptLibrary, uEcsKey, pl_get_entity_from_python(ptPyEntity).tEntity));
}

// returns list of entities that have the component
static PyObject*
ecs_get_components(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "library",
        "key",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
    uint32_t uEcsKey = 0;
	if (!pl_parse_args("OI", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyLibrary, &uEcsKey))
		return NULL;

    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;

    const plEntity* ptEntities = NULL;
    const uint32_t uCount = gptEcs->get_components(ptLibrary, uEcsKey, NULL, &ptEntities);

    PyObject* ptResult = PyList_New(uCount);
    for(uint32_t i = 0; i < uCount; i++)
        PyList_SetItem(ptResult, i, pl_entity_to_py(uEcsKey, ptEntities[i]));
    return ptResult;
}

// call between system updates; assets remain shared
static PyObject*
ecs_copy_component(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "library",
        "key",
        "source",
        "destination",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
    uint32_t uEcsKey = 0;
    PyObject* ptPySource = NULL;
    PyObject* ptPyDestination = NULL;
	if (!pl_parse_args("OIOO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyLibrary, &uEcsKey, &ptPySource, &ptPyDestination))
		return NULL;

    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;
    return PyBool_FromLong(gptEcs->copy_component(ptLibrary, uEcsKey,
        pl_get_entity_from_python(ptPySource).tEntity,
        pl_get_entity_from_python(ptPyDestination).tEntity));
}

// does not clone children; returns invalid entity on failure
static PyObject*
ecs_clone_entity(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {
        "library",
        "source",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
    PyObject* ptPySource = NULL;
	if (!pl_parse_args("OO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyLibrary, &ptPySource))
		return NULL;

    plComponentLibrary* ptLibrary = pl_library_from_py(ptPyLibrary);
    if(ptLibrary == NULL)
        return NULL;
    plPythonEntity tSource = pl_get_entity_from_python(ptPySource);
    plEntity tEntity = gptEcs->clone_entity(ptLibrary, tSource.tEntity);
    return pl_entity_to_py(tSource.tKey, tEntity);
}

//-----------------------------------------------------------------------------
// [SECTION] method tables
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplEcsI[] =
{
    {"initialize",           (PyCFunction)ecs_initialize,           METH_NOARGS | METH_STATIC, NULL},
    {"finalize",             (PyCFunction)ecs_finalize,             METH_NOARGS | METH_STATIC, NULL},
    {"cleanup",              (PyCFunction)ecs_cleanup,              METH_NOARGS | METH_STATIC, NULL},
    {"register_asset_types", (PyCFunction)ecs_register_asset_types, METH_NOARGS | METH_STATIC, NULL},
    {"get_asset_type_key",   (PyCFunction)ecs_get_asset_type_key,   METH_NOARGS | METH_STATIC, NULL},
    {"get_ecs_type_key_tag", (PyCFunction)ecs_get_ecs_type_key_tag, METH_NOARGS | METH_STATIC, NULL},
    {"create_library",       (PyCFunction)ecs_create_library,       METH_NOARGS | METH_STATIC, NULL},
    {"destroy_library",      (PyCFunction)ecs_destroy_library,      METH_O | METH_STATIC, NULL},
    {"clear_changes",        (PyCFunction)ecs_clear_changes,        METH_O | METH_STATIC, NULL},
    {"create_entity",        (PyCFunction)ecs_create_entity,        METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"remove_entity",        (PyCFunction)ecs_remove_entity,        METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"is_entity_valid",      (PyCFunction)ecs_is_entity_valid,      METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"has_component",        (PyCFunction)ecs_has_component,        METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_components",       (PyCFunction)ecs_get_components,       METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"copy_component",       (PyCFunction)ecs_copy_component,       METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"clone_entity",         (PyCFunction)ecs_clone_entity,         METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plEcsI)
