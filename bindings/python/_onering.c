#include <onering.h>
#include <Python.h>

static PyObject *OneRingError;
static PyObject *g_appfunc = NULL;

static onering_response_handle_t
c_appfunc(const char* method, const char* path, const char* body,
		const char **response, int *response_len)
{
	int code = 0;
	PyObject *args, *py_response=NULL;

	*response = "";
	*response_len = 0;

	if (!g_appfunc) {
		code = 404;
		goto exit;
	}

	args = Py_BuildValue("ssz", method, path, body);
	if (!args) {
		code = 400;
		goto exit;
	}

	py_response = PyObject_CallObject(g_appfunc, args);
	if (!py_response) {
		code = 500;
		goto exit_args;
	}

	/* don't know why PyString_AsStringAndSize not work on Snow Leopard */
	*response = PyString_AsString(py_response);
	*response_len = PyString_Size(py_response);

exit_args:
	Py_DECREF(args);
exit:
	return (onering_response_handle_t)py_response;
}

static void
free_response(onering_response_handle_t response_handle)
{
	PyObject *py_response = (PyObject*)response_handle;

	if (py_response) {
		Py_DECREF(py_response);
	}
}

static PyObject *
register_app(PyObject *self, PyObject *args)
{
	const char *appname;
	PyObject *appfunc;
	int err;

	if (!PyArg_ParseTuple(args, "sO", &appname, &appfunc))
		return NULL;

	if (g_appfunc) {
		Py_DECREF(g_appfunc);
	}
	g_appfunc = appfunc;
	Py_INCREF(g_appfunc);

	err = onering_register_app(appname, &c_appfunc, &free_response);
	if (err) {
		g_appfunc = NULL;
		Py_DECREF(g_appfunc);
		return PyErr_Format(OneRingError,
				"onering_register_app() returns %d", err);
	}

	Py_RETURN_NONE;
}

static PyObject *
loop(PyObject *self, PyObject *args)
{
	const char *appname;
	int err;

	if (!PyArg_ParseTuple(args, "s", &appname))
		return NULL;

	err = onering_loop(appname);
	if (err) {
		return PyErr_Format(OneRingError,
			       	"onering_loop() returns %d", err);
	}
	Py_RETURN_NONE;
}

static PyMethodDef OneRingMethods[] = {
	{"register_app", register_app, METH_VARARGS,
	       	"register a app callback to onering framework"}, 
	{"loop", loop, METH_VARARGS, "loop"},
	{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
init_onering(void)
{
	PyObject *m;

	m = Py_InitModule("_onering", OneRingMethods);
	if (m == NULL)
		return;

	OneRingError = PyErr_NewException("onering.error", NULL, NULL);
	Py_INCREF(OneRingError);
	PyModule_AddObject(m, "error", OneRingError);
}

