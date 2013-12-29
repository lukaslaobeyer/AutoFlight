******************************
Available AutoScript Functions
******************************

Here you will find a description of every available function you can use to control the AR.Drone, divided into the main modules (``control`,``sensors``, ``util``).

AR.Drone control
================

.. function:: control.backward_distance(speed, centimeters)

   Moves the drone backward, and stops it after the specified distance has been traveled. Shortcut for ``move_distance(0, speed, 0, 0, centimeters)``
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   :param centimeters: The distance to be traveled, in centimeters.
   :type centimeters: float
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

.. function:: control.backward_time(speed, millis)

   Moves the drone backward, and stops it after the specified amount of time has elapsed. Shortcut for ``move_time(0, speed, 0, 0, millis)``
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   :param millis: The time to wait before stoping the drone, in milliseconds.
   :type millis: int
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

   
.. function:: control.backward(speed)

   Moves the drone backward. Shortcut for ``move(0, speed, 0, 0)``. To stop it, call the ``hover()`` command.
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.

.. function:: control.down_distance(speed, centimeters)

   Moves the drone down, and stops it after the specified distance has been traveled. Shortcut for ``move_distance(0, 0, -speed, 0, centimeters)``
   
   .. warning:: Not implemented yet! Do not use.
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   :param centimeters: The distance to be traveled, in centimeters.
   :type centimeters: float
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

.. function:: control.down_time(speed, millis)

   Moves the drone down, and stops it after the specified amount of time has elapsed. Shortcut for ``move_time(0, 0, 0, -speed, millis)``
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   :param millis: The time to wait before stoping the drone, in milliseconds.
   :type millis: int
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.

   
.. function:: control.down(speed)

   Moves the drone backward. Shortcut for ``move(0, 0, 0, -speed)``. To stop it, call the ``hover()`` command.
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   
   
.. function:: control.flip(direction)
   
   Sends the flip command to the AR.Drone. Only works with 2.0 drones.
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   
   
.. function:: control.forward_distance(speed, centimeters)

   Moves the drone forward, and stops it after the specified distance has been traveled. Shortcut for ``move_distance(0, -speed, 0, 0, centimeters)``
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   :param centimeters: The distance to be traveled, in centimeters.
   :type centimeters: float
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

.. function:: control.forward_time(speed, millis)

   Moves the drone forward, and stops it after the specified amount of time has elapsed. Shortcut for ``move_time(0, -speed, 0, 0, millis)``
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   :param millis: The time to wait before stoping the drone, in milliseconds.
   :type millis: int
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

.. function:: control.forward(speed)

   Moves the drone forward. Shortcut for ``move(0, -speed, 0, 0)``. To stop it, call the ``hover()`` command.
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.

.. function:: control.hover()

   Hovers the drone, so it tries to stay at a fixed position. Equivalent to calling ``move(0, 0, 0, 0)``.
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   
.. function:: control.land()

   Sends a land command to the drone.
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

.. function:: control.left_distance(speed, centimeters)

   Moves the drone left, and stops it after the specified distance has been traveled. Shortcut for ``move_distance(-speed, 0, 0, 0, centimeters)``
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   :param centimeters: The distance to be traveled, in centimeters.
   :type centimeters: float
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

.. function:: control.left_time(speed, millis)

   Moves the drone left, and stops it after the specified amount of time has elapsed. Shortcut for ``move_distance(-speed, 0, 0, 0, millis)``
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   :param millis: The time to wait before stoping the drone, in milliseconds.
   :type millis: int
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

.. function:: control.left(speed)

   Moves the drone left. Shortcut for ``move_distance(-speed, 0, 0, 0)``. To stop it, call the ``hover()`` command.
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

.. function:: control.move_distance(phi, theta, gaz, yaw, centimeters)

   Moves the drone as in ``move()``, but stops it automatically after the specified distance has been traveled.
   
   .. warning:: If the vertical camera of your AR.Drone does not work or data is somehow not sent back correctly, this can be dangerous. This function uses speed to calculate the traveled distance, and this speed data is computed by analyzing the vertical cameras pictures. **Will** be problematic if what the drone is flying over has no distinguishable features!
   
   :param phi: See ``move()``.
   :type phi: float
   :param theta: See ``move()``.
   :type theta: float
   :param gaz: See ``move()``.
   :type gaz: float
   :param yaw: See ``move()``.
   :type yaw: float
   :param centimeters: The distance to be traveled, in centimeters.
   :type centimeters: float
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

.. function:: control.move_time(phi, theta, gaz, yaw, millis)

   Moves the drone as in ``move()``, but stops it automatically after the specified amount of time.
   
   :param phi: See ``move()``.
   :type phi: float
   :param theta: See ``move()``.
   :type theta: float
   :param gaz: See ``move()``.
   :type gaz: float
   :param yaw: See ``move()``.
   :type yaw: float
   :param millis: The time to wait before stoping the drone, in milliseconds.
   :type millis: int
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

.. function:: control.move(phi, theta, gaz, yaw)

   Moves the drone. The parameters are fractions of the maximum allowed angle, and have to be in the range from -1.0 (corresponding to the maximum tilt into one direction) to 1.0 (corresponding to the maximum tilt into the other direction). 
   
   .. warning:: This function will cause the drone to move with the specified parameters for an infinite amount of time. You will need to call the ``hover()`` command to stop it.

   :param phi: Roll angle (-1.0: full angle to left hand side; 1.0: full angle to right hand side)
   :type phi: float
   :param theta: Pitch angle (**-1.0**: full speed in **forward** direction; **1.0**: full speed in **backward** direction)
   :type theta: float
   :param gaz: Vertical speed (Exception as it is no angle. -1.0 would then be full speed down, 1.0 full speed up)
   :type gaz: float
   :param yaw: Yaw speed (Also no angle. -1.0 would be full speed in counterclockwise direction, 1.0 full speed in clockwise direction)
   :type yaw: float
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.


.. function:: control.right_distance(speed, centimeters)

   Moves the drone right, and stops it after the specified distance has been traveled. Shortcut for ``move_distance(speed, 0, 0, 0, centimeters)``
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   :param centimeters: The distance to be traveled, in centimeters.
   :type centimeters: float
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

.. function:: control.right_time(speed, millis)

   Moves the drone right, and stops it after the specified amount of time has elapsed. Shortcut for ``move_distance(speed, 0, 0, 0, millis)``
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   :param millis: The time to wait before stoping the drone, in milliseconds.
   :type millis: int
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

.. function:: control.right(speed)

   Moves the drone right. Shortcut for ``move_distance(speed, 0, 0, 0)``. To stop it, call the ``hover()`` command.
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

.. function:: control.rotate(speed, degs, clockwise)

   Rotates the drone by the specified angle at the specified speed in the specified direction. Yes, it does exactly what you specified. And no, in the next sentence there will not be the word *specified* again.
   
   .. warning:: Somehow, this does not always work as expected. I don't yet know why, but I hope I'll find the error soon.
   
   :param speed: The rotation speed
   :type speed: float
   :param degs: How many degrees the drone should rotate (>=0, please)
   :type degs: float
   :param clockwise: ``True`` for clockwise rotation, ``False`` for counterclockwise rotation
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   
.. function:: control.takeOff()

   Sends a take off command to the drone. This will only send the command and continue immediately, so you'll probably want to wait 4-6 seconds before calling any other functions.
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

.. function:: control.up_distance(speed, centimeters)

   Moves the drone up, and stops it after the specified distance has been traveled. Shortcut for ``move_distance(0, 0, speed, 0, centimeters)``

   .. warning:: Not implemented yet! Do not use.
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   :param centimeters: The distance to be traveled, in centimeters.
   :type centimeters: float
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

.. function:: control.up_time(speed, millis)

   Moves the drone up, and stops it after the specified amount of time has elapsed. Shortcut for ``move_time(0, 0, 0, speed, millis)``
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   :param millis: The time to wait before stoping the drone, in milliseconds.
   :type millis: int
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.

   
.. function:: control.up(speed)

   Moves the drone up. Shortcut for ``move(0, 0, 0, speed)``. To stop it, call the ``hover()`` command.
   
   :param speed: The speed at which the drone should be flying. See ``move()``.
   :type speed: float
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   

Sensor data retrieving
======================

.. function:: sensors.getAcceleration(axis)

   Reads the accelerometer's value on the specified axis.
   
   :param axis: "X", "Y" or "Z"
   :type axis: string
   
   :returns: The acceleration on the specified axis, in g.
   
.. function:: sensors.getAltitude()

   Reads the drone's altitude.
   
   :returns: The altitude in m.
   
.. function:: sensors.getBatteryLevel()

   Reads the battery's level.
   
   :returns: The battery level in %.
   
.. function:: sensors.getOrientation(axis)

   Reads the gyroscope's value on the specified axis.
   
   :param axis: "YAW", "PITCH" or "ROLL"
   :type axis: string

   :returns: The angle on the specified axis, in degrees, as a value between -180 and +180.
   
.. function:: sensors.getOrientation360(axis, clockwise)

   Reads the gyroscope's value on the specified axis.
   
   :param axis: "YAW", "PITCH" or "ROLL"
   :type axis: string
   :param clockwise: The direction in which to count, if ``True`` then in clockwise direction.
   :type clockwise: boolean

   :returns: The angle on the specified axis, in degrees, as a value between 0 and 360. E.g. if the drone is tilted 10 degrees to the right, the value would not be 10 but 100 degrees, if clockwise is ``True``. Useful for measuring yaw angles.
   
.. function:: sensors.getLinearVelocity(axis)

   Reads the drone's speed on the specified axis.
   
   :param axis: "X", "Y", or "Z"
   :type axis: string
   
   :returns: The drone's speed on the specified axis, in m/s.
   
.. function:: util.isConnected()

   Checks if AutoFlight is receiving data from the drone.
   
   :returns: ``True`` if connected, ``False`` otherwise
   
.. function:: util.isFlying()

   Checks if the drone is flying.
   
   :returns: ``True`` if flying, ``False`` otherwise

Utilities and other
===================

.. function:: util.flatTrim()

   Sends the "flat trim" command to the AR.Drone. This command calibrates the drone's inertial measurement unit, so it probably is nonsense if you call this while not on a flat surface.
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.
   
   
.. function:: util.calibrateMagnetometer()

   Sends the "magnetometer calibration" command to the AR.Drone. This command will make the drone rotate on its yaw axis a couple of times, so be careful when calling this.
   
   :returns: ``True`` if the command could be send successfully, ``False`` otherwise.


.. function:: util.startRecording()

   Starts recording video to the default location.
   
   :returns: ``True`` if command completed successfully, ``False`` otherwise.
   

.. function:: util.stopRecording()

   Stops recording video.
   
   :returns: ``True`` if command completed successfully, ``False`` otherwise.
   

.. function:: util.toggleRecording()

   Toggles video recording. (Starts recording if it's currently not recording, stops recording if it's currently recording.)
   
   :returns: ``True`` if command completed successfully, ``False`` otherwise.
   
   
.. function:: util.savePicture(path)

   Takes a picture and saves it as JPEG to the specified file.
   
   :param path: The filename of the picture.
   :type path: string
   :returns: ``True`` if command completed successfully, ``False`` otherwise.
