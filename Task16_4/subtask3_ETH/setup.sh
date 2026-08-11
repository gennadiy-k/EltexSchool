#!/bin/bash

# Сбрасываем старое окружение
docker compose down

# Чистим старые бинарники
make clean 2>/dev/null

echo "=== Запуск сервера ==="
docker compose up -d eth_server

echo "Ожидание запуска сервера..."
sleep 3

echo "=== Запуск клиента и вывод логов ==="
docker compose up eth_client

echo "=== Очистка ==="
docker compose down
