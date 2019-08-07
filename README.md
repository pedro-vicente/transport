# transport
Finite volume solver for advection-diffusion equatio

Dependencies
------------

[wxWidgets](https://www.wxwidgets.org/)
wxWidgets is a library for creating graphical user interfaces for cross-platform applications.


[Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)
Eigen is a C++ template library for linear algebra: matrices, vectors, numerical solvers, and related algorithms.

Install dependency packages (Ubuntu):
<pre>
sudo apt-get install build-essential
sudo apt-get install autoconf
sudo apt-get install libwxgtk3.0-dev
</pre>

Get source:
<pre>
git clone https://github.com/pedro-vicente/transport.git
</pre>

Build with:
<pre>
autoreconf -vfi
./configure
make
</pre>


