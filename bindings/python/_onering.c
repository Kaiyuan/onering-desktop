#include <onering.h>
#include <Python.h>

static PyObject *OneRingError;
static PyObject *g_appfuncs = NULL;  /* a dict */

static onering_response_handle_t
c_appfunc(const char* appname, const char* method, const char* path, const
		char* body, const char **response, int *response_len)
{
	int code = 0;
	PyObject *args, *py_response=NULL, *appfunc;
	PyGILState_STATE gstate;

	gstate = PyGILState_Ensure();

	*response = "";
	*response_len = 0;

	if (!g_appfuncs) {
		code = 404;
		goto exit;
	}

	appfunc = PyDict_GetItemString(g_appfuncs, appname);
	if (!appfunc) {
		code = 404;
		goto exit;
	}

	args = Py_BuildValue("ssz", method, path, body);
	if (!args) {
		code = 400;
		goto exit;
	}

	py_response = PyObject_CallObject(appfunc, args);
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
	PyGILState_Release(gstate);
	return (onering_response_handle_t)py_response;
}

static void
free_response(const char *appname, onering_response_handle_t response_handle)
{
	PyObject *py_response = (PyObject*)response_handle;
	PyGILState_STATE gstate;

	gstate = PyGILState_Ensure();

	if (py_response) {
		Py_DECREF(py_response);
	}

	PyGILState_Release(gstate);
}

static PyObject *
register_app(PyObject *self, PyObject *args)
{
	const char *appname;
	PyObject *appfunc, *oldappfunc;
	int err;

	if (!PyArg_ParseTuple(args, "sO:register_app", &appname, &appfunc))
		return NULL;

	if (!PyCallable_Check(appfunc)) {
		PyErr_SetString(PyExc_TypeError, "app must be callable");
		return NULL;
	}

	if (!g_appfuncs) {
		g_appfuncs = PyDict_New();
	}

	oldappfunc = PyDict_GetItemString(g_appfuncs, appname);

	if (PyDict_SetItemString(g_appfuncs, appname, appfunc) == -1) {
		return NULL;
	}

	err = onering_register_app(appname, &c_appfunc, &free_response);
	if (err) {
		if (oldappfunc) {
			PyDict_SetItemString(g_appfuncs, appname, oldappfunc);
		} else {
			PyDict_DelItemString(g_appfuncs, appname);
		}
		return PyErr_Format(OneRingError,
				"onering_register_app() returns %d", err);
	}

	Py_XDECREF(oldappfunc);
	Py_INCREF(appfunc);

	Py_RETURN_NONE;
}

static PyObject *
loop(PyObject *self, PyObject *args)
{
	const char *appname;
	int err;

	if (!PyArg_ParseTuple(args, "s:loop", &appname))
		return NULL;

	Py_BEGIN_ALLOW_THREADS
	err = onering_loop(appname);
	Py_END_ALLOW_THREADS
	if (err) {
		return PyErr_Format(OneRingError,
			       	"onering_loop() returns %d", err);
	}
	Py_RETURN_NONE;
}

static PyObject *
publish(PyObject *self, PyObject *args)
{
	const char *channel;
	const char *msg;

	if (!PyArg_ParseTuple(args, "ss", &channel, &msg))
		return NULL;

	Py_BEGIN_ALLOW_THREADS
	onering_publish(channel, msg);
	Py_END_ALLOW_THREADS

	Py_RETURN_NONE;
}

static PyMethodDef OneRingMethods[] = {
	{"register_app", register_app, METH_VARARGS,
	       	"register a app callback to onering framework"}, 
	{"loop", loop, METH_VARARGS, "loop"},
	{"publish", publish, METH_VARARGS,
	       	"publish a message to onering pages"},
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

