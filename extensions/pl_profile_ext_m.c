/*
   pl_profile_ext_m.c
     * plProfileI (cpu profiling extension)
     * differences from the C api:
         - begin_frame/end_frame/set_thread_count are not bound (plStarterI
           drives frames when PL_STARTER_FLAGS_PROFILE_EXT is set)
         - get_last_frame_samples returns a list of plProfileCpuSample
           (struct sequence)
         - see pilotlight.profile for a "with profile_sample(...)" helper
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] python types
// [SECTION] implementations
// [SECTION] method table
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] python types
//-----------------------------------------------------------------------------

static PyTypeObject* gptProfileCpuSampleType = NULL;

static PyStructSequence_Field gatProfileCpuSampleFields[] = {
    {"dStartTime", "seconds since the start of the frame"},
    {"dDuration",  "seconds"},
    {"pcName",     "sample name"},
    {"uDepth",     "nesting depth (0 = top level)"},
    {NULL, NULL}
};

static PyStructSequence_Desc gtProfileCpuSampleDesc = {
    "pilotlight.plProfileCpuSample",
    "a cpu profile sample from the last frame (plProfileCpuSample)",
    gatProfileCpuSampleFields,
    4
};

// called from PyInit_pilotlight
static int
pl__profile_add_python_types(PyObject* ptModule)
{
    gptProfileCpuSampleType = PyStructSequence_NewType(&gtProfileCpuSampleDesc);
    if(gptProfileCpuSampleType == NULL)
        return -1;
    Py_INCREF(gptProfileCpuSampleType);
    if(PyModule_AddObject(ptModule, "plProfileCpuSample", (PyObject*)gptProfileCpuSampleType) < 0)
        return -1;
    return 0;
}

//-----------------------------------------------------------------------------
// [SECTION] implementations
//-----------------------------------------------------------------------------

// open samples per thread, so an unmatched end_sample raises instead of
// tripping the profiler's assert
#define PL_PROFILE_MAX_PY_THREADS 64
static uint32_t gauProfileOpenSamples[PL_PROFILE_MAX_PY_THREADS] = {0};

// the C api doesn't range check thread indices
static bool
pl__profile_check_thread(unsigned int uThreadIndex)
{
    if(gptProfileContext == NULL || uThreadIndex >= gptProfileContext->uThreadCount || uThreadIndex >= PL_PROFILE_MAX_PY_THREADS)
    {
        PyErr_Format(PyExc_IndexError, "threadIndex %u out of range (profiler thread count %u)",
            uThreadIndex, gptProfileContext ? gptProfileContext->uThreadCount : 0);
        return false;
    }
    return true;
}

static PyObject*
profile_begin_sample(PyObject* self, PyObject* args, PyObject* kwargs)
{
    // python code runs on the main thread (threadIndex 0)
    static const char* apcKeywords[] = {"threadIndex", "name", NULL};
    unsigned int uThreadIndex = 0;
    const char* pcName = NULL; // copied by the profiler
    if(!pl_parse_args("Is", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uThreadIndex, &pcName))
        return NULL;
    if(!pl__profile_check_thread(uThreadIndex))
        return NULL;

    gptProfile->begin_sample(uThreadIndex, pcName);
    gauProfileOpenSamples[uThreadIndex]++;
    Py_RETURN_NONE;
}

static PyObject*
profile_end_sample(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"threadIndex", NULL};
    unsigned int uThreadIndex = 0;
    if(!pl_parse_args("I", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uThreadIndex))
        return NULL;
    if(!pl__profile_check_thread(uThreadIndex))
        return NULL;
    if(gauProfileOpenSamples[uThreadIndex] == 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "end_sample() without a matching begin_sample()");
        return NULL;
    }
    gauProfileOpenSamples[uThreadIndex]--;
    gptProfile->end_sample(uThreadIndex);
    Py_RETURN_NONE;
}

static PyObject*
profile_get_last_frame_samples(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"threadIndex", NULL};
    unsigned int uThreadIndex = 0;
    if(!pl_parse_args("I", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uThreadIndex))
        return NULL;
    if(!pl__profile_check_thread(uThreadIndex))
        return NULL;

    uint32_t uCount = 0;
    const plProfileCpuSample* atSamples = gptProfile->get_last_frame_samples(uThreadIndex, &uCount);
    if(atSamples == NULL)
        uCount = 0;

    PyObject* ptList = PyList_New(uCount);
    if(ptList == NULL)
        return NULL;
    for(uint32_t i = 0; i < uCount; i++)
    {
        PyObject* ptSample = PyStructSequence_New(gptProfileCpuSampleType);
        if(ptSample == NULL)
        {
            Py_DECREF(ptList);
            return NULL;
        }
        PyStructSequence_SetItem(ptSample, 0, PyFloat_FromDouble(atSamples[i].dStartTime));
        PyStructSequence_SetItem(ptSample, 1, PyFloat_FromDouble(atSamples[i].dDuration));
        PyStructSequence_SetItem(ptSample, 2, PyUnicode_FromString(atSamples[i].pcName ? atSamples[i].pcName : ""));
        PyStructSequence_SetItem(ptSample, 3, PyLong_FromUnsignedLong(atSamples[i]._uDepth));
        PyList_SET_ITEM(ptList, i, ptSample);
    }
    if(PyErr_Occurred())
    {
        Py_DECREF(ptList);
        return NULL;
    }
    return ptList;
}

static PyObject*
profile_get_last_frame_overhead(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"threadIndex", NULL};
    unsigned int uThreadIndex = 0;
    if(!pl_parse_args("I", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uThreadIndex))
        return NULL;
    if(!pl__profile_check_thread(uThreadIndex))
        return NULL;
    return PyFloat_FromDouble(gptProfile->get_last_frame_overhead(uThreadIndex));
}

//-----------------------------------------------------------------------------
// [SECTION] method table
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplProfileI[] =
{
    {"begin_sample",            (PyCFunction)profile_begin_sample,            METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"end_sample",              (PyCFunction)profile_end_sample,              METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_last_frame_samples",  (PyCFunction)profile_get_last_frame_samples,  METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_last_frame_overhead", (PyCFunction)profile_get_last_frame_overhead, METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plProfileI)
