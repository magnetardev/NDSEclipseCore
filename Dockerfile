FROM emscripten/emsdk

RUN apt update && apt-get -y install meson libsdl2-dev libpcap-dev libgtk-3-dev

CMD ["make", "build"]