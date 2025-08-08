#include <Python.h>
#include <tmb.h>

static PyObject* py_tmb_print_version_(PyObject* self) {
    tmb_print_version();
    return Py_None;
}

static struct PyMethodDef methods[] = {
    { "print_version", (PyCFunction)py_tmb_print_version_, METH_NOARGS },
    { NULL, NULL }
};

static struct PyModuleDef module = { PyModuleDef_HEAD_INIT,
                                     "_timber",
                                     "",
                                     -1,
                                     methods };

PyMODINIT_FUNC PyInit__timber(void) {
    return PyModule_Create(&module);
}
