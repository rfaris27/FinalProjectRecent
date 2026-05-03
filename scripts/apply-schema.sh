#!/usr/bin/env bash
# Apply schema.sql to the ATM database (creates `accounts` + seed rows).
# Works inside the Dev Container (mysql -> host db) or on the host (docker compose -> db service).
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SCHEMA="$ROOT/schema.sql"

apply_with_mysql_client() {
  local host="${DB_HOST:-db}"
  local port="${DB_PORT:-3306}"
  local user="${DB_USER:-root}"
  local pass="${DB_PASS:-p}"
  export MYSQL_PWD="$pass"
  echo "Waiting for MySQL at ${host}:${port}..."
  local ready=0
  for _ in $(seq 1 30); do
    if mysql -h "$host" -P "$port" -u "$user" -e "SELECT 1" 2>/dev/null; then
      ready=1
      break
    fi
    sleep 2
  done
  if [[ "$ready" != 1 ]]; then
    echo "ERROR: MySQL not reachable at ${host}:${port}"
    return 1
  fi
  mysql -h "$host" -P "$port" -u "$user" ATM < "$SCHEMA"
  echo "Verifying..."
  mysql -h "$host" -P "$port" -u "$user" -e "USE ATM; SHOW TABLES; SELECT AccountNumber, Login, IsAdmin FROM accounts"
  unset MYSQL_PWD
}

apply_with_docker_compose() {
  cd "$ROOT"
  local compose=(docker compose -f .devcontainer/docker-compose.yml)
  "${compose[@]}" up -d db
  echo "Waiting for MySQL (docker)..."
  export MYSQL_PWD="${DB_PASS:-p}"
  for _ in $(seq 1 30); do
    if "${compose[@]}" exec -T db mysqladmin ping -h localhost -u root -pp --silent 2>/dev/null; then
      break
    fi
    sleep 2
  done
  "${compose[@]}" exec -T db mysql -u root -pp ATM < "$SCHEMA"
  echo "Verifying..."
  "${compose[@]}" exec -T db mysql -u root -pp -e "USE ATM; SHOW TABLES; SELECT AccountNumber, Login, IsAdmin FROM accounts"
  unset MYSQL_PWD
}

if [[ ! -f "$SCHEMA" ]]; then
  echo "ERROR: missing $SCHEMA"
  exit 1
fi

if command -v mysql >/dev/null 2>&1; then
  if apply_with_mysql_client; then
    echo "Done. Logins: Adnan123 / 12345 (admin), John456 / 11111 (customer)."
    exit 0
  fi
fi

if command -v docker >/dev/null 2>&1; then
  apply_with_docker_compose
  echo "Done. Logins: Adnan123 / 12345 (admin), John456 / 11111 (customer)."
  exit 0
fi

echo "ERROR: need mysql client (inside dev container) or docker (on host)."
exit 1
