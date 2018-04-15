# Alloy

Alloy is the middleware/interface to the BareMetal kernel.

It's a full-color shell interface that supports active user command highlighting.

### Building on Linux

Alloy supports building on Linux.

It requires a full-color terminal to display properly.

To build it, enter the directory of the project and run these commands.

```bash
mkdir build
cd build
cmake ..
cmake --build .
./src/alloy
```

You should type `clear` after starting Alloy, to get rid of the current screen content.

### Dependencies

If you're building it for BareMetal, you'll need FreeType installed.

On Ubuntu, you'd install it like this:

```
sudo apt install libfreetype6-dev
```

Otherwise, you just need CMake and a build system that CMake supports, like GNU Make.
