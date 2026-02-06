/*
   pl_python.c
*/

/*
Index of this file:
// [SECTION] includes
// [SECTION] internal api
// [SECTION] public api implementation
// [SECTION] internal api implementation
*/

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdarg.h>  // arg vars
#include <stdio.h>

#include "pl_python.h"
#include "pl_ds.h"

//-----------------------------------------------------------------------------
// [SECTION] internal api
//-----------------------------------------------------------------------------

static char        pl_get_argument_symbol   (plPyDataType);
static const char* pl_get_type_string_actual(plPyDataType);
static const char* pl_get_type_string       (plPyDataType);

//-----------------------------------------------------------------------------
// [SECTION] public api implementation
//-----------------------------------------------------------------------------

bool
pl_parse(char* formatstring, char* const* keywords, PyObject* args, PyObject* kwargs, const char* message, ...)
{

    bool check = true;

    va_list arguments;
    va_start(arguments, message);
    if (!PyArg_VaParseTupleAndKeywords(args, kwargs, formatstring, keywords, arguments))
    {
        check = false;
    }

    va_end(arguments);

    // if (!check)
    //     mvThrowPythonError(mvErrorCode::mvNone, "Error parsing Dear PyGui command: " + std::string(message));

    return check;
}

void
pl_create_parser_pack(plPythonArgument* atArguments, uint32_t uArgumentCount, plPythonArgumentPack* ptPackOut)
{
    for(uint32_t i = 0; i < uArgumentCount; i++)
    {
        switch (atArguments[i].tArgType)
        {
            case PL_PYTHON_ARG_TYPE_REQUIRED:   pl_sb_push(ptPackOut->sbtRequiredElements, atArguments[i]); break;
            case PL_PYTHON_ARG_TYPE_POSITIONAL: pl_sb_push(ptPackOut->sbtOptionalElements, atArguments[i]); break;
            case PL_PYTHON_ARG_TYPE_KEYWORD:    pl_sb_push(ptPackOut->sbtKeywordElements,  atArguments[i]); break;
            default: break;
        }
    }

    // build format string and keywords

    for (uint32_t i = 0; i < pl_sb_size(ptPackOut->sbtRequiredElements); i++)
    {
        pl_sb_push(ptPackOut->sbcFormatString, pl_get_argument_symbol(ptPackOut->sbtRequiredElements[i].tType));
        pl_sb_push(ptPackOut->sbtKeywords, ptPackOut->sbtRequiredElements[i].pcName);
    }

    pl_sb_push(ptPackOut->sbcFormatString, '|');

    for (uint32_t i = 0; i < pl_sb_size(ptPackOut->sbtOptionalElements); i++)
    {
        pl_sb_push(ptPackOut->sbcFormatString, pl_get_argument_symbol(ptPackOut->sbtOptionalElements[i].tType));
        pl_sb_push(ptPackOut->sbtKeywords, ptPackOut->sbtOptionalElements[i].pcName);
    }

    for (uint32_t i = 0; i < pl_sb_size(ptPackOut->sbtKeywordElements); i++)
    {
        pl_sb_push(ptPackOut->sbcFormatString, '$');
        pl_sb_push(ptPackOut->sbcFormatString, pl_get_argument_symbol(ptPackOut->sbtKeywordElements[i].tType));
        pl_sb_push(ptPackOut->sbtKeywords, ptPackOut->sbtKeywordElements[i].pcName);
    }
    pl_sb_push(ptPackOut->sbcFormatString, 0);
    pl_sb_push(ptPackOut->sbtKeywords, NULL);

    // build documentation
    pl_sb_sprintf(ptPackOut->sbuDocumentation, "%s \n\nReturn Type: %s\n", ptPackOut->pcAbout, pl_get_type_string_actual(ptPackOut->tReturnType));

    if (pl_sb_size(ptPackOut->sbtRequiredElements) > 0)
        pl_sb_sprintf(ptPackOut->sbuDocumentation, "%s", "\n\nRequired Arguments\n_______________\n\n");

    for (uint32_t i = 0; i < pl_sb_size(ptPackOut->sbtRequiredElements); i++)
    {
        plPythonArgument* ptArg = &ptPackOut->sbtRequiredElements[i];
        pl_sb_sprintf(ptPackOut->sbuDocumentation, "%s", "\n* ");
        pl_sb_sprintf(ptPackOut->sbuDocumentation, "%s %s", ptArg->pcName, pl_get_type_string(ptArg->tType));
        pl_sb_sprintf(ptPackOut->sbuDocumentation, "\n\t\t%s", "", ptArg->pcDescription);
    }

    if (pl_sb_size(ptPackOut->sbtOptionalElements) > 0)
        pl_sb_sprintf(ptPackOut->sbuDocumentation, "%s", "\n\nOptional Arguments\n_______________\n\n");

    for (uint32_t i = 0; i < pl_sb_size(ptPackOut->sbtOptionalElements); i++)
    {
        plPythonArgument* ptArg = &ptPackOut->sbtOptionalElements[i];
        pl_sb_sprintf(ptPackOut->sbuDocumentation, "%s", "\n* ");
        pl_sb_sprintf(ptPackOut->sbuDocumentation, "%s %s", ptArg->pcName, pl_get_type_string(ptArg->tType));
        pl_sb_sprintf(ptPackOut->sbuDocumentation, " = %s", ptArg->pcDefaultValue);
        pl_sb_sprintf(ptPackOut->sbuDocumentation, "\n\t\t%s", "", ptArg->pcDescription);
    }

    if (pl_sb_size(ptPackOut->sbtKeywordElements) > 0)
        pl_sb_sprintf(ptPackOut->sbuDocumentation, "%s", "\n\nKeyword Arguments\n_______________\n\n");

    for (uint32_t i = 0; i < pl_sb_size(ptPackOut->sbtKeywordElements); i++)
    {
        plPythonArgument* ptArg = &ptPackOut->sbtKeywordElements[i];
        pl_sb_sprintf(ptPackOut->sbuDocumentation, "%s", "\n* ");
        pl_sb_sprintf(ptPackOut->sbuDocumentation, "%s %s", ptArg->pcName, pl_get_type_string(ptArg->tType));
        pl_sb_sprintf(ptPackOut->sbuDocumentation, " = %s", ptArg->pcDefaultValue);
        pl_sb_sprintf(ptPackOut->sbuDocumentation, "\n\t\t%s", "", ptArg->pcDescription);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] internal api implementation
//-----------------------------------------------------------------------------

static char
pl_get_argument_symbol(plPyDataType tType)
{
    switch (tType)
    {
        case PL_PYTHON_DATA_TYPE_LONG:    return 'l';
        case PL_PYTHON_DATA_TYPE_STRING:  return 's';
        case PL_PYTHON_DATA_TYPE_INTEGER: return 'i';
        case PL_PYTHON_DATA_TYPE_FLOAT:   return 'f';
        case PL_PYTHON_DATA_TYPE_DOUBLE:  return 'd';
        case PL_PYTHON_DATA_TYPE_BOOL:    return 'p';
        default:                          return 'O';
    }
}

static const char*
pl_get_type_string_actual(plPyDataType tType)
{
    switch (tType)
    {
        case PL_PYTHON_DATA_TYPE_STRING:           return "str";
        case PL_PYTHON_DATA_TYPE_INTEGER:          return "int";
        case PL_PYTHON_DATA_TYPE_LONG:             return "int";
        case PL_PYTHON_DATA_TYPE_FLOAT:            return "float";
        case PL_PYTHON_DATA_TYPE_DOUBLE:           return "float";
        case PL_PYTHON_DATA_TYPE_BOOL:             return "bool";
        case PL_PYTHON_DATA_TYPE_STRING_LIST:      return "Union[List[str], Tuple[str, ...]]";
        case PL_PYTHON_DATA_TYPE_FLOAT_LIST:       return "Union[List[float], Tuple[float, ...]]";
        case PL_PYTHON_DATA_TYPE_INTEGER_LIST:     return "Union[List[int], Tuple[int, ...]]";
        case PL_PYTHON_DATA_TYPE_CALLABLE:         return "Callable";
        case PL_PYTHON_DATA_TYPE_DICT:             return "dict";
        case PL_PYTHON_DATA_TYPE_LIST_FLOAT_LIST:  return "List[List[float]]";
        case PL_PYTHON_DATA_TYPE_LIST_STRING_LIST: return "List[List[str]]";
        case PL_PYTHON_DATA_TYPE_NONE:             return "None";
        case PL_PYTHON_DATA_TYPE_OBJECT:           return "Any";
        default:                                   return "Any";
    }
}

static const char*
pl_get_type_string(plPyDataType tType)
{
    switch (tType)
    {
        case PL_PYTHON_DATA_TYPE_STRING:           return " : str";
        case PL_PYTHON_DATA_TYPE_INTEGER:          return " : int";
        case PL_PYTHON_DATA_TYPE_LONG:             return " : int";
        case PL_PYTHON_DATA_TYPE_FLOAT:            return " : float";
        case PL_PYTHON_DATA_TYPE_DOUBLE:           return " : float";
        case PL_PYTHON_DATA_TYPE_BOOL:             return " : bool";
        case PL_PYTHON_DATA_TYPE_STRING_LIST:      return " : Union[List[str], Tuple[str, ...]]";
        case PL_PYTHON_DATA_TYPE_FLOAT_LIST:       return " : Union[List[float], Tuple[float, ...]]";
        case PL_PYTHON_DATA_TYPE_DOUBLE_LIST:      return " : Union[List[float], Tuple[float, ...]]";
        case PL_PYTHON_DATA_TYPE_INTEGER_LIST:     return " : Union[List[int], Tuple[int, ...]]";
        case PL_PYTHON_DATA_TYPE_CALLABLE:         return " : Callable";
        case PL_PYTHON_DATA_TYPE_DICT:             return " : dict";
        case PL_PYTHON_DATA_TYPE_LIST_ANY:         return " : List[Any]";
        case PL_PYTHON_DATA_TYPE_LIST_LIST_INT:    return " : List[Union[List[int], Tuple[int, ...]]]";
        case PL_PYTHON_DATA_TYPE_LIST_FLOAT_LIST:  return " : List[List[float]]";
        case PL_PYTHON_DATA_TYPE_LIST_DOUBLE_LIST: return " : List[List[float]]";
        case PL_PYTHON_DATA_TYPE_LIST_STRING_LIST: return " : List[List[str]]";
        case PL_PYTHON_DATA_TYPE_OBJECT:           return " : Any";
        default:                                   return " : unknown";
    }
}