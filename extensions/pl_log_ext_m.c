/*
   pl_log_ext_m.c
     * plLogI (log extension)
     * differences from the C api:
         - messages are already formatted python strs (the *_p & *_va
           variants are not bound)
         - get_channel_id returns None for unknown channels
         - get_channel_info returns a plLogChannelInfo (struct sequence) with
           the channel's entries as (level, text) tuples, oldest first
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] python types
// [SECTION] channels
// [SECTION] messages
// [SECTION] file sinks
// [SECTION] method table
// [SECTION] constants
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "pilotlight.h"

//-----------------------------------------------------------------------------
// [SECTION] python types
//-----------------------------------------------------------------------------

static PyTypeObject* gptLogChannelInfoType = NULL;

static PyStructSequence_Field gatLogChannelInfoFields[] = {
    {"uID",            "channel id"},
    {"tType",          "plLogChannelType flags"},
    {"pcName",         "channel name"},
    {"uEntryCount",    "entries logged (cyclic channels keep the last uEntryCapacity)"},
    {"uEntryCapacity", "entry capacity"},
    {"entries",        "list of (level, text) for buffer channels, oldest first; empty for console-only channels"},
    {NULL, NULL}
};

static PyStructSequence_Desc gtLogChannelInfoDesc = {
    "pilotlight.plLogChannelInfo",
    "information about a log channel (plLogExtChannelInfo)",
    gatLogChannelInfoFields,
    6
};

// called from PyInit_pilotlight
static int
pl__log_add_python_types(PyObject* ptModule)
{
    gptLogChannelInfoType = PyStructSequence_NewType(&gtLogChannelInfoDesc);
    if(gptLogChannelInfoType == NULL)
        return -1;
    Py_INCREF(gptLogChannelInfoType);
    if(PyModule_AddObject(ptModule, "plLogChannelInfo", (PyObject*)gptLogChannelInfoType) < 0)
        return -1;

    // 64 bit constants (PyModule_AddIntConstant takes a long, 32 bit on windows)
    if(PyModule_AddObject(ptModule, "PL_LOG_ALL_CHANNELS", PyLong_FromUnsignedLongLong(PL_LOG_ALL_CHANNELS)) < 0)
        return -1;
    return 0;
}

//-----------------------------------------------------------------------------
// [SECTION] channels
//-----------------------------------------------------------------------------

static PyObject*
log_add_channel(PyObject* self, PyObject* args, PyObject* kwargs)
{
    // init: plLogExtChannelInit (tType, uEntryCount) or None for the defaults
    static const char* apcKeywords[] = {"name", "init", NULL};
    const char* pcName = NULL; // copied by the log
    PyObject* ptInit = Py_None;
    if(!pl_parse_args("s|O", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcName, &ptInit))
        return NULL;

    plLogExtChannelInit tInit = {0};
    if(ptInit != Py_None)
    {
        PyObject* ptType = PyObject_GetAttrString(ptInit, "tType");
        PyObject* ptCount = ptType ? PyObject_GetAttrString(ptInit, "uEntryCount") : NULL;
        if(ptType == NULL || ptCount == NULL)
        {
            Py_XDECREF(ptType);
            return NULL;
        }
        tInit.tType = (plLogChannelType)PyLong_AsLong(ptType);
        tInit.uEntryCount = (uint64_t)PyLong_AsUnsignedLongLong(ptCount);
        Py_DECREF(ptType);
        Py_DECREF(ptCount);
        if(PyErr_Occurred())
            return NULL;
    }

    return PyLong_FromUnsignedLongLong(gptLog->add_channel(pcName, tInit));
}

static PyObject*
log_set_level(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"channelId", "level", NULL};
    unsigned long long uChannel = 0;
    unsigned long long uLevel = 0;
    if(!pl_parse_args("KK", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uChannel, &uLevel))
        return NULL;
    gptLog->set_level(uChannel, uLevel);
    Py_RETURN_NONE;
}

static PyObject*
log_clear_channel(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"channelId", NULL};
    unsigned long long uChannel = 0;
    if(!pl_parse_args("K", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uChannel))
        return NULL;
    gptLog->clear_channel(uChannel);
    Py_RETURN_NONE;
}

static PyObject*
log_reset_channel(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"channelId", NULL};
    unsigned long long uChannel = 0;
    if(!pl_parse_args("K", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uChannel))
        return NULL;
    gptLog->reset_channel(uChannel);
    Py_RETURN_NONE;
}

static PyObject*
log_get_channel_id(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"name", NULL};
    const char* pcName = NULL;
    if(!pl_parse_args("s", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcName))
        return NULL;
    const uint64_t uId = gptLog->get_channel_id(pcName);
    if(uId >= gptLog->get_channel_count())
        Py_RETURN_NONE;
    return PyLong_FromUnsignedLongLong(uId);
}

static PyObject*
log_get_channel_info(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* apcKeywords[] = {"channelId", NULL};
    unsigned long long uChannel = 0;
    if(!pl_parse_args("K", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uChannel))
        return NULL;

    plLogExtChannelInfo tInfo = {0};
    if(!gptLog->get_channel_info(uChannel, &tInfo))
        Py_RETURN_NONE;

    // entries (same ordering as the log tool window)
    uint64_t uCount = 0;
    uint64_t uStart = 0;
    if(tInfo.ptEntries && tInfo.pcBuffer)
    {
        if(tInfo.tType & PL_LOG_CHANNEL_TYPE_CYCLIC_BUFFER)
        {
            uCount = tInfo.uEntryCount < tInfo.uEntryCapacity ? tInfo.uEntryCount : tInfo.uEntryCapacity;
            uStart = uCount ? tInfo.uEntryCount % uCount : 0;
        }
        else if(tInfo.tType & PL_LOG_CHANNEL_TYPE_BUFFER)
            uCount = tInfo.uEntryCount;
    }
    PyObject* ptEntries = PyList_New((Py_ssize_t)uCount);
    if(ptEntries == NULL)
        return NULL;
    for(uint64_t i = 0; i < uCount; i++)
    {
        const plLogExtEntry* ptEntry = &tInfo.ptEntries[(uStart + i) % uCount];
        PyObject* ptEntryTuple = Py_BuildValue("(Ks)", (unsigned long long)ptEntry->uLevel, &tInfo.pcBuffer[ptEntry->uOffset]);
        if(ptEntryTuple == NULL)
        {
            Py_DECREF(ptEntries);
            return NULL;
        }
        PyList_SET_ITEM(ptEntries, (Py_ssize_t)i, ptEntryTuple);
    }

    PyObject* ptResult = PyStructSequence_New(gptLogChannelInfoType);
    if(ptResult == NULL)
    {
        Py_DECREF(ptEntries);
        return NULL;
    }
    PyStructSequence_SetItem(ptResult, 0, PyLong_FromUnsignedLongLong(tInfo.uID));
    PyStructSequence_SetItem(ptResult, 1, PyLong_FromLong((long)tInfo.tType));
    PyStructSequence_SetItem(ptResult, 2, PyUnicode_FromString(tInfo.pcName ? tInfo.pcName : ""));
    PyStructSequence_SetItem(ptResult, 3, PyLong_FromUnsignedLongLong(tInfo.uEntryCount));
    PyStructSequence_SetItem(ptResult, 4, PyLong_FromUnsignedLongLong(tInfo.uEntryCapacity));
    PyStructSequence_SetItem(ptResult, 5, ptEntries);
    if(PyErr_Occurred())
    {
        Py_DECREF(ptResult);
        return NULL;
    }
    return ptResult;
}

static PyObject*
log_get_channel_count(PyObject* self, PyObject* unused)
{
    return PyLong_FromUnsignedLongLong(gptLog->get_channel_count());
}

//-----------------------------------------------------------------------------
// [SECTION] messages
//-----------------------------------------------------------------------------

static PyObject*
log_custom(PyObject* self, PyObject* args, PyObject* kwargs)
{
    // prefix is written in front of the message (e.g. "[CUSTOM]")
    static const char* apcKeywords[] = {"prefix", "level", "channelId", "message", NULL};
    const char* pcPrefix = NULL;
    Py_ssize_t szPrefixSize = 0;
    unsigned long long uLevel = 0;
    unsigned long long uChannel = 0;
    const char* pcMessage = NULL;
    if(!pl_parse_args("s#KKs", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &pcPrefix, &szPrefixSize, &uLevel, &uChannel, &pcMessage))
        return NULL;
    gptLog->custom(pcPrefix, (int)szPrefixSize, uLevel, uChannel, pcMessage);
    Py_RETURN_NONE;
}

#define PL_LOG_LEVEL_BINDING(NAME) \
static PyObject* \
log_##NAME(PyObject* self, PyObject* args, PyObject* kwargs) \
{ \
    static const char* apcKeywords[] = {"channelId", "message", NULL}; \
    unsigned long long uChannel = 0; \
    const char* pcMessage = NULL; \
    if(!pl_parse_args("Ks", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uChannel, &pcMessage)) \
        return NULL; \
    gptLog->NAME(uChannel, pcMessage); \
    Py_RETURN_NONE; \
}

PL_LOG_LEVEL_BINDING(trace)
PL_LOG_LEVEL_BINDING(debug)
PL_LOG_LEVEL_BINDING(info)
PL_LOG_LEVEL_BINDING(warn)
PL_LOG_LEVEL_BINDING(error)
PL_LOG_LEVEL_BINDING(fatal)

//-----------------------------------------------------------------------------
// [SECTION] file sinks
//-----------------------------------------------------------------------------

static PyObject*
log_set_file(PyObject* self, PyObject* args, PyObject* kwargs)
{
    // path is a native filesystem path; channelId may be PL_LOG_ALL_CHANNELS
    static const char* apcKeywords[] = {"channelId", "path", "flags", NULL};
    unsigned long long uChannel = 0;
    const char* pcPath = NULL;
    int iFlags = 0;
    if(!pl_parse_args("Ks|i", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uChannel, &pcPath, &iFlags))
        return NULL;
    return PyBool_FromLong(gptLog->set_file(uChannel, pcPath, iFlags));
}

#define PL_LOG_FILE_BINDING(NAME) \
static PyObject* \
log_##NAME(PyObject* self, PyObject* args, PyObject* kwargs) \
{ \
    static const char* apcKeywords[] = {"channelId", NULL}; \
    unsigned long long uChannel = 0; \
    if(!pl_parse_args("K", (const char**)apcKeywords, args, kwargs, __FUNCTION__, &uChannel)) \
        return NULL; \
    return PyBool_FromLong(gptLog->NAME(uChannel)); \
}

PL_LOG_FILE_BINDING(flush_file)
PL_LOG_FILE_BINDING(close_file)
PL_LOG_FILE_BINDING(has_file_error)

//-----------------------------------------------------------------------------
// [SECTION] method table
//-----------------------------------------------------------------------------

static PyMethodDef gatCommandsplLogI[] =
{
    {"add_channel",       (PyCFunction)log_add_channel,       METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_level",         (PyCFunction)log_set_level,         METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"clear_channel",     (PyCFunction)log_clear_channel,     METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"reset_channel",     (PyCFunction)log_reset_channel,     METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_channel_id",    (PyCFunction)log_get_channel_id,    METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_channel_info",  (PyCFunction)log_get_channel_info,  METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"get_channel_count", (PyCFunction)log_get_channel_count, METH_NOARGS | METH_STATIC, NULL},
    {"custom",            (PyCFunction)log_custom,            METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"trace",             (PyCFunction)log_trace,             METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"debug",             (PyCFunction)log_debug,             METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"info",              (PyCFunction)log_info,              METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"warn",              (PyCFunction)log_warn,              METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"error",             (PyCFunction)log_error,             METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"fatal",             (PyCFunction)log_fatal,             METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"set_file",          (PyCFunction)log_set_file,          METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"flush_file",        (PyCFunction)log_flush_file,        METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"close_file",        (PyCFunction)log_close_file,        METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {"has_file_error",    (PyCFunction)log_has_file_error,    METH_VARARGS | METH_KEYWORDS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

PL_NEW_PYTHON_API(plLogI)

//-----------------------------------------------------------------------------
// [SECTION] constants
//-----------------------------------------------------------------------------

plPythonIntConstantPair gatLogIntPairs[] = {

    // levels
    PL_ADD_INT_CONSTANT(PL_LOG_LEVEL_ALL),
    PL_ADD_INT_CONSTANT(PL_LOG_LEVEL_TRACE),
    PL_ADD_INT_CONSTANT(PL_LOG_LEVEL_DEBUG),
    PL_ADD_INT_CONSTANT(PL_LOG_LEVEL_INFO),
    PL_ADD_INT_CONSTANT(PL_LOG_LEVEL_WARN),
    PL_ADD_INT_CONSTANT(PL_LOG_LEVEL_ERROR),
    PL_ADD_INT_CONSTANT(PL_LOG_LEVEL_FATAL),
    PL_ADD_INT_CONSTANT(PL_LOG_LEVEL_OFF),

    // plLogChannelType
    PL_ADD_INT_CONSTANT(PL_LOG_CHANNEL_TYPE_DEFAULT),
    PL_ADD_INT_CONSTANT(PL_LOG_CHANNEL_TYPE_CONSOLE),
    PL_ADD_INT_CONSTANT(PL_LOG_CHANNEL_TYPE_BUFFER),
    PL_ADD_INT_CONSTANT(PL_LOG_CHANNEL_TYPE_CYCLIC_BUFFER),

    // plLogFileFlags
    PL_ADD_INT_CONSTANT(PL_LOG_FILE_FLAGS_NONE),
    PL_ADD_INT_CONSTANT(PL_LOG_FILE_FLAGS_APPEND),
    PL_ADD_INT_CONSTANT(PL_LOG_FILE_FLAGS_FLUSH_EACH_ENTRY),
};
