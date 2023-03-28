# Use an official Ubuntu runtime as a parent image
FROM ubuntu:latest

# Set the working dir to /app
WORKDIR /app

# Copy the current dir contents into the container at /app
COPY . /app

# Install any needed packages
RUN apt-get update && \
  apt-get install -y g++

# Compile the C++ program
RUN g++ -o main main.cpp

# Set the default command to run the executable
CMD ["./main"]
