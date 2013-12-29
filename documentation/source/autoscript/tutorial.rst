********
Tutorial
********

This tutorial will teach you some very basic Python and AutoScript functions so you can get started writing scripts right away. For a full Python tutorial visit http://docs.python.org/3/tutorial/.

AutoScript Scripting Environment
================================

First, take a look at the user interface for writing scripts. In the center there is a text area with line numbering and syntax highliting. On the top there's a main menu containing options to save/open scripts and execute them as well as to open the documentation.
There are shortcuts for these actions on the bottom bar. Note that on this bar there's also a toggle button to enable the "land on error" behavior, which will automatically make the AR.Drone land when an error in the script is occurrs.
Also, apart from executing the scripts you can also simulate them. When simulating a script no commands will be sent to the drone. Instead, what would happen gets printed in the script output and the program needs the user to enter simulated sensor values. This feature is useful for testing your scripts and making sure everything works as expected.

.. note:: When editing a script you can always press ``F1`` to open the documentation. If there's an AutoScript command in the current line, the description of that command will be opened automatically. (Try it!)

Take Off and Land
=================

Let's start with something very simple. Send a take off command to the drone, keep it in the air for some seconds, and land.
To make the drone take off, the function ::

   control.takeOff()

has to be called. It's that simple. This function, however, does only send the take off command without waiting for the drone to actually take off, so we'll have to wait a few seconds. Python has a function called ``sleep()`` which does exactly this. It can be found in the ``time`` module. To use functions in the time module, you have to import it. This is done at the beginning of the Python script, using the ``import`` command followed by the module you want to import, in this case ::

   import time

Then you can use the sleep function with the time you want to sleep as parameter, in parentheses. ::

   time.sleep(6)

The above function waits 6 seconds before letting the script contine. This will be enough time for the AR.Drone to take off. The rest of the time it will simply stay in the air. Now it's time to land the drone. The function for doing that is ::

   control.land()

Finally, we'll show a message saying that the script worked. Python has a command that shows a message called ``print``. Let's print our message ::

   print("I just made the drone automatically take off and land!")

The finished script would look like this: ::

   import time
   
   control.takeOff()
   time.sleep(6)
   control.land()
   print("I just made the drone automatically take off and land!")
   
Now it's time to run that script. Click run (The 4th button on the bottom bar, counting from the left). If there's a typo and the drone doesn't land because an error ocurred, you can just land the drone manually (switch to the main window and press ``T``), check your code for typos and try to run it again.

If everything worked, congratulations! You ran your first AR.Drone-controlling-Python-script!

More coming soon
================

You can take a look at the official Python tutorial and all the available functions to experiment a bit!
