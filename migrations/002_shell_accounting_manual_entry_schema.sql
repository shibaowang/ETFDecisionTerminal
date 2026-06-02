-- ShellAccounting manual entry schema migration.
-- This file is DDL-only and must be applied after 001_initial_schema.sql.

ALTER TABLE trade_log ADD COLUMN request_id TEXT;
ALTER TABLE trade_log ADD COLUMN idempotency_key TEXT;
ALTER TABLE trade_log ADD COLUMN occurred_at_utc TEXT;
ALTER TABLE trade_log ADD COLUMN tax_cents INTEGER NOT NULL DEFAULT 0;
ALTER TABLE trade_log ADD COLUMN source_memo_sanitized TEXT;
ALTER TABLE trade_log ADD COLUMN cash_adjustment_uid TEXT;

ALTER TABLE cash_adjustment ADD COLUMN request_id TEXT;
ALTER TABLE cash_adjustment ADD COLUMN idempotency_key TEXT;
ALTER TABLE cash_adjustment ADD COLUMN occurred_at_utc TEXT;
ALTER TABLE cash_adjustment ADD COLUMN source_memo_sanitized TEXT;
ALTER TABLE cash_adjustment ADD COLUMN trade_log_uid TEXT;

ALTER TABLE audit_log ADD COLUMN request_id TEXT;
ALTER TABLE audit_log ADD COLUMN idempotency_key TEXT;
ALTER TABLE audit_log ADD COLUMN payload_classification TEXT;
ALTER TABLE audit_log ADD COLUMN redaction_status TEXT;

CREATE INDEX IF NOT EXISTS idx_trade_log_manual_request_id
ON trade_log(request_id);

CREATE UNIQUE INDEX IF NOT EXISTS ux_trade_log_manual_idempotency_key
ON trade_log(idempotency_key)
WHERE idempotency_key IS NOT NULL AND idempotency_key <> '';

CREATE INDEX IF NOT EXISTS idx_trade_log_cash_adjustment_uid
ON trade_log(cash_adjustment_uid);

CREATE INDEX IF NOT EXISTS idx_cash_adjustment_manual_request_id
ON cash_adjustment(request_id);

CREATE UNIQUE INDEX IF NOT EXISTS ux_cash_adjustment_manual_idempotency_key
ON cash_adjustment(idempotency_key)
WHERE idempotency_key IS NOT NULL AND idempotency_key <> '';

CREATE INDEX IF NOT EXISTS idx_cash_adjustment_trade_log_uid
ON cash_adjustment(trade_log_uid);

CREATE INDEX IF NOT EXISTS idx_audit_log_manual_request_id
ON audit_log(request_id);

CREATE INDEX IF NOT EXISTS idx_audit_log_manual_idempotency_key
ON audit_log(idempotency_key);
