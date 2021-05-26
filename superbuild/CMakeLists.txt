
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_custom_target(superbuild ALL
    VERBATIM
    COMMAND Python3::Interpreter ${PROJECT_SOURCE_DIR}/scripts/merge.py
      "-I$<JOIN:$<TARGET_PROPERTY:asd_progetto2021,INTERFACE_INCLUDE_DIRECTORIES>,;-I>"
      "--include-match=asd_progetto2021/*"
      "-o${CMAKE_CURRENT_LIST_DIR}/superbuild.cpp"
      "${PROJECT_SOURCE_DIR}/solution.cpp"
    COMMENT Generate super unit.
    COMMAND_EXPAND_LISTS)
else()
  message("Python3 not found.")
endif()
