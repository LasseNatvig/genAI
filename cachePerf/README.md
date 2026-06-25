# README.md

Experiment with RPi4 vsCode cline mistral-medium June 24th 2026. Continued June 25th  (mistral-medium being under test these days).

## chat log

* make a plan for demonstrating the performance effect of regular vs. random access to a large array of numbers. The demo should show how the performance drops more sharply when the array does not any longer fit into the L1 cache, and similarly for the L2 cache. Use plain C, plot the performance effects with matplotlib and describe it briefly, not too verbose. I am running 32bit OS on a Raspberry Pi4B
* (after "first round") improve the plot by using logarithmic scale on the x-axis, and mark the measured samples as small rings along the lines
* rewrite analysis.md so that my markdownlint extension do not give report problems (**TODO** see if this can be specified as a skill)

## technical experience

* hang/stopped --- closed vsCode and restarted, could continue.
