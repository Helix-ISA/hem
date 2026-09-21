pkgname=hem
pkgver=0.1.0
pkgrel=1
pkgdesc="Helix Emulator"
arch=('x86_64')
license=('MIT')
depends=('glibc')
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
