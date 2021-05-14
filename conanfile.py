from conans import ConanFile, CMake, tools
from conans.client.tools import settings

Options = [
    # Cafe
    ("CAFE_INCLUDE_TESTS", [True, False], False),

    # Cafe.ErrorHandling
    ("CAFE_ERROR_HANDLING_INCLUDE_STACKWALKER", [True, False], False),
    ("CAFE_ERROR_HANDLING_ENABLE_STACKWALKER_IN_CAFE_EXCEPTION",
     [True, False], False),
]


class CafeErrorHandlingConan(ConanFile):
    name = "Cafe.ErrorHandling"
    version = "0.1"
    license = "MIT"
    author = "akemimadoka <chino@hotococoa.moe>"
    url = "https://github.com/akemimadoka/Cafe.ErrorHandling"
    description = "A general purpose C++ library"
    topics = "C++"
    settings = "os", "compiler", "build_type", "arch"
    options = {opt[0]: opt[1] for opt in Options}
    default_options = {opt[0]: opt[2] for opt in Options}

    requires = "Cafe.Encoding/0.1"
    python_requires = "CafeCommon/0.1"

    generators = "cmake"

    exports_sources = "CMakeLists.txt", "CafeCommon*", "cmake*", "src*"

    def requirements(self):
        if self.options.CAFE_INCLUDE_TESTS:
            self.requires("catch2/3.0.0@catchorg/stable", private=True)

    def configure_cmake(self):
        cmake = CMake(self)
        for opt in Options:
            cmake.definitions[opt[0]] = getattr(self.options, opt[0])
        cmake.configure()
        return cmake

    def build(self):
        with tools.vcvars(self.settings, filter_known_paths=False) if self.settings.compiler == 'Visual Studio' else tools.no_op():
            cmake = self.configure_cmake()
            cmake.build()

    def package(self):
        with tools.vcvars(self.settings, filter_known_paths=False) if self.settings.compiler == 'Visual Studio' else tools.no_op():
            cmake = self.configure_cmake()
            cmake.install()

    def package_info(self):
        self.python_requires["CafeCommon"].module.addCafeSharedCompileOptions(self)
        self.cpp_info.libs = ["Cafe.ErrorHandling"]
        if self.options.CAFE_ERROR_HANDLING_INCLUDE_STACKWALKER and self.settings.os == "Windows":
            self.cpp_info.libs.append("DbgHelp.lib")
