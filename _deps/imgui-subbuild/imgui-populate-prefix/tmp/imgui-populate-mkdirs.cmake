# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/Admin/Desktop/github/3D-Graphics-Template/_deps/imgui-src"
  "C:/Users/Admin/Desktop/github/3D-Graphics-Template/_deps/imgui-build"
  "C:/Users/Admin/Desktop/github/3D-Graphics-Template/_deps/imgui-subbuild/imgui-populate-prefix"
  "C:/Users/Admin/Desktop/github/3D-Graphics-Template/_deps/imgui-subbuild/imgui-populate-prefix/tmp"
  "C:/Users/Admin/Desktop/github/3D-Graphics-Template/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp"
  "C:/Users/Admin/Desktop/github/3D-Graphics-Template/_deps/imgui-subbuild/imgui-populate-prefix/src"
  "C:/Users/Admin/Desktop/github/3D-Graphics-Template/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Admin/Desktop/github/3D-Graphics-Template/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Admin/Desktop/github/3D-Graphics-Template/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
