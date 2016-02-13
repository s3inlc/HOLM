# Examples

This directory contains some example tasks to show how they need to be created that they can get parsed by HOLM.

Support for MDXfind will be added later...

## Important things

* Only comments over the while line (the first character in line must be a '#').
* Except 'type' all other keys must be present to define a task.
* Don't use the '-o' and '--outfile-format' switches on the Hashcat call string.
* Also don't use the '-m' switch on the call string.
* Be careful when capsuling file paths into quotes. Always just really quote the only necessary part and not the whole call string (Example: ```call=-a 0 "dicts/mydict.txt" "dicts/otherdict.txt" -r rules/myrule.rule``` instead of ~~```call="-a 0 dicts/mydict.txt dicts/otherdict.txt -r rules/myrule.rule"```~~ 
