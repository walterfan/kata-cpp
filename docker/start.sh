#!/usr/bin/env bash
docker run --rm -it -v $PWD/..:/opt/bcpp --name bcpp walterfan/bcpp:latest