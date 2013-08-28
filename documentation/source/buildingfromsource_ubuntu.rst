Building AutoFlight from Source on Ubuntu
*****************************************

Installing the dependencies
===========================

You will first need to install the Boost, Qt 5, FFmpeg, OpenCV and Python development packages.


Installing the Boost libraries
------------------------------

Run
::
   
   sudo apt-get install libboost1.53-dev libboost-python1.53-dev libboost-system1.53-dev libboost-filesystem1.53-dev libboost-thread1.53 libboost-chrono1.53-dev


Installing Qt 5
---------------

You can follow the steps described on http://qt-project.org/wiki/Building_Qt_5_from_Git to install necessary dependencies and get the source code. Please note that you should not skip WebKit support.

To configure the Qt build, use the following command:
::
   
   ./configure -opensource -confirm-license -nomake tests -nomake examples

Then just ``make`` and ``make install``.

FFmpeg
------

FFmpeg can be built as described on http://ffmpeg.org/trac/ffmpeg/wiki/UbuntuCompilationGuide. Support for x264 is needed, but everything else doesn't have to be built.

OpenCV
------

As for Qt and FFmpeg, the official guide (at http://docs.opencv.org/doc/tutorials/introduction/linux_install/linux_install.html) is great.

Python
------

::
   
   sudo apt-get install python3-dev


Building AutoFlight
===================

Now you are ready to get the latest AutoFlight source from it's Git repository.

::
   
   git clone https://????????????????????????????

When it's ready::

   cd AutoFlight/build
   cmake .
   make

This will build AutoFlight and put the executable into the ``release_linux`` folder.
