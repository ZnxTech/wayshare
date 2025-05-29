#####################
# WayShare MakeFile #
#####################

# files
c_src  := $(wildcard ./src/*.c)
c_head := $(wildcard ./src/*.h)
c_obj  	     := $(c_src:./src/%.c=./build/obj/%.o)
c_obj_debug  := $(c_src:./src/%.c=./build/obj/%-debug.o)

wl_prot := $(wildcard ./wl_prot/*.xml)
wl_src  := $(wl_prot:./wl_prot/%.xml=./build/wl_src/%.c)
wl_head := $(wl_prot:./wl_prot/%.xml=./build/wl_src/%.h)
wl_obj  := $(wl_prot:./wl_prot/%.xml=./build/obj/%.o)

# flags
c_defs := -DWLR_USE_UNSTABLE
c_incs := -I./build/wl_src -I/usr/include/pixman-1
c_libs := -lwayland-client -lcurl -ljson-c -lpixman-1 -lcairo -lpng -lxkbcommon -ljpeg
c_warn := -Wall -Wno-error=incompatible-pointer-types

all: ./build/bin/wayshare ./build/bin/wayshare-debug

run: ./build/bin/wayshare-debug
	./build/bin/wayshare-debug;

format:
	@for f in $(c_src) $(c_head); do 				\
        indent $$f -o $$f -linux 					\
		-l100 -di0 -i4 -ts4 -il0 -c32 -cd32 -cp32;	\
    done

# code linking
./build/bin/wayshare: $(c_obj) $(wl_obj)
	@mkdir ./build/bin -p
	clang $(c_obj) $(wl_obj) -o $@ $(c_defs) $(c_libs)

./build/bin/wayshare-debug: $(c_obj_debug) $(wl_obj)
	@mkdir ./build/bin -p
	clang $(c_obj_debug) $(wl_obj) -o $@ $(c_defs) $(c_libs)

# code compilation
$(c_obj): ./build/obj/%.o: ./src/%.c $(c_head) $(wl_head)
	@mkdir ./build/obj -p
	clang -c $< $(c_incs) -o ./$@ $(c_defs) $(c_warn) -s

$(c_obj_debug): ./build/obj/%-debug.o: ./src/%.c $(c_head) $(wl_head)
	@mkdir ./build/obj -p
	clang -c $< $(c_incs) -o ./$@ $(c_defs) $(c_warn) -g -DDEBUG

$(wl_obj): ./build/obj/%.o: ./build/wl_src/%.c $(wl_head)
	@mkdir ./build/obj -p
	clang -c $< -o ./$@

# wayland source creation via wayland-scanner
$(wl_src): ./build/wl_src/%.c: ./wl_prot/%.xml
	@mkdir ./build/wl_src -p
	wayland-scanner private-code $< $@

$(wl_head): ./build/wl_src/%.h: ./wl_prot/%.xml
	@mkdir ./build/wl_src -p
	wayland-scanner client-header $< $@