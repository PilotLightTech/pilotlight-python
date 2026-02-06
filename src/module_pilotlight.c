/*
   module_pilotlight.c
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] global data
// [SECTION] python api
// [SECTION] public api
// [SECTION] unity build
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include "module_pilotlight.h"
#include "pl_ds.h"
#include "pl_python.h"

//-----------------------------------------------------------------------------
// [SECTION] global data
//-----------------------------------------------------------------------------

plPythonArgumentPack* sbtParserPacks = NULL;

//-----------------------------------------------------------------------------
// [SECTION] python api
//-----------------------------------------------------------------------------

PyObject*
pilotlight_func0(PyObject* self, PyObject* args, PyObject* kwargs)
{
    printf("func 0 ran\n");
    Py_RETURN_NONE;
}

PyObject*
pilotlight_func1(PyObject* self, PyObject* args, PyObject* kwargs)
{
    return PyFloat_FromDouble(43.2);
}

PyObject*
pilotlight_func2(PyObject* self, PyObject* args, PyObject* kwargs)
{
    double x = 0.0;
    double y = 0.0;

	if (!pl_parse(sbtParserPacks[0].sbcFormatString, (char**)sbtParserPacks[0].sbtKeywords, args, kwargs, "func2", &x, &y))
		return NULL;

    return PyFloat_FromDouble(x + y);
}

//-----------------------------------------------------------------------------
// [SECTION] public api
//-----------------------------------------------------------------------------

PyMODINIT_FUNC
PyInit__pilotlight(void)
{
    {
        plPythonArgument atArgs[] = {
            { PL_PYTHON_DATA_TYPE_DOUBLE, 0, "input"},
            { PL_PYTHON_DATA_TYPE_DOUBLE, 0, "input2"},
        };

        plPythonArgumentPack tParserPack = {0};
        pl_create_parser_pack(atArgs, 2, &tParserPack);
        tParserPack.pcAbout     = "Testing Documentation";
        tParserPack.tReturnType = PL_PYTHON_DATA_TYPE_DOUBLE;
        pl_sb_push(sbtParserPacks, tParserPack);
    }

    static PyMethodDef atMethods[] = {
        {.ml_name = "func0", .ml_meth = (PyCFunction)pilotlight_func0, .ml_flags = METH_VARARGS | METH_KEYWORDS, .ml_doc = "test docs"},
        {.ml_name = "func1", .ml_meth = (PyCFunction)pilotlight_func1, .ml_flags = METH_VARARGS | METH_KEYWORDS, .ml_doc = "test docs"},
        {.ml_name = "func2", .ml_meth = (PyCFunction)pilotlight_func2, .ml_flags = METH_VARARGS | METH_KEYWORDS, .ml_doc = "test docs"},
        {NULL, NULL, 0, NULL}
    };

	static PyModuleDef tPilotLightModule = {
		PyModuleDef_HEAD_INIT,
        "_pilotlight",
        NULL,
        -1,
        atMethods,
		NULL,
        NULL,
        NULL,
        NULL
	};

	PyObject* ptModule = PyModule_Create(&tPilotLightModule);
	if (ptModule == NULL)
    {
		return NULL;
    }

    return ptModule;
}

//-----------------------------------------------------------------------------
// [SECTION] unity build
//-----------------------------------------------------------------------------

#include "pl_python.c"