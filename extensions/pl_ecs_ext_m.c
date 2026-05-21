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
ecs_initialize(PyObject* self)
{
 
    gptECS->initialize((plEcsInit){0});
    Py_RETURN_NONE;
}

PyObject*
ecs_finalize(PyObject* self)
{
 
    gptECS->finalize();
    Py_RETURN_NONE;
}

PyObject*
ecs_cleanup(PyObject* self)
{
 
    gptECS->cleanup();
    Py_RETURN_NONE;
}

PyObject*
ecs_get_default_library(PyObject* self, PyObject* args, PyObject* kwargs)
{
    plComponentLibrary* ptCompLibrary = gptECS->get_default_library();
    return PyCapsule_New(ptCompLibrary, "plComponentLibrary", NULL);
}

PyObject*
ecs_get_component(PyObject* self, PyObject* args, PyObject* kwargs)
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
	if (!pl_parse("OIO", (const char**)apcKeywords, args, kwargs, __FUNCTION__,
        &ptPyLibrary, &uEcsKey, &ptPyEntity))
		return NULL;

    plComponentLibrary* ptCompLibrary = PyCapsule_GetPointer(ptPyLibrary, "plComponentLibrary");
    plPythonEntity tPyEntity = pl_get_entity_from_python(ptPyEntity);

    void* pComponent = gptECS->get_component(ptCompLibrary, uEcsKey, tPyEntity.tEntity);
    if(uEcsKey == gptCameraEcs->get_ecs_type_key())
    {
        plPyCamera* obj = (plPyCamera*)PyObject_CallObject(gptCameraType, NULL);
        PL_FREE(obj->ptCamera);
        obj->ptCamera = pComponent;
        return (PyObject*)obj;
    }
    Py_RETURN_NONE;
}

PyObject*
ecs_run_transform_update_system(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "library",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
	if (!pl_parse("O", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPyLibrary))
		return NULL;

    plComponentLibrary* ptCompLibrary = PyCapsule_GetPointer(ptPyLibrary, "plComponentLibrary");

    gptECS->run_transform_update_system(ptCompLibrary);
    Py_RETURN_NONE;
}

PyObject*
ecs_run_hierarchy_update_system(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "library",
        NULL,
    };

    PyObject* ptPyLibrary = NULL;
	if (!pl_parse("O", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &ptPyLibrary))
		return NULL;

    plComponentLibrary* ptCompLibrary = PyCapsule_GetPointer(ptPyLibrary, "plComponentLibrary");

    gptECS->run_hierarchy_update_system(ptCompLibrary);
    Py_RETURN_NONE;
}

static PyMethodDef gatCommandsplEcsI[] =
{
    {"initialize", (PyCFunction)ecs_initialize, METH_NOARGS | METH_STATIC, NULL},
    {"finalize", (PyCFunction)ecs_finalize, METH_NOARGS | METH_STATIC, NULL},
    {"cleanup", (PyCFunction)ecs_cleanup, METH_NOARGS | METH_STATIC, NULL},
    {"get_default_library", (PyCFunction)ecs_get_default_library, METH_NOARGS | METH_STATIC, NULL},
    {"get_component", (PyCFunction)ecs_get_component, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"run_transform_update_system", (PyCFunction)ecs_run_transform_update_system, METH_VARARGS | METH_STATIC, NULL},
    {"run_hierarchy_update_system", (PyCFunction)ecs_run_hierarchy_update_system, METH_VARARGS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plEcsI)