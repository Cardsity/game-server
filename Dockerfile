FROM alpine

# environment variables
ENV USE_SYSTEM_CURL ON

# make the 'app' folder the current working directory
WORKDIR /app

# copy the source to the current working directory
COPY ./src .

# install project dependencies, compile it and remove them afterwards
RUN apk update && \
    buildDependencies='build-base git libc6-compat gcompat cmake ninja curl-dev openssl-dev boost-dev websocket++ asio-dev' && \
    runtimeDependencies='libcurl libstdc++ libgcc' && \
    apk add --no-cache $buildDependencies && \
    git clone https://github.com/whoshuu/cpr.git && \
    cd cpr && \
    cmake . && \
    make && \
    make install && \
    cd .. && rm -rf cpr && \
    g++ -o cardsity *.cpp -std=c++17 -fpermissive -pthread -lssl -lcrypto -lcpr -lcurl && \
    apk del $buildDependencies && \
    apk add --no-cache $runtimeDependencies;


EXPOSE 9012
STOPSIGNAL SIGTERM
CMD [ "/app/cardsity" ]
