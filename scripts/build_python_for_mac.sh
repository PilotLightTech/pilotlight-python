set -e

cd ../dependencies/cpython
mkdir -p build/debug
cd build/debug

# use homebrew's openssl if available (otherwise _ssl can fail to build/import,
# which makes "make install" fail when it tries to install the missing module)
OPENSSL_ARGS=""
if command -v brew >/dev/null 2>&1 && brew --prefix openssl@3 >/dev/null 2>&1; then
    OPENSSL_ARGS="--with-openssl=$(brew --prefix openssl@3)"
fi

../../configure --with-pydebug --enable-shared --prefix=$(pwd) LDFLAGS=-Wl,-rpath,$(pwd) $OPENSSL_ARGS
make
make install
