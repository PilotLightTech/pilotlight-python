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
plScreenLogI_clear(PyObject* self)
{
    gptScreenLog->clear();
    Py_RETURN_NONE;
}

PyObject*
plScreenLogI_add_message(PyObject* self, PyObject* args)
{
    static const char* apcKeywords[] = {
        "time_to_display",
        "message",
        NULL,
    };

    double dTimeToDisplay = 0.0;
    const char* pcMessage = NULL;
	if (!pl_parse("ds", (const char**)apcKeywords, args, NULL, __FUNCTION__,
        &dTimeToDisplay, &pcMessage))
		return NULL;

    gptScreenLog->add_message(dTimeToDisplay, pcMessage);
    Py_RETURN_NONE;
}
