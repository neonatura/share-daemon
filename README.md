share-daemon
====

<b>The Share Library Project.</b>

The share daemon distributes context generated by the libshare runtime library by providing an automated transport protocol in addition to an authenticatable API.

API: <a href="http://docs.sharelib.net/">Share Library Documentation</a>

<h2>Quick Instructions</h2>

<h3>Share Library Build Instructions</h3>

On linux, a build can be performed by running the following:
<i><small><pre>
  cd ~
  git clone https://github.com/neonatura/share
  cd share
  mkdir build
  cd build
  ../configure
  make
  make install

  cd ~
  git clone https://github.com/neonatura/share-daemon
  cd share-daemon
  mkdir build
  cd build
  ../configure
  make
  make install
</pre></small><i>

 Note: Use --libdir=/usr/lib64 on Cent OS platforms.

<h3>Build Dependencies</h3>

The "libusb-1.0" library is required for compiling the libshare library suite unless the configure flag "--disable-usb" is specified. 

On CentOS you can run the following:
	yum install libusb1 libusb1-devel

On Ubuntu you can run the following:
	apt-get install libusb-1.0.0-dev

You can compile and install the included "libusb-1.0.XX.zip" on any platform.
Download Url: https://sourceforge.net/projects/libusb/files/libusb-1.0/libusb-1.0.21/libusb-1.0.21.zip/download

"libusb-1.0" compile instructions:
	unzip libusb-1.0.XX.zip
	cd libusb-1.0.XX
	./autogen.sh
	./configure
	make
	make install

