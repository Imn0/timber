#include <Python.h>
#include <tmb.h>

static PyObject* _hello_world(PyObject* self) {
    return PyUnicode_FromString("hello world");
}

static PyObject* _test_tmb(PyObject* self) {
    tmb_test();
    return PyUnicode_FromString("hello world");

}

static struct PyMethodDef methods[] = {
    { "hello_world", (PyCFunction)_hello_world, METH_NOARGS },
    { "test_tmb", (PyCFunction)_test_tmb, METH_NOARGS },
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
