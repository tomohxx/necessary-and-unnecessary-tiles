#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "calsht_dw.hpp"
#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)
namespace py = pybind11;

PYBIND11_MODULE(necessary_and_unnecessary_tiles, m)
{
  py::class_<CalshtDW>(m, "CalshtDW")
      .def(py::init<>())
      .def("initialize", &CalshtDW::initialize)
      .def("__call__", &CalshtDW::operator());

#ifdef VERSION_INFO
  m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
  m.attr("__version__") = "dev";
#endif
}
