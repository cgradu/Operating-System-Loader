## Loader Implementation

This repository contains the implementation of a loader, developed as part of an Operating Systems project in 2022. The loader is responsible for handling SIGSEGV (Segmentation Fault) signals and managing memory mappings accordingly.

### Implementation Details

- **Author**: Constantin Radu-Giorgian

### Key Components

- **segv_handler**: This function handles SIGSEGV signals, which typically occur when a program tries to access memory that it doesn't have permission to access. The handler iterates through the file segments, mapping pages as needed based on fault addresses and segment boundaries.
  
- **so_init_loader**: This function initializes the loader by setting up the signal handler for SIGSEGV signals using `sigaction`.
  
- **so_execute**: This function is responsible for executing the specified program file (`path`) and passing any command-line arguments (`argv`). It opens the file, parses its contents using `so_parse_exec`, starts the execution using `so_start_exec`, and then closes the file.

### Usage

To use this loader implementation:

1. Include the necessary header files.
2. Implement the `segv_handler`, `so_init_loader`, and `so_execute` functions in your project.
3. Call `so_init_loader` to initialize the loader.
4. Call `so_execute` to execute the desired program file.

### Notes

- Ensure that the `exec_parser.h` header file is available in your project, as it is included in this implementation.
