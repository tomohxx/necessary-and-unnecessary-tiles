# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension, build_ext  # type: ignore
from setuptools import setup  # type: ignore

__version__ = "1.0.0"

ext_modules = [
    Pybind11Extension(
        "necessary_and_unnecessary_tiles",
        sources=["src/calsht_dw.cpp", "src/python.cpp"],
        cxx_std=17,
        define_macros=[("VERSION_INFO", __version__)],
        extra_compile_args=["-O3"],
    ),
]

setup(
    name="necessary_and_unnecessary_tiles",
    version=__version__,
    author="tomohxx",
    description="Shanten Number",
    long_description="",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.7",
)
