# g4me
Geant for me

## Simulation framework

I did not manage to do all the order I wanted in my Geant4 photon-conversion framework.
But I managed to put the current status on GitHub and what is there is something that works.  
https://github.com/preghenella/g4me

The software is called `g4me`.
There is support to build it with the aliBuild system.
For that you need to get the `g4me.sh` receipt from my private alidist branch  
https://github.com/preghenella/alidist/tree/g4me

If you manage to build it, here is how you run it.
You can start from the example configurations, these ones

```
g4me/share/g4macro/init.mac          [configures the detector geometry and B field]
g4me/share/g4macro/pythia8.mac       [steers the Geant4 simulation with Pythia8]
g4me/share/py8config/pythia8_hi.cfg  [configures Pythia8 for Pb-Pb collisions]
```

I will have to explain you better all the details.
Something is detailed in the following. Pease ask if it is unclear.  
Put those files in the same directory and run

```
$ g4me pythia8.mac
```

It will generated 10 Pb-Pb collisions and perform Geant4 transport of photons and unstable particles.
Eventually you should have a new file in the directory with the output of the simulation.

```
pythia8.000.root
```

There are switches in `pythia8.mac` to control which particles to transport

```
/stacking/transport gamma
/stacking/transport unstable
```

and you can control the number of events to be generated with

```
/run/beamOn 10
```

If you want to switch to pp inelastic collisions, put this file in your working directory

```
share/py8config/pythia8_inel.cfg
```

and modify the line

```
/pythia8/config pythia8_hi.cfg
```

Notice that in the default configuration only particles with |eta| < 0.8 are injected in Geant4.
You can change that behaviour from `pythia8.mac` with

```
/pythia8/cuts/eta -0.8 0.8
```

Notice that in the default Pythia8 configuration all particles with proper lifetime ctau < 10 mm are decayed by the event generator. Therefore they are not injected in Geant4 and are not visible in the output. This is a limitation that will have to be fixed in a future version to attempt to keep in the output file also resonances and very short-lived particles.  
The behaviour of Pythia8 can be modified in the `pythia8_hi.cfg` configuration file via the line

```
ParticleDecays:tau0Max = 10
```

This has to be done with care, because Geant4 might not know how to deal with the decay of some particles. This is another limitation that will have to be overcome in the future with the addition of the external decayer feature.

## Analysis Framework

If you did not manage to run the simulation by yourself, you can find an example output on Dropbox  
https://www.dropbox.com/s/rklbxjnvao4hbu2/pythia8.000.root

The output file `pythia8.000.root` is a plain ROOT Tree.
So you can browse it and do whatever you like with it.

The interesting branch is the `Tracks` branch, the elements of which contain this information

```
n                [number of tracks]
id               [track id]
pgd              [pdg code]
parent           [parent id, -1 if primary injected]
px, py, pz, e    [four momentum vector at creation]
vx, vy, vz, vt   [position and time at creation]
status           [auxiliary status information]
```

There is an utility macro `io.C` to deal with the IO from the tree.
Take that toghether with the example analysis `electron.C`

```
g4me/share/analysis/io.C
g4me/share/analysis/electron.C
```

and start a ROOT session for the analysis

```
root [0] .L electron.C
root [1] electron("pythia8.000.root")
```

This will create a histogram showing the log10(E) distribution of electrons.
They are separated by

* primary electrons
* secondary from photon conversion
* secondary from Compton scattering
* secondary from other (i.e. from Dalitz decay, I still have to put a decent flag for it)

The macro should be self-exaplanatory enough to understand what is goin on.
It shows you how to deal with the IO using the functionality provided by the `io.C` macro.  

Notice that the structure of the IO is one of the things that is likely will be modified in the near future.
