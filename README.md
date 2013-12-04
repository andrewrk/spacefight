# spacefight!

vaporware 3d space dogfighting simulator game

## Building

Install [rucksack](https://github.com/superjoe30/rucksack).

Set up a build rule to run `rucksack bundle assets.json assets.bundle` as
part of the build process.

In Qt Creator that looks like:

Command: rucksack
Arguments: bundle %{sourceDir}/assets.json %{buildDir}/assets.bundle
Working directory: %{sourceDir}

Check spacefight.pro for the required libraries.
Build with Qt Creator or `qmake spacefight.pro && make && ./spacefight`
