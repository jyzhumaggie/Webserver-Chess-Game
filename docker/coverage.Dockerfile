# Define builder stage
FROM chickens-and-more:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN pwd
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage