Passes a token from 0 to the end of a 1d chare array, each object calling AtSync as soon as the token is passed.

Non-deterministically hangs with current master (ee0aff696ea13296c28ee032f32609af4f0ff159).

I've been reproducing it via this Fish script:
```
#! /usr/bin/env fish

set p 8
set arg 8

if count $argv > /dev/null
    set p $argv[1]
    set arg $argv[2]
end

while test $status -eq 0
    timeout 6 ./charmrun +p$p ./hello $arg +balancer RotateLB +setcpuaffinity ++local > debugOutput
    #rm debugOutput
end
```
