#!/usr/bin/env bash
# Show whether MySQL has the ATM database and the accounts table (run inside dev container or on host).
set -euo pipefail
HOST="${DB_HOST:-db}"
PORT="${DB_PORT:-3306}"
USER="${DB_USER:-root}"
export MYSQL_PWD="${DB_PASS:-p}"

echo "=== Connection: ${USER}@${HOST}:${PORT} ==="
if ! mysql -h "$HOST" -P "$PORT" -u "$USER" -e "SELECT 1 AS ok" 2>/dev/null; then
  echo "FAILED: cannot reach MySQL. Is the db container running?"
  echo "Inside dev container: host should be 'db'. On host machine: try DB_HOST=127.0.0.1 DB_PORT=3307 ./scripts/check-db.sh"
  unset MYSQL_PWD
  exit 1
fi

echo ""
echo "=== Databases (look for ATM) ==="
mysql -h "$HOST" -P "$PORT" -u "$USER" -e "SHOW DATABASES"

echo ""
echo "=== Tables in ATM ==="
mysql -h "$HOST" -P "$PORT" -u "$USER" -e "USE ATM; SHOW TABLES" || true

echo ""
echo "=== Row count in accounts (fails if table missing) ==="
mysql -h "$HOST" -P "$PORT" -u "$USER" -e "USE ATM; SELECT COUNT(*) AS account_rows FROM accounts" 2>&1 || {
  echo "(Table accounts does not exist — run: ./scripts/apply-schema.sh)"
  unset MYSQL_PWD
  exit 1
}

unset MYSQL_PWD
echo ""
echo "OK — database looks usable."
