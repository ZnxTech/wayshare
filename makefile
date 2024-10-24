#####################
# WayShare MakeFile #
#####################

# files
cxx_src  := $(wildcard ./src/*.cc)
cxx_head := $(wildcard ./src/*.hh)
cxx_obj  	   := $(cxx_src:./src/%.cc=./build/obj/%.o)
cxx_obj_debug  := $(cxx_src:./src/%.cc=./build/obj/%-debug.o)

wl_prot := $(wildcard ./wl_prot/*.xml)
wl_src  := $(wl_prot:./wl_prot/%.xml=./build/wl_src/%.c)
wl_head := $(wl_prot:./wl_prot/%.xml=./build/wl_src/%.hh)
wl_obj  := $(wl_prot:./wl_prot/%.xml=./build/obj/%.o)

# flags
cxx_defs := -D WLR_USE_UNSTABLE
cxx_libs := -l m -l wayland-client

all: ./build/bin/wayshare ./build/bin/wayshare-debug

# code linking
./build/bin/wayshare: $(cxx_obj) $(wl_obj)
	@mkdir ./build/bin -p
	g++ $(cxx_obj) $(wl_obj) -o $@ $(cxx_defs) $(cxx_libs)

./build/bin/wayshare-debug: $(cxx_obj_debug) $(wl_obj)
	@mkdir ./build/bin -p
	g++ $(cxx_obj) $(wl_obj) -o $@ $(cxx_defs) $(cxx_libs)

# code compilation
$(cxx_obj): ./build/obj/%.o: ./src/%.cc $(cxx_head) $(wl_head)
	@mkdir ./build/obj -p
	g++ -c $< -I ./build/wl_src -o ./$@ $(cxx_defs)

$(cxx_obj_debug): ./build/obj/%-debug.o: ./src/%.cc $(cxx_head) $(wl_head)
	@mkdir ./build/obj -p
	g++ -c $< -I ./build/wl_src -o ./$@ $(cxx_defs) -D DEBUG

$(wl_obj): ./build/obj/%.o: ./build/wl_src/%.c $(wl_head)
	@mkdir ./build/obj -p
	gcc -c $< -o ./$@

# wayland source creation via wayland-scanner
$(wl_src): ./build/wl_src/%.c: ./wl_prot/%.xml
	@mkdir ./build/wl_src -p
	wayland-scanner private-code $< $@

$(wl_head): ./build/wl_src/%.hh: ./wl_prot/%.xml
	@mkdir ./build/wl_src -p
	wayland-scanner client-header $< $@