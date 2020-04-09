
Example for testing if `find_package( MarlinMT ... )` is working .

- `simple`: build a simple main file, linking to MarlinMT::Core. This example should always work
- `failing`: Build the thing as `simple` but tries to link against a module library (LCIOPlugins) which is not imported 
