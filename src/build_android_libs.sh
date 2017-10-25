#! /bin/bash


for i in $(ls Make_*); do make -f "$i" android CL=test; done

