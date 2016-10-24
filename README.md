# BetterCast

The BetterCast project aims to create an open-source engine that enables high definition
screen/video broadcasting from a desktop or laptop PC, over a local network, to
a small linux-enabled device (such as an ODROID XU4). Inspired by the Google Chromecast,
the BetterCast aims to fill the lower-level need many have felt to simply broadcast
one's entire screen to a physically distant monitor or TV with little fuss and in
a completely content-agnostic fashion. People may find this desirable for a number
of reasons — Netflix, Vudu, Twitch, and other online video streaming services have
notoriously bad or spotty app support across all devices, and many times it would
be very useful/desirable to wirelessly broadcast a laptop or desktop screen onto a large
TV without having to manage long HDMI cables.

## Hardware

The BetterCast is being designed with the excellent ODROID XU4 in mind as the
desired target platform, though any ARM-based linux device with an OpenGL ES 3.0
profile and similar performance to the XU4 should work as well. The XU4 is
particularly desirable for its octa-core 2 GHz processor, 2 GB of memory, and
most importantly, a dedicated Mali-T628 GPU. This level of performance is very
rare for such a small (and relatively affordable) device, and rivals that of the
Google Chromecast by a fair margin. The XU4 also features an HDMI port and Gigabit
ethernet.

![ODROID XU4](http://www.hardkernel.com/main/_Files/prdt/2016/201606/201606241810180839.jpg)

The final deployment of the BetterCast will consist of an ODROID XU4 with a black
plastic enclosure, power cable, a very short HDMI cable, and USB 3.0 WiFi module.
This will allow the BetterCast to be attached directly to the back of a TV or
monitor, so users can stream video wirelessly to the TV/Monitor.

## How to use

1. Obtain an ODROID XU4, preferably with a WiFi module
2. Use one of the provided ROMs to install Ubuntu 16.04 (LTS)
3. Get the XU4 to connect to the LAN
4. Use our install script (coming soon) to install BetterCast on the XU4
5. Install OBS (or similar) RTMP software on your personal computer
6. (coming soon)


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

## Recent Progress

* ODROID XU4 Purchased
* installed Ubuntu 16.04 LTS on XU4
* tested video performance, performance in XBMC/Kodi is very good
* tested OpenGL support -- works, but difficult to compile

### Current Tasks:
1. get full-screen OpenGL ES window working
2. determine if full-screen rendering is going to be a bottleneck at 1920x1080 (1080p).
3. find a reliable h264 decoding library that can be used in OpenGL ES
