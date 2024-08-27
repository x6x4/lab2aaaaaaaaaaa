SET(LIB_NAME "regex")

add_library (${LIB_NAME} ${LIB_NAME}.cpp ${LIB_NAME}.h)
target_link_libraries(${LIB_NAME} ast)
target_link_libraries(${LIB_NAME} dfa_sets)
target_link_libraries(${LIB_NAME} regexTokenizer)