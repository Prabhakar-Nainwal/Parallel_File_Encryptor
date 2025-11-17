CXX = g++
# Added -pthread for std::thread
CXXFLAGS = -std=c++17 -g -Wall -I. -Isrc/app/encryptDecrypt -Isrc/app/fileHandling -Isrc/app/processes -pthread

MAIN_TARGET = encrypt_decrypt

MAIN_SRC = main.cpp \
           src/app/processes/ProcessManagement.cpp \
           src/app/fileHandling/IO.cpp \
           src/app/fileHandling/ReadEnv.cpp \
           src/app/encryptDecrypt/Cryption.cpp

MAIN_OBJ = $(MAIN_SRC:.cpp=.o)

# Updated 'all' to only build the main target
all: $(MAIN_TARGET)

$(MAIN_TARGET): $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Updated 'clean' to only clean the main target
clean:
	rm -f $(MAIN_OBJ) $(MAIN_TARGET)

.PHONY: clean all
