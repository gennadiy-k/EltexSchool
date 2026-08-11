FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Устанавливаем GCC, Make и системные утилиты
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    net-tools \
    iproute2 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

CMD ["/bin/bash"]
