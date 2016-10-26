# BetterCast

The BetterCast project aims to create an open-source engine that enables high definition
screen/video broadcasting from a desktop or laptop PC, over a local network, to
a small linux-enabled device (such as an ODROID XU4). Inspired by the Google Chromecast,
the BetterCast aims to fill the more basic need many people have to simply broadcast
one's entire screen to a physically distant monitor or TV with little fuss and in
a completely content-agnostic fashion. People may find this desirable for a number
of reasons — Netflix, Vudu, Twitch, and other online video streaming services have
notoriously bad or spotty app support across all devices, and many times it would
be very useful/desirable to wirelessly broadcast a laptop or desktop screen onto a large
TV without having to manage long HDMI cables. The Chromecast is capable of doing this
in a very basic sense, however it chokes if you actually try to screen capture a
"non-castable" video screen or live-stream a video game.

## Hardware

The BetterCast is being designed with the excellent ODROID XU4 in mind as the
desired target platform, though any ARM-based linux device with an OpenGL ES 3.0
profile and similar performance to the XU4 should work as well. The XU4 is
particularly desirable for its octa-core 2 GHz processor, 2 GB of memory, and
most importantly, a dedicated Mali-T628 GPU. This level of performance is very
rare for such a small (and relatively affordable) device, and rivals that of the
Google Chromecast by a fair margin. The XU4 also features an HDMI port and Gigabit
ethernet. The XU4 is also capable of running Android. This could be extremely useful.
If BetterCast were written for Android instead of for linux, it could target an even
wider range of devices.

![ODROID XU4](http://www.hardkernel.com/main/_Files/prdt/2016/201606/201606241810180839.jpg)

The final deployment of the BetterCast will consist of an ODROID XU4 with a black
plastic enclosure, power cable, a very short HDMI cable, and USB 3.0 WiFi module.
This will allow the BetterCast to be attached directly to the back of a TV or
monitor, so users can stream video wirelessly to the TV/Monitor.

## How to use

1. Obtain an ODROID XU4, preferably with a WiFi module
2. Use one of the provided ROMs to install Ubuntu 16.04 (LTS) (may change to Android, stay tuned)
3. Get the XU4 to connect to the LAN
4. Use our install script (coming soon) to install BetterCast on the XU4
5. Install OBS (or similar) RTMP software on your personal computer
6. Enter the local IP address of the XU4 as the broadcast target for OBS and start broadcasting!


## Protocol Support

The BetterCast will implement the RTMP protocol for receiving video streams. This
is the same protocol used by popular services such as Twitch.tv to receive incoming
video data. By supporting RTMP, we are enabling users to use the broadcast
software of their choice to send video to the BetterCast. For best results,
we recommend using [OBS (Open Broadcaster Software)](https://obsproject.com/), which
has binaries available for Linux, Windows, and even OSX. Supporting RTMP also has
the advantage of allowing us to focus solely on the BetterCast itself, instead of
worrying about writing client-side software capable of broadcasting for every
operating system.

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
