# HOLM (Hashes.Org Left Manager)

[![Travis build status](https://travis-ci.org/s3inlc/holm.svg?branch=master)](https://travis-ci.org/s3inlc/holm)

HOLM-alpha currently in development...

## Current progress

You can simply generate updated lists by calling:
```
./holm-alpha gen 40 16 32
```
to generate the 40, 16 and 32 hex new left list.

You will be required to enter an API key from your Hashes.org account. It can be found on the settings page of your user account.

To get the old lists, you just have to add the ```--old``` parameter. 

In case of errors, to increase the verbosity level, you can use ```-v```. ```-v 1``` for increased logging level and ```-v 2``` for the highest logging level.
