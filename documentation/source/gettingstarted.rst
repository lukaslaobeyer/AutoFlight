Overview
********

AutoFlight's GUI is divided into a main panel used to display the video stream and two sidebars containing buttons (to connect to the AR.Drone, launch the scripting environment, etc.) and indicators to show the real time sensor data.

.. note::
	
	If you have any questions, do not hesistate to contact me at ``lukas (at) lbpclabs (dot) com``.


Basic Usage
===========

.. warning::
   
   By using AutoFlight you agree that I'm not responsible for any damage you might cause to your AR.Drone while using this program. This is an alpha version!

Connecting to the AR.Drone
--------------------------

To start flying your AR.Drone, make sure you are already connected to it via WiFi, just like you would connect to any other wireless router. Then, just click the button in the upper right corner ("Connect to AR.Drone"). In about one second the real time video stream and sensor data should appear..

.. note:: 
	
	If it doesn't work, check you are not connected to any other wired or wireless networks. If it still doesn't work, contact me.

Head-Up Display
---------------

You can switch to a head-up display mode by pressing F5 or going into :menuselection:`View --> Head-Up Display` option. Now the sensor data should be presented as an overlay on top of the video stream. Press F5 again to exit this mode.

Flying with a Joystick / Gamepad / Keyboard
-------------------------------------------

Flying with a Joystick / Gamepad
++++++++++++++++++++++++++++++++

You can configure your joystick over the :menuselection:`Edit --> Controller Configuration` menu - it should be pretty straightforward.
If you don't have a controller, you can fly with your keyboard, too. The next section shows how.

.. note::
	
	My gamepad and joysticks work flawlessy, but if your do not, I've heard from users that `MotioninJoy <http://www.motioninjoy.com/>`_ can solve the problems.

Flying with the Keyboard
++++++++++++++++++++++++

No configuration is needed. Just use the commands described in the table below.

.. image:: _images/gettingstarted/af_controls.svg
   :width: 820px


+-------------------------------------------+---------------------------------------+---------------------------------------------+-------------+
| AR.Drone Commands                                                                 | AutoFlight Commands                                       |
+===========================================+=======================================+=============================================+=============+
| Take Off / Land                           | :kbd:`T`                              | Take Picture                                | :kbd:`P`    |
+-------------------------------------------+---------------------------------------+---------------------------------------------+-------------+
| Switch camera (Front/Bottom)              | :kbd:`V`                              | Start/Stop recording video                  | :kbd:`R`    |
+-------------------------------------------+---------------------------------------+---------------------------------------------+-------------+
| Flip                                      | 2x :kbd:`F`                           | Toggle HUD                                  | :kbd:`F5`   |
+-------------------------------------------+---------------------------------------+---------------------------------------------+-------------+
| Emergency                                 | 2x :kbd:`Y`                           | Start/Stop recording sensor data            | :kbd:`N`    |
+-------------------------------------------+---------------------------------------+---------------------------------------------+-------------+
| Up | Rotate left | Down | Rotate right    | :kbd:`I` :kbd:`J` :kbd:`K` :kbd:`L`   |                                                           |
+-------------------------------------------+---------------------------------------+---------------------------------------------+-------------+
| Forward | Left | Backward | Right         | :kbd:`W` :kbd:`A` :kbd:`S` :kbd:`D`   |                                                           |
+-------------------------------------------+---------------------------------------+---------------------------------------------+-------------+

Important warnings and known issues
===================================

*This program is still in alpha, which means that it is not yet stable and complete enough to be considered production-quality software. Also, you should keep in mind that I can not take responsability for broken AR.Drones and you should use this program at your own risk. (However, should AutoFlight crash while flying, the AR.Drone would hover and descend to an altitude of 1m.)*

The controller configuration is not checked automatically (yet), so you should confirm that you haven't assigned the same button/axis to multiple actions.

Some features like the image processor are not implemented yet but may be shown in the menus.

A few AutoScript functions are not implemented yet (see in-program AutoScript documentation).

The WiFi indicator is shown in the GUI but does not work.

There seem to be problems with the 3D map view not adjusting the view correctly (the virtual camera does not follow the drone indicator as it should).

If the main panel doesn't show the AutoFlight logo and you are unable to see the live video stream or the head-up display, you should make sure that you have at least OpenGL version 2. When running the program in VirtualBox (or other virtual environments) this might be a problem.

Miscellaneous
=============

Photos/Video
------------

Photos and recorded videos are saved in your home folder, under a new folder called AutoFlightSaves (e.g. in ``C:\Users\your_username\AutoFlightSaves`` on Windows 7).

To change the resolution of the video stream from 360P to 720P, you need to use the ``--stream-resolution`` command-line argument when starting the program::
	
	/path/to/AutoFlight.exe --stream-resolution 720P

For this you will need to start the program from the command prompt/terminal.

AR.Drone configuration
----------------------

Go into the :menuselection:`AR.Drone --> Flight Settings` menu to change the on-board flight parameters of the drone (max. roll/pitch angles, max. height, etc.).

Hardware extensions
-------------------

Hardware extensions to the AR.Drone 2.0 are not yet available.

Something does not work / I found a bug
=======================================

Please create an issue on my `JIRA project tracker <https://lbpclabs.atlassian.net>`_. You just will have to sign up. Thank you!

