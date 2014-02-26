YOLOS
=====

An OS for... well, nobody.

Environment
-----------

This stuff is much easier to do on Linux, so I'm using Vagrant to easily work
inside a Linux vm. It's super easy to use:

    vagrant up
    vagrant ssh

Running
-------

Now that you have the vagrant environment, you should be able to run the kernel
using qemu:

    vagrant ssh
    cd /vagrant
    make qemu # or make bochs

Note: You must run `make cross` before running the OS for the first time.