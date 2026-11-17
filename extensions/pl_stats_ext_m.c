/*
   pl_stats_ext_m.c
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
stats_new_frame(PyObject* self)
{
    gptStats->new_frame();
    Py_RETURN_NONE;
}

PyObject*
stats_get_counter(PyObject* self, PyObject* arg)
{
    const char* pcName = PyUnicode_AsUTF8(arg);
    double* pdCounter = gptStats->get_counter(pcName);
    return PyCapsule_New(pdCounter, "plDoublePointer", NULL);
}

// names of all counters (get_counter_data isn't bound: the caller provides
// the history buffer, see the stats tool)
static PyObject*
stats_get_names(PyObject* self, PyObject* unused)
{
    uint32_t uCount = 0;
    const char** apcNames = gptStats->get_names(&uCount);
    PyObject* ptList = PyList_New(uCount);
    if(ptList == NULL)
        return NULL;
    for(uint32_t i = 0; i < uCount; i++)
    {
        PyObject* ptName = PyUnicode_FromString(apcNames[i] ? apcNames[i] : "");
        if(ptName == NULL)
        {
            Py_DECREF(ptList);
            return NULL;
        }
        PyList_SET_ITEM(ptList, i, ptName);
    }
    return ptList;
}

static PyObject*
stats_set_max_frames(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"maxFrames", NULL};
    unsigned int uMaxFrames = 0;
    if(!pl_parse_args("I", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uMaxFrames))
        return NULL;
    gptStats->set_max_frames(uMaxFrames);
    Py_RETURN_NONE;
}

static PyObject*
stats_get_max_frames(PyObject* self, PyObject* unused)
{
    return PyLong_FromUnsignedLong(gptStats->get_max_frames());
}

static PyMethodDef gatCommandsplStatsI[] =
{
    {"get_names", (PyCFunction)stats_get_names, METH_NOARGS | METH_STATIC, NULL},
    {"set_max_frames", (PyCFunction)stats_set_max_frames, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_max_frames", (PyCFunction)stats_get_max_frames, METH_NOARGS | METH_STATIC, NULL},
    {"new_frame", (PyCFunction)stats_new_frame, METH_NOARGS | METH_STATIC, NULL},
    {"get_counter", (PyCFunction)stats_get_counter, METH_O | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plStatsI)