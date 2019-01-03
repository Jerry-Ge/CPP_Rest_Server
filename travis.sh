#!/bin/bash
# Install npm test runner
echo -e "\e[36mInstalling newman..."
npm install newman
curl -O http://chalonverse.com/435/pa7.tar.gz
tar xzf pa7.tar.gz
echo -e "\e[36mDownloading IMDB data..."
curl -O https://datasets.imdbws.com/title.basics.tsv.gz
gunzip title.basics.tsv.gz

# Cmake into build directory
echo -e "\e[36mCompiling..."
mkdir build
cd build
RELEASE=ON CC=clang CXX=clang++ cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. || exit 1
make || exit 1
echo -e "\e[36mRunning clang-tidy..."
../run-clang-tidy.py -quiet -header-filter=".*/src/[a-zA-Z].*" || exit 1
# Return to root folder (so cwd is correct)
cd ..

# Try to spin up server
build/main title.basics.tsv &

# Try to connect to server
echo -e "\e[36mWaiting for server to start up..."
wget --tries=7 --waitretry=2 --retry-connrefused http://localhost:12345/movie/id/tt0092099 || exit 1

# Run graded tests
echo -e "\e[36mRunning tests..."
timeout 30 node_modules/.bin/newman run postman/pa7-tests.postman_collection.json --color on || exit 1
