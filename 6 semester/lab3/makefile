BUILD_DIR=build
TESTS_DIR=tests
SRC_DIR=src

all: build
	g++ ${SRC_DIR}/main.cpp ${SRC_DIR}/symbol_inversion.cpp -o ${BUILD_DIR}/main -Wall

test: clean_tests
	mkdir ${TESTS_DIR} && g++ ${SRC_DIR}/tests.cpp ${SRC_DIR}/symbol_inversion.cpp -o ${TESTS_DIR}/main -Wall && ${TESTS_DIR}/main

clean_tests:
	rm -rf ${TESTS_DIR}

build:
	mkdir build

clean: clean_tests
	rm -rf build
