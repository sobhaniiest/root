#ifndef CPYCPPYY_PROXYWRAPPERS_H
#define CPYCPPYY_PROXYWRAPPERS_H

// Standard
#include <string>


namespace CPyCppyy {

// construct a Python shadow class for the named C++ class
PyObject* GetScopeProxy(Cppyy::TCppScope_t);
PyObject* CreateScopeProxy(Cppyy::TCppScope_t);
PyObject* CreateScopeProxy(PyObject*, PyObject* args);
PyObject* CreateScopeProxy(
    const std::string& scope_name, PyObject* parent = nullptr);

// bind a C++ object into a Python proxy object (flags are CPPInstance::Default)
PyObject* BindCppObjectNoCast(Cppyy::TCppObject_t object,
    Cppyy::TCppType_t klass, int flags = 0x0001);
PyObject* BindCppObject(Cppyy::TCppObject_t object,
    Cppyy::TCppType_t klass, int flags = 0x0001);
PyObject* BindCppObjectArray(
    Cppyy::TCppObject_t address, Cppyy::TCppType_t klass, Py_ssize_t* dims);

} // namespace CPyCppyy

#endif // !CPYCPPYY_PROXYWRAPPERS_H
