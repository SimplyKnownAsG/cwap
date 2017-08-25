#include <Python.h>

% for func in project.functions:
    static PyObject*
    ${func.name}(PyObject* self, PyObject* args) {
        % for param in func.parameters:
            const ${param.cwap_type} ${param.name}
            if (!PyArg_ParseTuple(args, "s", &${param.name})) {
                return NULL;
            }
        % endfor

        % if func.return_type is not None:
            return Py_RETURN_NONE;
        % else:
            Py_RETURN_NONE;
        % endif
    }
% endfor

static PyMethodDef ${project.name}Methods[] = {
    % for func in project.functions:
        {"${func.name}", ${func.name}, METH_VARARGS, "${func.name}"},
    % endfor
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
init${project.name}(void) {
     (void) Py_InitModule("${project.name}", ${project.name}Methods);
}

