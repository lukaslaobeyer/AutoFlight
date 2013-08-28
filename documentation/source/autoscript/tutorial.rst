********
Tutorial
********

This tutorial will teach you some very basic Python and AutoScript functions so you can get started writing scripts right away. For a full Python tutorial visit http://docs.python.org/2/tutorial/.

AutoScript Scripting Environment
================================

First, take a look at the user interface for writing scripts. In the center there is a text area with line numbering and syntax highliting. On the bottom there is a bar with different actions:

.. figure::  _images/actionbar.png

From left to right:

   1. Save file as: Saves the file in the specified directory
   2. Save file: Saves the file, shows the "Save file as" dialog if the file was not yet saved
   3. Open file: Opens the specified file
   4. Simulate script: Executes the script, but instead of sending commands to the drone it prints out its actions on the terminal, and instead of reading sensor data it prompts the user for values. Extremely useful for testing.
   5. Execute script: Executes the script.
   6. Abort script execution: If a script is running, clicking this button aborts it and automatically lands the drone (in future versions, this behavior will be configurable).
   7. Show script output: Shows the output of the script if it is not already visible.
   8. Toggle land-on-error: When toggled, automatically lands the drone when a script error ocurrs.
   9. Help: Shows the help (The window you're looking at right now)

Take Off and Land
=================

Let's start with something very simple. Send a take off command to the drone, keep it in the air for some seconds, and land.
To make the drone take off, the function ::

   takeOff()

has to be called. It's that simple. This function, however, does only send the take off command without waiting for the drone to actually take off, so we'll have to wait a few seconds. Python has a function called ``sleep()`` which does exactly this. It can be found in the ``time`` module. To use functions in the time module, you have to import it. This is done at the beginning of the Python script, using the ``import`` command followed by the module you want to import, in this case ::

   import time

Then you can use the sleep function with the time you want to sleep as parameter, in parentheses. ::

   time.sleep(6)

The above function waits 6 seconds before letting the script contine. This will be enough time for the AR.Drone to take off. The rest of the time it will simply stay in the air. Now it's time to land the drone. The function for doing that is ::

   land()

Finally, we'll show a message saying that the script worked. Python has a command that shows a message called ``print``. Let's print our message ::

   print "I just made the drone automatically take off and land!"

.. note::

  ``print`` is a special command, and needs no parentheses.

The finished script would look like this: ::

   import time
   
   takeOff()
   time.sleep(5)
   land()
   print "I just made the drone automatically take off and land!"
   
Now it's time to run that script. Click run (The 5th button, counting from the left). If there's a typo and the drone doesn't land because an error ocurred, land the drone manually by pressing ``CTRL + F6``, check your code for typos and try to run it again.

If everything worked, congratulations! You ran your first AR.Drone-controlling-Python-script!

More coming soon
================

You can take a look at the official Python tutorial and all the available functions to experiment a bit!
