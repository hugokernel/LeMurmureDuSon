#!/bin/bash
MODEL_HEIGHT=10
BASE_HEIGHT=1

for i in {0..5}; do java -jar heightmap2stl.jar "$i.jpg" $MODEL_HEIGHT $BASE_HEIGHT; done
