# sycira
symbolic circuit analyzer for the Maxima computer algebra system

# Installation
There is no need to install sycira. 
You can load the package by giving the full path, for example:
```
load("/home/username/sycira/sycira.mac");
```
To be able to simply call `load("sycira")`, you have to add the following line to your `maxima-init.mac` file.
```
file_search_maxima: append(file_search_maxima, ["/home/username/sycira/###.mac"])$
```
Remember to adjust the path to the corresponding locations on your system.
