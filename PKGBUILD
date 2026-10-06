pkgname=hem
pkgver=0.1.1
pkgrel=0
pkgdesc="Helix Emulator"
arch=('x86_64')
license=('MIT')
depends=('glibc' 'flagparser' 'isac')
makedepends=('clang' 'make' 'git')

source=("https://github.com/Helix-ISA/hem/archive/refs/heads/master.tar.gz")
sha256sums=('SKIP')

build() {
    cd "$srcdir/hem-master"
    make
}

package() {
    cd "$srcdir/hem-master"
    install -Dm755 bin/hem "$pkgdir/usr/bin/hem"
}
