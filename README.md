# sycira
symbolic circuit analyzer for the Maxima computer algebra system.

With sycira you get easily from a circuit:
![simple example](images/simpleRC.png) 
to the equations which describe the circuit:
![simple example](images/simpleRC_GZ.png) 

sycira supports the following elements:
![supported elements](images/elements.png)

- V and I: independant voltage and current sources
- R, L, C: common passive elements: resistor, inductors and capacitors
- Controlled sources:
    - E: VCVS (voltage controlled voltage source)
    - F: CCCS (current controlled current source)
    - G: VCCS (voltage controlled current source)
    - H: CCVS (current controlled voltage source)
- K: coupling between inductors


##Tutorial
### sycira
We like to get the transferfunction G of the following circuit:
![simple example](images/simpleR_CpR.png)

A wxMaxima session would look like this:
![simple maxima session](images/tutorial_sycira.png)

After starting maxima or wxMaxima we prepare to have the sycira function available: ```load("sycira");```. then we have to describe the circuit with a netlist ```ckt:["R_CpR", ... ]```.
With ```sys:sycira(ckt);```we get a system of euqations for maxima's solver.
Which will be solved by:```sol:solve(sys[1],sys[2]);```
We get transferfucntion $G=\frac{v_{out}}{v_{in}}$ the ratio from the output to the input by ```G:ev(v[out]/v[in],sol[1]);```. and the input impedance "seen" by the source V1 with ```Z_in:v[in]/-i[V1] ```. The negative sign comes from the definition of the current throught the element V1. This is from pin 1 to pin 2 for alle element types.

The creation of the netlist is error prone. In the next section we use KiCad to create the netlist and a minimal wxMaxima session with the first commands that are always needed.

###KiCad2sycira
Here we derive the transferfunction of an active multiple feedback low pass filter.

#### Preparation
In KiCad we create a new project and open the schematics editor. Here we have to add the components library with the sycira elements. In the menubar click on "Preferences"->"Manage Symbol Libraries...":
 ![add Sycira Library](images/AddLibraries.png)
 
 We add a project specific library:
 ![](images/AddProjectSpecificLibrary.png)
 
By default the library is installed in ```/usr/local/share/KiCad2sycira/```.
![](images/LibraryAdded.png)

#### Drawing the schematic
![](images/tutorial_KiCad2sycira_schematics.png)
Remeber to add the 0-symbol. It will define the reference net with the name ```0```.
You don't have to define the Value fields but the defaults for the SymbolicValue are the same for the each type of element and can not be distinguished in the formula later. Here it is allowed to give algebraic expressions for example k*R for resistor with a constant scaling compared to another resistor.
 
#### Generate the netlist and wxMaxima session


#### Get the desired answers from wxMaxima

## Installation
### sycira
sycira is based on the computer algebra system Maxima. This has to be installed to be able to use sycira.
There is no need to install Sycira. You can load the package by giving the full path, for example:
```
load("/home/username/sycira/sycira.mac");
```
However - to simply call `load("sycira")`, you have to add the following line to your `maxima-init.mac` file.
```
file_search_maxima: append(file_search_maxima, ["/home/username/sycira/###.mac"])$
```
Remember to adjust the path to the corresponding locations on your system.



### KiCad2sycira
#### Build and installation
To use KiCad2sycira you need KiCad. Optional  is wxMaxima for which a minimal session file is created.
KiCad2sycira depends on the libraries TinyXml and zip. So the coresponding -dev packages have to be installed for the build.

To install the runtime requirements and the development requirements on Fedora:
```
sudo dnf install kicad wxmaxima tinyxml2 libzip
sudo dnf install git make g++ tinyxml2-devel libzip-devel
```
And for Ubuntu:
```
sudo apt install kicad wxmaxima libtinyxml2-6 libzip5
sudo apt install git make g++ libtinyxml2-dev libzip-dev
```
When these dependencies are ready,the KiCad2sycira plugin can be built and installed with:
```
cd KiCad2sycira
make
sudo make install
```
Default location is ```/usr/loca/```, which can be changed by the he ```PREFIX``` environment variable. 
Make sure ```$PREFIX/bin/``` is in the PATH.

#### Configuration of KiCad
After successfully installing KiCad2Sycira open KiCad's Eeschema and go to Generate Netlist

 ![add KiCad2Sycira plugin](images/GenerateNetlist.png)

Click "Add Plugin...". As Netlist command use the following:
```
KiCad2sycira %I %O
```
And choose a name, for example ```KiCad2sycira```. Afterwards click on "OK".

![Netlist command](images/AddPlugin.png)


    
