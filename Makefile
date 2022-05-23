# release build

.PHONY: all
all: release

.PHONY: release
release: build/release
	meson compile -C $<

build/release:
	meson setup $@ -Dbuildtype=release

# dev build

.PHONY: o3test
o3test: o3
	meson test -C build/$<

.PHONY: o3
o3: build/o3
	meson compile -C $<

build/o3:
	meson setup $@ -Doptimization=3

.PHONY: o2test
o2test: o2
	meson test -C build/$<

.PHONY: o2
o2: build/o2
	meson compile -C $<

build/o2:
	meson setup $@ -Doptimization=2

.PHONY: o1test
o1test: o1
	meson test -C build/$<

.PHONY: o1
o1: build/o1
	meson compile -C $<

build/o1:
	meson setup $@ -Doptimization=1

.PHONY: ogtest
ogtest: og
	meson test -C build/$<

.PHONY: og
og: build/og
	meson compile -C $<

build/og:
	meson setup $@ -Doptimization=g

# misc

.PHONY: clean
clean:
	rm -rf build
