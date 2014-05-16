Overview
********

.. note::

   If you have any questions, do not hesistate to contact me at ``lukas (at) lbpclabs (dot) com``.

.. todo::

   Installation instructions

Basic Usage
===========

.. warning::
   
   By using AutoFlight you agree that I'm not responsible for any damage you might cause to your AR.Drone while using this program. This is an alpha version!

Connecting to the AR.Drone
--------------------------

To start flying your AR.Drone, make sure you are already connected to it via WiFi, just like you would connect to any other wireless router. Then click `Connect to AR.Drone 2.0` in AutoFlight. If it doesn't work, make sure you are indeed connected to the drone, and click `Connect to AR.Drone 2.0` again. You should also make sure you are not connected to any other wired or wireless networks. If it still doesn't work, contact me.

Flying with a Joystick / Gamepad / Keyboard
-------------------------------------------

Flying with a Joystick / Gamepad
++++++++++++++++++++++++++++++++

You can configure your joystick over the "Edit > Controller Configuration" menu - it should be pretty straightforward. If you have questions don't hesistate to contact me. 
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

HUD
---

Since version DEV61, AutoFlight features a nice head-up display (HUD). To show/hide it, press :kbd:`F5`.

Hardware extensions
-------------------

Hardware extensions to the AR.Drone 2.0 are not yet available.

Something does not work / I found a bug
=======================================

Please create an issue on my `JIRA project tracker <https://lbpclabs.atlassian.net>`_. You just will have to sign up. Thank you!

