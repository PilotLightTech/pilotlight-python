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
    return PyCapsule_New(pComponent, "plEntityComponent", NULL);
}