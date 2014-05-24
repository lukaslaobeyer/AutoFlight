******************************************
Getting Started using OpenCV in AutoFlight
******************************************

.. note::
	To be able to use OpenCV from AutoScript, you will first have to install the image processing add-on.
	Get it `here <https://github.com/lukaslaobeyer/AutoFlight/releases/download/v0.2.0-alpha/AF_ImageProcessing.zip>`_.
	Also, please make sure you are using at least AutoFlight version 0.2.

Installation
============

Extract the downloaded ``AF_ImageProcessing.zip`` file into the ``Lib/site-packages/`` folder in AutoFlight's root directory, which usually will be in the AppData folder. (To find it, you can simply open Explorer and type ``%appdata%/AutoFlight/Lib/site-packages/``.)
Now you are ready to use OpenCV from the AutoScript environment. You can test this by importing the ``cv2`` module and running the help function for it. To do this, open AutoScript and type:

.. code-block:: python

	import cv2
	help(cv2)

If the image processing add-on was correctly installed, running the script should output a long list with available classes, functions, etc.
You are now ready to use OpenCV.