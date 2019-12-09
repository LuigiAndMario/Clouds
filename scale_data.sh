#!/bin/bash

for file in cloud_data/*/*.vti
do
	./scale_clouds/build/prog_scale_clouds $file
done
