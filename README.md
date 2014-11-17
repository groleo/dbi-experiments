# Chrome Dynamic Binary Instrumentation Experiments

This is used to see what DBI frameworks can be used to
wrap *gpu::gles2::GLES2Implementation::Viewport* method in Chrome,
but also to expose the overhead.

### Dyninst
http://www.dyninst.org/

So far I'm able to instrument Chrome and start it, but inserting only one
probe takes around two minutes and nothing is rendered.

### Pintool
https://pintool.org/

As specified in pintool readme, Chrome segfaults while inserting the probe.


### DynamoRIO
http://dynamorio.org/

Chrome stars and renders. The overhead makes webgl aquarium drop from 60fps
to 33fps.

# Setup
* Dyninst to be installed under /usr/local/
* Have chrome binary compiled run 'strip -g' on it.
* Edit ./config to specify the paths to chrome binary and to dynamorio instalation point.
