#!/usr/bin/env bash
docker run -c=2 -m 1024m --rm -it -v $PWD/..:/opt/bcpp --name bcpp walterfan/bcpp:latest
