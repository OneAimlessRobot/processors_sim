#!/bin/bash

valgrind -s --max-stackframe=6256368 --show-leak-kinds=all --track-fds=yes --leak-check=full --track-origins=yes ./*.exe

