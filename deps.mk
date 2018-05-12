# Targets to fetch and build external dependencies

# Get and build libelfin
$(ROOT)/deps/gtest:
	@echo $(LOG_PREFIX) Downloading gtest $(LOG_SUFFIX)
	@mkdir -p $(ROOT)/deps
	@cd $(ROOT)/deps; \
		wget https://github.com/google/googletest/archive/release-1.7.0.tar.gz; \
		tar xzf release-1.7.0.tar.gz; \
		rm release-1.7.0.tar.gz; \
		mv googletest-release-1.7.0 gtest

# Update build settings to include the gtest framework
ifneq (,$(findstring gtest,$(PREREQS)))
CXXFLAGS += -isystem $(ROOT)/deps/gtest/include/
LDFLAGS += -isystem $(ROOT)/deps/gtest/ \
					 -isystem $(ROOT)/deps/gtest/include/ \
					 $(ROOT)/deps/gtest/src/gtest-all.cc \
					 $(ROOT)/deps/gtest/src/gtest_main.cc
endif
