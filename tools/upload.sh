#!/bin/sh

## Copy the zipped Arduino Library to http://downloads.onion.io/Onion-Arduino-Library.zip

aws s3 cp Onion-Arduino-Library.zip s3://onion-downloads/Onion-Arduino-Library.zip

