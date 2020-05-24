# ArmA3 DMA Script Injector

## How to use
- Download the latest version of [VMRead](https://github.com/Heep042/vmread) and place the contents in the VMRead directory.
- Using the meson build system run `meson build` to setup the build directory.
- In the build directory run `ninja` to start building.

You should now have an executable named `A3Injector` that can be run as root and supports the following parameters;

`./a3injector <dir to script> <infistar/ac thread deactivation (y/n)> <restore event handler after injecting (y/n)>`

## Environment Setup
To use this, you require a computer booted into Linux that will be the host machine, you will need to setup a KVM virtual machine and pass through a dedicated graphics card. (recommended: libvirt)

See the resources section for more information on setting up an environment.

## Requirements
- A computer capable of running a KVM virtual machine
- - Two graphics cards (dedicated/integrated)
- - A CPU that supports VT-d or equivalent instructions for KVM accelerated VM's

## Resources
https://www.youtube.com/watch?v=SsgI1mkx6iw
https://www.reddit.com/r/VFIO/comments/f15cok/how_to_setup_kvm_for_gaming/
https://www.mathiashueber.com/fighting-error-43-nvidia-gpu-virtual-machine/