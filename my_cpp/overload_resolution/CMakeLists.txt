add_executable(hidden_friend hidden_friend.cpp)
target_compile_features(hidden_friend PRIVATE cxx_std_23)

add_executable(msgs_print msgs_print.cpp)
target_compile_features(msgs_print PRIVATE cxx_std_23)
