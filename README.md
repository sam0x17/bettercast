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

## Data Model

![BetterCast data model](/docs/bettercast_data_model1.png "data model")

The data model used by the BetterCast is simple. Video data from the client desktop/laptop
PC is recorded from the primary monitor and system sound output, encoded in OBS, and sent
via the RTMP protocol over the LAN to the the BetterCast, which decodes and displays
this stream on the TV/monitor via a direct HDMI connection.

## Tasks / Software Model

Because of the generally linear flow of data through the system, it is relatively easy
to identify key tasks and the overall software model that will implement these tasks.
The overall model follows the linear data model defined above, with the exception of
a limited set of control message the XU4 can send "backwards" to the client to change
this such as audio/video bitrate and display resolution.

Task Name | Parameters | Description
--- | --- | ---
RTMPListen | source_IP | listen for incoming connections on the RTMP port
RTMPRead | data_block | read a chunk of video data from the RTMP stream
RTMPDecode | stream, data_block | decode a chunk of video data from the RTMP stream
RTMPDisplay | stream, decoded_data_block | display a chunk of video data on the HDMI device
DisplayNoInput | none | visually notify the user that no input stream has been specified
RTMPSetResolution | stream, target_IP | notify connected client to send a specific resolution
RTMPSetBitRate | stream, target_IP | notify connected client to send a specific bitrate
RTMPClose | none | client notifies XU4 that the stream has closed/ended

## Recent Progress

* ODROID XU4 Purchased
* installed Ubuntu 16.04 LTS on XU4
* tested video performance, performance in XBMC/Kodi is very good
* tested OpenGL support -- works, but difficult to compile

### Current Tasks:
1. get full-screen OpenGL ES window working
2. determine if full-screen rendering is going to be a bottleneck at 1920x1080 (1080p).
3. find a reliable h264 decoding library that can be used in OpenGL ES
4. assess whether it would be better to use Android on the XU4 and write an Android app

![Odroid XU4 running Android](http://www.cnx-software.com/wp-content/uploads/2015/12/Android_6.0_ODROID_XU4.png)
Odroid XU4 running the Android alternative ROM instead of Ubuntu
