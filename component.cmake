
add_compile_options(-fdiagnostics-color=always)

list(APPEND EXTRA_COMPONENT_DIRS 
                                "$ENV{IOT_SOLUTION_PATH}/components"
                                "$ENV{IOT_SOLUTION_PATH}/components/bus"
                                "$ENV{IOT_SOLUTION_PATH}/components/display"
                                "$ENV{IOT_SOLUTION_PATH}/components/gui"
                                "$ENV{IOT_SOLUTION_PATH}/components/boards"
                                )

