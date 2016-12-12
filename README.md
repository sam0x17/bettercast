# BetterCast

The BetterCast project aims to create an open-source engine that enables high definition
low-latency screencasting from a desktop or laptop PC, over a local network, to
a small android-enabled device (such as an ODROID XU4). Existing devices, such as the
Google Chromecast are already optimized for streaming high definition video, with a
3-4 second delay. If you simply want to broadcast the screen of your laptop while
you work on some code, however, there are few if any workable options, since this
requires a high definition feed and very low latency. The BetterCast seeks to fill
this niche of broadcasting highly static content at a lower latency than the 3-4
second delay offered by Chromecast and similar devices. The BetterCast accomplishes
this goal using two primary optimizations:

1. The BetterCast does not using any encoding or transcoding, as this is slow and
   is the primary source of delay in modern streaming devices. Since we are streaming
   over the LAN, we can afford to send uncompressed raw pixel data over the network,
   and it turns out this is significantly faster (in terms of latency) than encoding,
   transferring, and then decoding.
2. The BetterCast uses a custom "diff patch" algorithm. The screen is broken up into
   60x60 patches (this number works best on modern UIs), and once a keyframe has
   been sent, only the patches that had changed pixels since the last transmission
   are sent to the BetterCast.

The result of these optimizations is that a full-screen rewrite takes about a second,
however partial screen changes take a small fraction of the time. When doing basic
text editing and programming, the experience is virtually zero-latency (unless you
are scrolling).

Because the BetterCast already runs Android, streaming things like Netflix, etc.,
does not need to be handled directly by the BetterCast software, since you can
already do this directly with Android apps.

Note that currently only Linux is supported by the broadcaster / BetterCast client.
Support for other operating systems can be added easily.

## Hardware

The BetterCast is being designed with the excellent ODROID XU4 in mind as the
desired target platform, though any ARM-based Android device with an OpenGL ES 3.0
profile and similar performance to the XU4 should work as well. The XU4 is
particularly desirable for its octa-core 2 GHz processor, 2 GB of memory, and
most importantly, a dedicated Mali-T628 GPU. This level of performance is very
rare for such a small (and relatively affordable) device, and rivals that of the
Google Chromecast by a fair margin. The XU4 also features an HDMI port and Gigabit
ethernet.

![ODROID XU4](http://www.hardkernel.com/main/_Files/prdt/2016/201606/201606241810180839.jpg)

The final deployment of the BetterCast consists of an ODROID XU4 with a black
plastic enclosure, power cable, a very short HDMI cable, and USB 3.0 WiFi module.
This allows the BetterCast to be attached directly to the back of a TV or
monitor.

## How to use

1. Obtain an ODROID XU4, preferably with a WiFi module
2. Use one of the provided ROMs to install the latest version of Android available for the ODROID.
3. Get the XU4 to connect to the LAN
4. Install the BetterCast APK file (hostname can easily be changed from the code)
5. Run the BetterCast client

Note: it is assumed that you will broadcast at 1920x1080. If your computer runs at
a different resolution, simply change the screen resolution constants in the APK
and in bettercast.h and rebuild. If you use a different screen resolution, you will
need to select a patch size that divides both the width and height of your screen
dimensions.


## Protocol Support

Originally we had plans to implement the RTMP or the RTSP protocols on the BetterCast,
but we found that these protocols work against exactly what we are trying to optimize:
low latency, so we forged our own path and wrote our own simple network protocol for
the BetterCast. The following table summarizes the various commands available within
the protocol.

Command Name | Response Format | Description
--- | --- | ---
size | ####x#### | returns the broadcaster's screen resolution "e.g. 1920x1080"
key | (binary data) | returns the raw pixel data for the current frame
diff | ##x##, (binary data).. | returns the raw pixels of each changed patch
bettercast:probe | bettercast:ack | used to quickly scan the network for supported clients
close | NA | instructs the client to cease broadcasting


## Tasks / Software Model

There are two primary task. The "server" (BetterCast) app scans the network for a
broadcaster on the BetterCast port (13314). Once connected, it will display the
video feed from the broadcaster on the Android display. The "client" (laptop/PC)
simply runs the BetterCast broadcasting software and advertises the fact that it
has a feed available by responding to incoming connections on the BetterCast port.
Once a BetterCast is connected, the client will respond to its requsts for screen
data in a timely fashion.

## Source Code

The source code is divided into three parts. The "src" directory contains the source
code for the BetterCast client application (the broadcaster). The client application
can be built and run by running ./build.sh and then ./client. Any required libraries
will be installed automatically. The Android app is contained within the "app" directory.

### Future Work
1. Use OpenGL ES on Android for ~50-80 FPS instead of ~20-30 FPS. (currently drawing
   to the screen is slower than transferring pixel data over the network!)
2. Active scanning for broadcasters
3. Ability to customize screen resolution, hostname, etc. (not important for demo purposes)
4. Custom algorithm to optimize the performance of scrolling
5. True full-screen mode
6. Support more operating systems!
