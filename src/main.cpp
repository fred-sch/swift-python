#include <pybind11/pybind11.h>

namespace py = pybind11;

// C++ wrapper for the Swift function
extern "C" {
int swift_add(int a, int b);  // Declaration of the Swift function
}


PYBIND11_MODULE(_core, m) {
  m.def("add", &swift_add,
        "Add two numbers");
}
