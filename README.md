# HOLM (Hashes.Org Left Manager)

[![Travis build status](https://travis-ci.org/s3inlc/holm.svg?branch=master)](https://travis-ci.org/s3inlc/holm)

HOLM-alpha currently in development...

## Generating left lists

You can simply generate updated lists by calling:
```
holm-alpha gen 40 16 32
```
to generate the 40, 16 and 32 hex new left list.

You will be required to enter an API key from your Hashes.org account. It can be found on the settings page of your user account.

To get the old lists, you just have to add the ```--old``` parameter. 

In case of errors, to increase the verbosity level, you can use ```-v```. ```-v 1``` for increased logging level and ```-v 2``` for the highest logging level.

## Execute automatic tasks

You can configure HOLM to automatically call oclHashcat/Hashcat after the list generation with given settings and even upload founds automatically to Hashes.org after the run is finished.

To set a task configuration you need to create a task file (like in examples folder) and then call HOLM with it:
```
holm-alpha single examples/minimalist.ini
```
Additionally you can set the following switches:
```
--loop        (HOLM will start with generating a new list again and then running the task again, 
              as long as you don't press CTRL^C)
--upload      (This will turn on the automatic upload of the found hashes to Hashes.org with your 
              API keys related user account)
```
