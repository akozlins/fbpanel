#!/bin/bash

eval echo $(awk '/define/ { print $3 }' version.h)
