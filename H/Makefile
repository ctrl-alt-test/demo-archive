PLATFORM := $(shell uname -s)
OUTPUT_DIR = build/$(PLATFORM)

debug:
	mkdir -p $(OUTPUT_DIR)/debug
	cd $(OUTPUT_DIR)/debug && cmake ../../.. -DCMAKE_BUILD_TYPE=debug && make $(TARGET)

release:
	mkdir -p $(OUTPUT_DIR)/release
	cd $(OUTPUT_DIR)/release && cmake ../../.. -DCMAKE_BUILD_TYPE=release && make $(TARGET)

clean:
	rm -Rvf $(OUTPUT_DIR)
