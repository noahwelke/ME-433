add_executable(blink)

target_sources(blinky PRIVATE blinky.c)
# pull in common dependencies
target_link_libraries(blink pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(blink)
