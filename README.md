# HOLM (Hashes.Org Left Manager)

[![Travis build status](https://travis-ci.org/s3inlc/holm.svg?branch=master)](https://travis-ci.org/s3inlc/holm)

HOLM-alpha currently in development...

You will be required to enter an API key from your Hashes.org account when running HOLM. It can be found on the settings page of your user account.

Read [here](src/BUILD.md) about building HOLM.

## Generating left lists

You can simply generate updated lists by calling:
```
holm-alpha gen 40 16 32
```
to generate the 40, 16 and 32 hex new left list.

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

## Execute multiple automatic tasks with one run

You can set HOLM to execute more than one task each one after the other, for example to do some various dictionaries or rules with different settings. 
There are two ways to do this:
* You give all the tasks you want to execute as arguments when calling HOLM: ```holm-alpha multi task1.ini task2.ini ....```
* You put all the task files you want to run into the folder 'tasks' in the application directory and then just call ```holm-alpha multi``` without any additional argument.

```--loop``` and ```--upload``` also work with multiple task files. Founds get uploaded after every task, and when looping is set, it will start from the beginning when all tasks are completed.
