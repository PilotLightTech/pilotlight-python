/*
   pl_python.h
*/

/*
Index of this file:
// [SECTION] header mess
// [SECTION] includes
// [SECTION] forward declarations
// [SECTION] public api
// [SECTION] structs
// [SECTION] enums
*/

//-----------------------------------------------------------------------------
// [SECTION] header mess
//-----------------------------------------------------------------------------

#ifndef PL_PYTHON_H
#define PL_PYTHON_H

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>

//-----------------------------------------------------------------------------
// [SECTION] forward declarations
//-----------------------------------------------------------------------------

// basic types
typedef struct _plPythonArgument     plPythonArgument;
typedef struct _plPythonArgumentPack plPythonArgumentPack;

// enums
typedef int plPyDataType;
typedef int plPyArgType;

// external
typedef struct _object PyObject;

//-----------------------------------------------------------------------------
// [SECTION] public api
//-----------------------------------------------------------------------------

bool pl_parse             (char* formatstring, char* const* keywords, PyObject* args, PyObject* kwargs, const char* message, ...);
void pl_create_parser_pack(plPythonArgument* argList, uint32_t count, plPythonArgumentPack* out);

//-----------------------------------------------------------------------------
// [SECTION] structs
//-----------------------------------------------------------------------------

typedef struct _plPythonArgument
{
    plPyDataType tType;
    plPyArgType  tArgType;
    const char*  pcName;
    const char*  pcDefaultValue;
    const char*  pcDescription;
} plPythonArgument;

typedef struct _plPythonArgumentPack
{
    plPythonArgument* sbtRequiredElements;
    plPythonArgument* sbtOptionalElements;
    plPythonArgument* sbtKeywordElements;
    char*             sbcFormatString;
    const char*       pcAbout;
    char*             sbuDocumentation;
    plPyDataType      tReturnType;
    const char**      sbtKeywords; // must end in null
} plPythonArgumentPack;

//-----------------------------------------------------------------------------
// [SECTION] enums
//-----------------------------------------------------------------------------

enum _plPyArgType
{
    PL_PYTHON_ARG_TYPE_REQUIRED = 0,
    PL_PYTHON_ARG_TYPE_POSITIONAL,
    PL_PYTHON_ARG_TYPE_KEYWORD,
};

enum _plPyDataType
{
    PL_PYTHON_DATA_TYPE_NONE = 0,
    PL_PYTHON_DATA_TYPE_INTEGER,
    PL_PYTHON_DATA_TYPE_FLOAT,
    PL_PYTHON_DATA_TYPE_DOUBLE,
    PL_PYTHON_DATA_TYPE_STRING,
    PL_PYTHON_DATA_TYPE_BOOL,
    PL_PYTHON_DATA_TYPE_OBJECT,
    PL_PYTHON_DATA_TYPE_CALLABLE,
    PL_PYTHON_DATA_TYPE_DICT,
    PL_PYTHON_DATA_TYPE_INTEGER_LIST,
    PL_PYTHON_DATA_TYPE_FLOAT_LIST,
    PL_PYTHON_DATA_TYPE_DOUBLE_LIST,
    PL_PYTHON_DATA_TYPE_STRING_LIST,
    PL_PYTHON_DATA_TYPE_LIST_ANY,
    PL_PYTHON_DATA_TYPE_LIST_LIST_INT,
    PL_PYTHON_DATA_TYPE_LIST_FLOAT_LIST,
    PL_PYTHON_DATA_TYPE_LIST_DOUBLE_LIST,
    PL_PYTHON_DATA_TYPE_LIST_STRING_LIST,
    PL_PYTHON_DATA_TYPE_LONG,
    PL_PYTHON_DATA_TYPE_ANY
};

#endif // PL_PYTHON_H