-- migration_001_initial_schema.sql
-- 跨境 ETF 智能投资决策终端
-- 初始数据库结构 v0.1
--
-- 设计原则：
-- 1. SQLite + WAL
-- 2. ETFDataService 是唯一写入口
-- 3. trade_log 是唯一事实账本
-- 4. 金额、价格、份额使用整数定点
-- 5. 所有核心账务表带 account_id + portfolio_id
-- 6. 原始 trade_log 不物理删除，纠错走 CORRECTION / REVERSAL / voided

PRAGMA foreign_keys = ON;
PRAGMA journal_mode = WAL;
PRAGMA synchronous = NORMAL;

BEGIN IMMEDIATE;

-- =========================================================
-- 01. 系统与配置
-- =========================================================

CREATE TABLE IF NOT EXISTS schema_version (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    version INTEGER NOT NULL,
    migration_name TEXT NOT NULL,
    checksum TEXT,
    applied_at_utc TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS app_setting (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    key TEXT NOT NULL UNIQUE,
    value TEXT,
    value_type TEXT NOT NULL DEFAULT 'TEXT',
    memo TEXT,
    created_at_utc TEXT NOT NULL,
    updated_at_utc TEXT
);

CREATE TABLE IF NOT EXISTS secure_setting (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    key TEXT NOT NULL UNIQUE,
    encrypted_value TEXT NOT NULL,
    encryption_method TEXT NOT NULL,
    memo TEXT,
    created_at_utc TEXT NOT NULL,
    updated_at_utc TEXT
);

-- =========================================================
-- 02. 账户与组合
-- =========================================================

CREATE TABLE IF NOT EXISTS account (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL,
    account_type TEXT NOT NULL CHECK (account_type IN ('REAL', 'SIMULATED')),
    broker_name TEXT,
    base_currency TEXT NOT NULL DEFAULT 'CNY',
    is_active INTEGER NOT NULL DEFAULT 0 CHECK (is_active IN (0, 1)),
    initial_cash_cents INTEGER NOT NULL DEFAULT 0,
    memo TEXT,
    created_at_utc TEXT NOT NULL,
    updated_at_utc TEXT
);

CREATE TABLE IF NOT EXISTS portfolio (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL,
    description TEXT,
    risk_level TEXT,
    base_position_ratio_ppm INTEGER NOT NULL DEFAULT 200000,
    is_active INTEGER NOT NULL DEFAULT 0 CHECK (is_active IN (0, 1)),
    draft_expire_seconds_etf_buy INTEGER NOT NULL DEFAULT 300,
    draft_expire_seconds_etf_sell INTEGER NOT NULL DEFAULT 300,
    draft_expire_seconds_base INTEGER NOT NULL DEFAULT 900,
    draft_expire_seconds_grid INTEGER NOT NULL DEFAULT 300,
    draft_expire_seconds_take_profit INTEGER NOT NULL DEFAULT 300,
    draft_expire_otc_same_day INTEGER NOT NULL DEFAULT 1 CHECK (draft_expire_otc_same_day IN (0, 1)),
    created_at_utc TEXT NOT NULL,
    updated_at_utc TEXT
);

CREATE TABLE IF NOT EXISTS portfolio_target (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    portfolio_id INTEGER NOT NULL,
    strategy_code TEXT NOT NULL,
    target_weight_ppm INTEGER NOT NULL,
    max_weight_ppm INTEGER NOT NULL,
    priority INTEGER NOT NULL DEFAULT 100,
    enabled INTEGER NOT NULL DEFAULT 1 CHECK (enabled IN (0, 1)),
    rebalance_group TEXT,
    created_at_utc TEXT NOT NULL,
    updated_at_utc TEXT,
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id)
);

CREATE TABLE IF NOT EXISTS portfolio_target_version (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    portfolio_target_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    strategy_code TEXT NOT NULL,
    target_weight_ppm INTEGER NOT NULL,
    max_weight_ppm INTEGER NOT NULL,
    priority INTEGER NOT NULL,
    enabled INTEGER NOT NULL CHECK (enabled IN (0, 1)),
    version_no INTEGER NOT NULL,
    change_reason TEXT,
    created_at_utc TEXT NOT NULL,
    FOREIGN KEY (portfolio_target_id) REFERENCES portfolio_target(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id)
);

-- =========================================================
-- 03. 标的、策略、场外通道、交易日历
-- =========================================================

CREATE TABLE IF NOT EXISTS instrument (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    code TEXT NOT NULL,
    name TEXT NOT NULL,
    instrument_type TEXT NOT NULL CHECK (instrument_type IN ('ETF', 'INDEX', 'OTC_FUND', 'CASH')),
    market_code TEXT NOT NULL CHECK (market_code IN ('CN_A', 'US', 'FUND_OTC', 'CASH')),
    currency TEXT NOT NULL DEFAULT 'CNY',
    enabled INTEGER NOT NULL DEFAULT 1 CHECK (enabled IN (0, 1)),
    memo TEXT,
    created_at_utc TEXT NOT NULL,
    updated_at_utc TEXT
);

CREATE TABLE IF NOT EXISTS strategy (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    strategy_code TEXT NOT NULL UNIQUE,
    instrument_id INTEGER NOT NULL,
    name TEXT NOT NULL,
    enabled INTEGER NOT NULL DEFAULT 1 CHECK (enabled IN (0, 1)),
    external_premium_threshold_ppm INTEGER NOT NULL DEFAULT 0,
    take_profit_premium_threshold_ppm INTEGER NOT NULL DEFAULT 0,
    return_take_profit_ratio_ppm INTEGER NOT NULL DEFAULT 0,
    add_premium_limit_ppm INTEGER NOT NULL DEFAULT 0,
    adjustment_factor_1e6 INTEGER NOT NULL DEFAULT 1000000,
    memo TEXT,
    created_at_utc TEXT NOT NULL,
    updated_at_utc TEXT,
    FOREIGN KEY (instrument_id) REFERENCES instrument(id)
);

CREATE TABLE IF NOT EXISTS strategy_version (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    strategy_id INTEGER NOT NULL,
    strategy_code TEXT NOT NULL,
    version_no INTEGER NOT NULL,
    external_premium_threshold_ppm INTEGER NOT NULL DEFAULT 0,
    take_profit_premium_threshold_ppm INTEGER NOT NULL DEFAULT 0,
    return_take_profit_ratio_ppm INTEGER NOT NULL DEFAULT 0,
    add_premium_limit_ppm INTEGER NOT NULL DEFAULT 0,
    adjustment_factor_1e6 INTEGER NOT NULL DEFAULT 1000000,
    change_reason TEXT,
    created_at_utc TEXT NOT NULL,
    FOREIGN KEY (strategy_id) REFERENCES strategy(id)
);

CREATE TABLE IF NOT EXISTS strategy_tier_rule (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    strategy_id INTEGER NOT NULL,
    strategy_code TEXT NOT NULL,
    tier_no INTEGER NOT NULL CHECK (tier_no BETWEEN 1 AND 12),
    tier_name TEXT NOT NULL,
    drawdown_threshold_ppm INTEGER NOT NULL,
    capital_weight INTEGER NOT NULL,
    enabled INTEGER NOT NULL DEFAULT 1 CHECK (enabled IN (0, 1)),
    created_at_utc TEXT NOT NULL,
    updated_at_utc TEXT,
    FOREIGN KEY (strategy_id) REFERENCES strategy(id)
);

CREATE TABLE IF NOT EXISTS otc_channel (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    strategy_id INTEGER NOT NULL,
    strategy_code TEXT NOT NULL,
    actual_instrument_id INTEGER NOT NULL,
    actual_code TEXT NOT NULL,
    fund_class TEXT NOT NULL CHECK (fund_class IN ('A', 'C', 'OTHER')),
    enabled INTEGER NOT NULL DEFAULT 1 CHECK (enabled IN (0, 1)),
    daily_limit_cents INTEGER NOT NULL DEFAULT 0,
    priority INTEGER NOT NULL DEFAULT 999,
    min_buy_amount_cents INTEGER NOT NULL DEFAULT 1,
    memo TEXT,
    created_at_utc TEXT NOT NULL,
    updated_at_utc TEXT,
    FOREIGN KEY (strategy_id) REFERENCES strategy(id),
    FOREIGN KEY (actual_instrument_id) REFERENCES instrument(id)
);

CREATE TABLE IF NOT EXISTS market_calendar (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    market_code TEXT NOT NULL CHECK (market_code IN ('CN_A', 'US', 'FUND_OTC', 'CASH')),
    trading_date TEXT NOT NULL,
    is_open INTEGER NOT NULL CHECK (is_open IN (0, 1)),
    open_time_local TEXT,
    close_time_local TEXT,
    session_type TEXT,
    memo TEXT,
    created_at_utc TEXT NOT NULL,
    updated_at_utc TEXT
);

-- =========================================================
-- 04. 行情
-- =========================================================

CREATE TABLE IF NOT EXISTS market_quote_current (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    instrument_id INTEGER NOT NULL,
    code TEXT NOT NULL,
    provider TEXT NOT NULL,
    market_status TEXT NOT NULL CHECK (market_status IN ('REALTIME', 'DELAYED', 'STALE', 'FAILED', 'MARKET_CLOSED', 'FUND_NAV_PENDING', 'UNKNOWN')),
    last_price_1e6 INTEGER,
    prev_close_1e6 INTEGER,
    iopv_1e6 INTEGER,
    premium_ratio_ppm INTEGER,
    change_ratio_ppm INTEGER,
    index_point_1e6 INTEGER,
    high_point_1e6 INTEGER,
    drawdown_ratio_ppm INTEGER,
    quote_time_utc TEXT,
    trading_date TEXT,
    updated_at_utc TEXT NOT NULL,
    FOREIGN KEY (instrument_id) REFERENCES instrument(id)
);

CREATE TABLE IF NOT EXISTS market_quote_history (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    instrument_id INTEGER NOT NULL,
    code TEXT NOT NULL,
    provider TEXT NOT NULL,
    snapshot_type TEXT NOT NULL CHECK (snapshot_type IN ('SIGNAL', 'DRAFT', 'CLOSE', 'MANUAL', 'PERIODIC_SAMPLE')),
    market_status TEXT NOT NULL CHECK (market_status IN ('REALTIME', 'DELAYED', 'STALE', 'FAILED', 'MARKET_CLOSED', 'FUND_NAV_PENDING', 'UNKNOWN')),
    last_price_1e6 INTEGER,
    prev_close_1e6 INTEGER,
    iopv_1e6 INTEGER,
    premium_ratio_ppm INTEGER,
    change_ratio_ppm INTEGER,
    index_point_1e6 INTEGER,
    high_point_1e6 INTEGER,
    drawdown_ratio_ppm INTEGER,
    quote_time_utc TEXT,
    trading_date TEXT,
    created_at_utc TEXT NOT NULL,
    FOREIGN KEY (instrument_id) REFERENCES instrument(id)
);

-- =========================================================
-- 05. TradeDraft 交易建议
-- =========================================================

CREATE TABLE IF NOT EXISTS trade_draft (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    account_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    strategy_id INTEGER NOT NULL,
    strategy_code TEXT NOT NULL,
    strategy_version_id INTEGER,
    portfolio_target_version_id INTEGER,
    draft_group_no TEXT,
    action_type TEXT NOT NULL CHECK (action_type IN ('BUY', 'SELL')),
    trade_source TEXT NOT NULL CHECK (trade_source IN ('EXCHANGE_DIRECT', 'OTC_SUBSTITUTE', 'CASH', 'SYSTEM', 'MANUAL')),
    trade_intent_type TEXT NOT NULL CHECK (trade_intent_type IN ('BASE_INIT', 'BASE_FILL', 'BASE_REPAIR', 'GRID_BUY', 'TAKE_PROFIT', 'PREMIUM_SELL', 'RISK_SELL', 'NORMAL_BUY', 'NORMAL_SELL', 'CASH_EVENT', 'CYCLE_CLOSE', 'CORRECTION', 'MANUAL_TEMPLATE')),
    trigger_tier_no INTEGER,
    trigger_reason TEXT,
    status TEXT NOT NULL CHECK (status IN ('ACTIVE', 'CONFIRMED', 'EXPIRED', 'CANCELED', 'SUPERSEDED')),
    total_amount_cents INTEGER NOT NULL DEFAULT 0,
    total_quantity_1e6 INTEGER NOT NULL DEFAULT 0,
    draft_signature TEXT NOT NULL,
    market_status TEXT NOT NULL CHECK (market_status IN ('REALTIME', 'DELAYED', 'STALE', 'FAILED', 'MARKET_CLOSED', 'FUND_NAV_PENDING', 'UNKNOWN')),
    created_at_utc TEXT NOT NULL,
    last_seen_at_utc TEXT,
    expires_at_utc TEXT,
    confirmed_at_utc TEXT,
    expired_at_utc TEXT,
    superseded_at_utc TEXT,
    superseded_by_draft_id INTEGER,
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id),
    FOREIGN KEY (strategy_id) REFERENCES strategy(id),
    FOREIGN KEY (strategy_version_id) REFERENCES strategy_version(id),
    FOREIGN KEY (portfolio_target_version_id) REFERENCES portfolio_target_version(id),
    FOREIGN KEY (superseded_by_draft_id) REFERENCES trade_draft(id)
);

CREATE TABLE IF NOT EXISTS trade_draft_leg (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    draft_id INTEGER NOT NULL,
    strategy_code TEXT NOT NULL,
    actual_instrument_id INTEGER,
    actual_code TEXT,
    fund_class TEXT,
    action_type TEXT NOT NULL CHECK (action_type IN ('BUY', 'SELL')),
    trade_source TEXT NOT NULL CHECK (trade_source IN ('EXCHANGE_DIRECT', 'OTC_SUBSTITUTE', 'CASH', 'SYSTEM', 'MANUAL')),
    suggested_price_1e6 INTEGER,
    suggested_quantity_1e6 INTEGER,
    suggested_amount_cents INTEGER NOT NULL DEFAULT 0,
    suggested_fee_cents INTEGER NOT NULL DEFAULT 0,
    tier_no INTEGER,
    tier_name TEXT,
    memo TEXT,
    created_at_utc TEXT NOT NULL,
    FOREIGN KEY (draft_id) REFERENCES trade_draft(id),
    FOREIGN KEY (actual_instrument_id) REFERENCES instrument(id)
);

CREATE TABLE IF NOT EXISTS trade_draft_context (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    draft_id INTEGER NOT NULL,
    account_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    strategy_code TEXT NOT NULL,
    price_1e6 INTEGER,
    iopv_1e6 INTEGER,
    premium_ratio_ppm INTEGER,
    index_point_1e6 INTEGER,
    index_drawdown_ratio_ppm INTEGER,
    etf_drawdown_ratio_ppm INTEGER,
    holding_cost_cents INTEGER NOT NULL DEFAULT 0,
    holding_quantity_1e6 INTEGER NOT NULL DEFAULT 0,
    principal_base_cents INTEGER NOT NULL DEFAULT 0,
    cash_balance_cents INTEGER NOT NULL DEFAULT 0,
    base_target_cents INTEGER NOT NULL DEFAULT 0,
    sniper_budget_cents INTEGER NOT NULL DEFAULT 0,
    triggered_tier_no INTEGER,
    market_status TEXT NOT NULL CHECK (market_status IN ('REALTIME', 'DELAYED', 'STALE', 'FAILED', 'MARKET_CLOSED', 'FUND_NAV_PENDING', 'UNKNOWN')),
    strategy_version_id INTEGER,
    portfolio_target_version_id INTEGER,
    created_at_utc TEXT NOT NULL,
    FOREIGN KEY (draft_id) REFERENCES trade_draft(id),
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id),
    FOREIGN KEY (strategy_version_id) REFERENCES strategy_version(id),
    FOREIGN KEY (portfolio_target_version_id) REFERENCES portfolio_target_version(id)
);

-- =========================================================
-- 06. 狙击周期
-- =========================================================

CREATE TABLE IF NOT EXISTS grid_cycle (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    account_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    strategy_id INTEGER NOT NULL,
    strategy_code TEXT NOT NULL,
    cycle_no INTEGER NOT NULL,
    status TEXT NOT NULL CHECK (status IN ('ACTIVE', 'CLOSED', 'CANCELED')),
    start_time_utc TEXT NOT NULL,
    end_time_utc TEXT,
    start_cash_cents INTEGER NOT NULL DEFAULT 0,
    end_cash_cents INTEGER,
    start_principal_base_cents INTEGER NOT NULL DEFAULT 0,
    end_principal_base_cents INTEGER,
    start_sniper_budget_cents INTEGER NOT NULL DEFAULT 0,
    adjusted_sniper_budget_cents INTEGER,
    budget_locked INTEGER NOT NULL DEFAULT 1 CHECK (budget_locked IN (0, 1)),
    budget_adjust_count INTEGER NOT NULL DEFAULT 0,
    last_budget_adjust_time_utc TEXT,
    budget_adjust_reason TEXT,
    realized_pnl_cents INTEGER NOT NULL DEFAULT 0,
    memo TEXT,
    created_at_utc TEXT NOT NULL,
    updated_at_utc TEXT,
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id),
    FOREIGN KEY (strategy_id) REFERENCES strategy(id)
);

-- =========================================================
-- 07. 成交与账务
-- =========================================================

CREATE TABLE IF NOT EXISTS trade_execution_group (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    account_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    strategy_id INTEGER,
    strategy_code TEXT,
    grid_cycle_id INTEGER,
    draft_id INTEGER,
    original_draft_id INTEGER,
    action_type TEXT NOT NULL CHECK (action_type IN ('BUY', 'SELL', 'DIVIDEND', 'BONUS_SHARE', 'SPLIT', 'MERGE', 'CASH_IN', 'CASH_OUT', 'CASH_ADJUSTMENT', 'CYCLE_EVENT', 'CORRECTION', 'REVERSAL')),
    trade_source TEXT NOT NULL CHECK (trade_source IN ('EXCHANGE_DIRECT', 'OTC_SUBSTITUTE', 'CASH', 'SYSTEM', 'MANUAL')),
    trade_intent_type TEXT NOT NULL CHECK (trade_intent_type IN ('BASE_INIT', 'BASE_FILL', 'BASE_REPAIR', 'GRID_BUY', 'TAKE_PROFIT', 'PREMIUM_SELL', 'RISK_SELL', 'NORMAL_BUY', 'NORMAL_SELL', 'CASH_EVENT', 'CYCLE_CLOSE', 'CORRECTION')),
    trigger_tier_no INTEGER,
    total_amount_cents INTEGER NOT NULL DEFAULT 0,
    total_quantity_1e6 INTEGER NOT NULL DEFAULT 0,
    confirm_mode TEXT NOT NULL CHECK (confirm_mode IN ('DRAFT_CONFIRM', 'MANUAL_ENTRY', 'MANUAL_FROM_EXPIRED_DRAFT', 'MANUAL_FROM_SUPERSEDED_DRAFT')),
    manual_entry INTEGER NOT NULL DEFAULT 0 CHECK (manual_entry IN (0, 1)),
    manual_reason TEXT,
    base_violation INTEGER NOT NULL DEFAULT 0 CHECK (base_violation IN (0, 1)),
    risk_event_type TEXT,
    created_at_utc TEXT NOT NULL,
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id),
    FOREIGN KEY (strategy_id) REFERENCES strategy(id),
    FOREIGN KEY (grid_cycle_id) REFERENCES grid_cycle(id),
    FOREIGN KEY (draft_id) REFERENCES trade_draft(id),
    FOREIGN KEY (original_draft_id) REFERENCES trade_draft(id)
);

CREATE TABLE IF NOT EXISTS trade_log (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    account_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    trade_execution_group_id INTEGER,
    grid_cycle_id INTEGER,
    strategy_id INTEGER,
    strategy_code TEXT,
    actual_instrument_id INTEGER,
    actual_code TEXT,
    action_type TEXT NOT NULL CHECK (action_type IN ('BUY', 'SELL', 'DIVIDEND', 'BONUS_SHARE', 'SPLIT', 'MERGE', 'CASH_IN', 'CASH_OUT', 'CASH_ADJUSTMENT', 'CYCLE_EVENT', 'CORRECTION', 'REVERSAL')),
    trade_source TEXT NOT NULL CHECK (trade_source IN ('EXCHANGE_DIRECT', 'OTC_SUBSTITUTE', 'CASH', 'SYSTEM', 'MANUAL')),
    trade_intent_type TEXT CHECK (trade_intent_type IN ('BASE_INIT', 'BASE_FILL', 'BASE_REPAIR', 'GRID_BUY', 'TAKE_PROFIT', 'PREMIUM_SELL', 'RISK_SELL', 'NORMAL_BUY', 'NORMAL_SELL', 'CASH_EVENT', 'CYCLE_CLOSE', 'CORRECTION')),
    price_1e6 INTEGER,
    quantity_1e6 INTEGER NOT NULL DEFAULT 0,
    amount_cents INTEGER NOT NULL DEFAULT 0,
    fee_cents INTEGER NOT NULL DEFAULT 0,
    net_cash_impact_cents INTEGER NOT NULL DEFAULT 0,
    principal_base_after_cents INTEGER,
    cash_balance_after_cents INTEGER,
    total_assets_after_cents INTEGER,
    tier_no INTEGER,
    tier_name TEXT,
    memo TEXT,
    manual_entry INTEGER NOT NULL DEFAULT 0 CHECK (manual_entry IN (0, 1)),
    correction_of_log_id INTEGER,
    reversal_of_log_id INTEGER,
    voided INTEGER NOT NULL DEFAULT 0 CHECK (voided IN (0, 1)),
    void_reason TEXT,
    local_time TEXT,
    trading_date TEXT,
    created_at_utc TEXT NOT NULL,
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id),
    FOREIGN KEY (trade_execution_group_id) REFERENCES trade_execution_group(id),
    FOREIGN KEY (grid_cycle_id) REFERENCES grid_cycle(id),
    FOREIGN KEY (strategy_id) REFERENCES strategy(id),
    FOREIGN KEY (actual_instrument_id) REFERENCES instrument(id),
    FOREIGN KEY (correction_of_log_id) REFERENCES trade_log(id),
    FOREIGN KEY (reversal_of_log_id) REFERENCES trade_log(id)
);

CREATE TABLE IF NOT EXISTS risk_event (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    account_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    strategy_id INTEGER,
    strategy_code TEXT,
    risk_event_type TEXT NOT NULL CHECK (risk_event_type IN ('BASE_POSITION_BROKEN', 'ACCOUNTING_INCONSISTENCY', 'DATA_IMPORT_RISK', 'SERVICE_CRITICAL')),
    status TEXT NOT NULL CHECK (status IN ('OPEN', 'RESOLVED', 'CANCELED')),
    severity TEXT NOT NULL CHECK (severity IN ('INFO', 'WARNING', 'ERROR', 'CRITICAL', 'FATAL')),
    trigger_execution_group_id INTEGER,
    trigger_trade_log_id INTEGER,
    message TEXT,
    reason TEXT,
    created_at_utc TEXT NOT NULL,
    resolved_at_utc TEXT,
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id),
    FOREIGN KEY (strategy_id) REFERENCES strategy(id),
    FOREIGN KEY (trigger_execution_group_id) REFERENCES trade_execution_group(id),
    FOREIGN KEY (trigger_trade_log_id) REFERENCES trade_log(id)
);

CREATE TABLE IF NOT EXISTS trade_tier_allocation (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    trade_execution_group_id INTEGER NOT NULL,
    account_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    strategy_id INTEGER NOT NULL,
    strategy_code TEXT NOT NULL,
    grid_cycle_id INTEGER NOT NULL,
    tier_no INTEGER NOT NULL,
    tier_name TEXT NOT NULL,
    allocated_amount_cents INTEGER NOT NULL,
    created_at_utc TEXT NOT NULL,
    FOREIGN KEY (trade_execution_group_id) REFERENCES trade_execution_group(id),
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id),
    FOREIGN KEY (strategy_id) REFERENCES strategy(id),
    FOREIGN KEY (grid_cycle_id) REFERENCES grid_cycle(id)
);

CREATE TABLE IF NOT EXISTS base_position_allocation (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    trade_execution_group_id INTEGER NOT NULL,
    trade_log_id INTEGER NOT NULL,
    account_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    strategy_id INTEGER NOT NULL,
    strategy_code TEXT NOT NULL,
    actual_instrument_id INTEGER,
    actual_code TEXT,
    allocation_type TEXT NOT NULL CHECK (allocation_type IN ('INITIAL_BASE', 'BASE_REPAIR')),
    target_base_amount_cents INTEGER NOT NULL,
    allocated_amount_cents INTEGER NOT NULL,
    base_violation_event_id INTEGER,
    created_at_utc TEXT NOT NULL,
    FOREIGN KEY (trade_execution_group_id) REFERENCES trade_execution_group(id),
    FOREIGN KEY (trade_log_id) REFERENCES trade_log(id),
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id),
    FOREIGN KEY (strategy_id) REFERENCES strategy(id),
    FOREIGN KEY (actual_instrument_id) REFERENCES instrument(id),
    FOREIGN KEY (base_violation_event_id) REFERENCES risk_event(id)
);

CREATE TABLE IF NOT EXISTS cash_adjustment (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    trade_log_id INTEGER NOT NULL,
    account_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    adjustment_type TEXT NOT NULL CHECK (adjustment_type IN ('CASH_IN', 'CASH_OUT', 'CASH_ADJUSTMENT')),
    amount_cents INTEGER NOT NULL,
    reason TEXT NOT NULL,
    external_reference TEXT,
    created_at_utc TEXT NOT NULL,
    FOREIGN KEY (trade_log_id) REFERENCES trade_log(id),
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id)
);

CREATE TABLE IF NOT EXISTS budget_recalc_suggestion (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    account_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    strategy_id INTEGER NOT NULL,
    strategy_code TEXT NOT NULL,
    grid_cycle_id INTEGER NOT NULL,
    trigger_event_type TEXT NOT NULL,
    trigger_trade_log_id INTEGER,
    old_sniper_budget_cents INTEGER NOT NULL,
    suggested_sniper_budget_cents INTEGER NOT NULL,
    status TEXT NOT NULL CHECK (status IN ('PENDING', 'ACCEPTED', 'REJECTED', 'EXPIRED')),
    reason TEXT,
    created_at_utc TEXT NOT NULL,
    handled_at_utc TEXT,
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id),
    FOREIGN KEY (strategy_id) REFERENCES strategy(id),
    FOREIGN KEY (grid_cycle_id) REFERENCES grid_cycle(id),
    FOREIGN KEY (trigger_trade_log_id) REFERENCES trade_log(id)
);

-- =========================================================
-- 08. 资金分配计划
-- =========================================================

CREATE TABLE IF NOT EXISTS cash_allocation_plan_current (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    account_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    strategy_id INTEGER,
    strategy_code TEXT,
    plan_type TEXT NOT NULL CHECK (plan_type IN ('BASE_REPAIR', 'BASE_FILL', 'GRID_BUY', 'SELL', 'SURPLUS')),
    priority INTEGER NOT NULL,
    requested_amount_cents INTEGER NOT NULL DEFAULT 0,
    allocated_amount_cents INTEGER NOT NULL DEFAULT 0,
    blocked_amount_cents INTEGER NOT NULL DEFAULT 0,
    block_reason TEXT CHECK (block_reason IN ('CASH_LIMITED', 'PREMIUM_BLOCKED', 'OTC_LIMIT', 'BASE_PROTECTION', 'MAX_WEIGHT_LIMITED', 'NONE')),
    related_grid_cycle_id INTEGER,
    related_draft_id INTEGER,
    plan_signature TEXT NOT NULL,
    last_seen_at_utc TEXT,
    created_at_utc TEXT NOT NULL,
    updated_at_utc TEXT,
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id),
    FOREIGN KEY (strategy_id) REFERENCES strategy(id),
    FOREIGN KEY (related_grid_cycle_id) REFERENCES grid_cycle(id),
    FOREIGN KEY (related_draft_id) REFERENCES trade_draft(id)
);

CREATE TABLE IF NOT EXISTS cash_allocation_plan_history (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    account_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    strategy_id INTEGER,
    strategy_code TEXT,
    plan_type TEXT NOT NULL,
    priority INTEGER NOT NULL,
    requested_amount_cents INTEGER NOT NULL DEFAULT 0,
    allocated_amount_cents INTEGER NOT NULL DEFAULT 0,
    blocked_amount_cents INTEGER NOT NULL DEFAULT 0,
    block_reason TEXT,
    related_grid_cycle_id INTEGER,
    related_draft_id INTEGER,
    change_type TEXT NOT NULL CHECK (change_type IN ('CREATED', 'UPDATED', 'REMOVED')),
    old_signature TEXT,
    new_signature TEXT,
    created_at_utc TEXT NOT NULL,
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id),
    FOREIGN KEY (strategy_id) REFERENCES strategy(id),
    FOREIGN KEY (related_grid_cycle_id) REFERENCES grid_cycle(id),
    FOREIGN KEY (related_draft_id) REFERENCES trade_draft(id)
);

-- =========================================================
-- 09. 派生快照
-- =========================================================

CREATE TABLE IF NOT EXISTS position_snapshot (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    account_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    strategy_id INTEGER,
    strategy_code TEXT,
    actual_instrument_id INTEGER,
    actual_code TEXT,
    trade_source TEXT NOT NULL CHECK (trade_source IN ('EXCHANGE_DIRECT', 'OTC_SUBSTITUTE', 'CASH', 'SYSTEM', 'MANUAL')),
    quantity_1e6 INTEGER NOT NULL DEFAULT 0,
    cost_amount_cents INTEGER NOT NULL DEFAULT 0,
    market_value_cents INTEGER NOT NULL DEFAULT 0,
    unrealized_pnl_cents INTEGER NOT NULL DEFAULT 0,
    updated_at_utc TEXT NOT NULL,
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id),
    FOREIGN KEY (strategy_id) REFERENCES strategy(id),
    FOREIGN KEY (actual_instrument_id) REFERENCES instrument(id)
);

CREATE TABLE IF NOT EXISTS cash_snapshot (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    account_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    cash_balance_cents INTEGER NOT NULL DEFAULT 0,
    principal_base_cents INTEGER NOT NULL DEFAULT 0,
    updated_at_utc TEXT NOT NULL,
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id)
);

CREATE TABLE IF NOT EXISTS portfolio_summary (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    account_id INTEGER NOT NULL,
    portfolio_id INTEGER NOT NULL,
    total_assets_cents INTEGER NOT NULL DEFAULT 0,
    total_market_value_cents INTEGER NOT NULL DEFAULT 0,
    cash_balance_cents INTEGER NOT NULL DEFAULT 0,
    principal_base_cents INTEGER NOT NULL DEFAULT 0,
    holding_cost_cents INTEGER NOT NULL DEFAULT 0,
    total_pnl_cents INTEGER NOT NULL DEFAULT 0,
    total_return_ppm INTEGER NOT NULL DEFAULT 0,
    base_completion_ratio_ppm INTEGER NOT NULL DEFAULT 0,
    sniper_pool_cents INTEGER NOT NULL DEFAULT 0,
    active_grid_cycle_count INTEGER NOT NULL DEFAULT 0,
    active_draft_count INTEGER NOT NULL DEFAULT 0,
    base_violation_count INTEGER NOT NULL DEFAULT 0,
    updated_at_utc TEXT NOT NULL,
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id)
);

-- =========================================================
-- 10. 预警与日志
-- =========================================================

CREATE TABLE IF NOT EXISTS alert_event (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    account_id INTEGER,
    portfolio_id INTEGER,
    strategy_id INTEGER,
    strategy_code TEXT,
    signal_key TEXT NOT NULL,
    signal_type TEXT NOT NULL,
    level TEXT NOT NULL CHECK (level IN ('INFO', 'WARNING', 'ERROR', 'CRITICAL', 'FATAL')),
    channel TEXT NOT NULL CHECK (channel IN ('UI', 'DESKTOP', 'PUSHPLUS')),
    status TEXT NOT NULL CHECK (status IN ('PENDING', 'SENT', 'FAILED', 'SUPPRESSED')),
    message TEXT NOT NULL,
    cooldown_seconds INTEGER NOT NULL DEFAULT 0,
    last_sent_at_utc TEXT,
    created_at_utc TEXT NOT NULL,
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id),
    FOREIGN KEY (strategy_id) REFERENCES strategy(id)
);

CREATE TABLE IF NOT EXISTS audit_log (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    account_id INTEGER,
    portfolio_id INTEGER,
    entity_type TEXT NOT NULL,
    entity_id INTEGER,
    action TEXT NOT NULL,
    old_value_json TEXT,
    new_value_json TEXT,
    reason TEXT,
    operator TEXT,
    created_at_utc TEXT NOT NULL,
    FOREIGN KEY (account_id) REFERENCES account(id),
    FOREIGN KEY (portfolio_id) REFERENCES portfolio(id)
);

CREATE TABLE IF NOT EXISTS system_log (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    service_name TEXT NOT NULL,
    level TEXT NOT NULL CHECK (level IN ('INFO', 'WARNING', 'ERROR', 'CRITICAL', 'FATAL')),
    event_type TEXT NOT NULL,
    message TEXT NOT NULL,
    trace_id TEXT,
    created_at_utc TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS error_log (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    service_name TEXT NOT NULL,
    error_code TEXT NOT NULL,
    level TEXT NOT NULL CHECK (level IN ('INFO', 'WARNING', 'ERROR', 'CRITICAL', 'FATAL')),
    message TEXT NOT NULL,
    stack_trace TEXT,
    trace_id TEXT,
    created_at_utc TEXT NOT NULL
);

-- =========================================================
-- 11. Excel V8.2 导入 staging
-- =========================================================

CREATE TABLE IF NOT EXISTS import_job (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    source_file_path TEXT NOT NULL,
    source_file_hash TEXT,
    import_type TEXT NOT NULL CHECK (import_type IN ('EXCEL_V8_2')),
    status TEXT NOT NULL CHECK (status IN ('LOADED', 'VALIDATED', 'CONFIRMED', 'IMPORTED', 'FAILED', 'CANCELED')),
    target_account_id INTEGER,
    target_portfolio_id INTEGER,
    created_at_utc TEXT NOT NULL,
    updated_at_utc TEXT,
    FOREIGN KEY (target_account_id) REFERENCES account(id),
    FOREIGN KEY (target_portfolio_id) REFERENCES portfolio(id)
);

CREATE TABLE IF NOT EXISTS staging_trade_log (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    import_job_id INTEGER NOT NULL,
    row_no INTEGER NOT NULL,
    raw_time TEXT,
    strategy_code TEXT,
    actual_code TEXT,
    action_text TEXT,
    price_text TEXT,
    quantity_text TEXT,
    amount_text TEXT,
    tier_text TEXT,
    source_text TEXT,
    fee_text TEXT,
    memo TEXT,
    net_cash_impact_text TEXT,
    principal_text TEXT,
    cash_balance_text TEXT,
    total_assets_text TEXT,
    validation_status TEXT,
    validation_message TEXT,
    FOREIGN KEY (import_job_id) REFERENCES import_job(id)
);

CREATE TABLE IF NOT EXISTS staging_strategy (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    import_job_id INTEGER NOT NULL,
    row_no INTEGER NOT NULL,
    strategy_code TEXT,
    name TEXT,
    external_premium_threshold_text TEXT,
    take_profit_premium_threshold_text TEXT,
    return_take_profit_ratio_text TEXT,
    add_premium_limit_text TEXT,
    tier1_weight_text TEXT,
    tier2_weight_text TEXT,
    tier3_weight_text TEXT,
    tier4_weight_text TEXT,
    tier5_weight_text TEXT,
    tier6_weight_text TEXT,
    adjustment_factor_text TEXT,
    validation_status TEXT,
    validation_message TEXT,
    FOREIGN KEY (import_job_id) REFERENCES import_job(id)
);

CREATE TABLE IF NOT EXISTS staging_otc_channel (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    import_job_id INTEGER NOT NULL,
    row_no INTEGER NOT NULL,
    strategy_code TEXT,
    actual_code TEXT,
    fund_class TEXT,
    enabled_text TEXT,
    daily_limit_text TEXT,
    priority_text TEXT,
    min_buy_amount_text TEXT,
    memo TEXT,
    validation_status TEXT,
    validation_message TEXT,
    FOREIGN KEY (import_job_id) REFERENCES import_job(id)
);

CREATE TABLE IF NOT EXISTS import_issue (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL UNIQUE,
    import_job_id INTEGER NOT NULL,
    issue_level TEXT NOT NULL CHECK (issue_level IN ('INFO', 'WARNING', 'ERROR', 'FATAL')),
    issue_type TEXT NOT NULL,
    row_no INTEGER,
    message TEXT NOT NULL,
    created_at_utc TEXT NOT NULL,
    FOREIGN KEY (import_job_id) REFERENCES import_job(id)
);

-- =========================================================
-- 12. 唯一约束与索引
-- =========================================================

CREATE UNIQUE INDEX IF NOT EXISTS ux_account_one_active
ON account(is_active)
WHERE is_active = 1;

CREATE UNIQUE INDEX IF NOT EXISTS ux_portfolio_one_active
ON portfolio(is_active)
WHERE is_active = 1;

CREATE UNIQUE INDEX IF NOT EXISTS ux_instrument_code_market
ON instrument(code, market_code);

CREATE UNIQUE INDEX IF NOT EXISTS ux_portfolio_target
ON portfolio_target(portfolio_id, strategy_code);

CREATE UNIQUE INDEX IF NOT EXISTS ux_strategy_tier_rule
ON strategy_tier_rule(strategy_id, tier_no);

CREATE UNIQUE INDEX IF NOT EXISTS ux_otc_channel
ON otc_channel(strategy_id, actual_code);

CREATE UNIQUE INDEX IF NOT EXISTS ux_market_calendar
ON market_calendar(market_code, trading_date);

CREATE UNIQUE INDEX IF NOT EXISTS ux_market_quote_current
ON market_quote_current(instrument_id, provider);

CREATE UNIQUE INDEX IF NOT EXISTS ux_grid_cycle_active
ON grid_cycle(account_id, portfolio_id, strategy_code)
WHERE status = 'ACTIVE';

CREATE INDEX IF NOT EXISTS idx_trade_log_account_portfolio_time
ON trade_log(account_id, portfolio_id, created_at_utc);

CREATE INDEX IF NOT EXISTS idx_trade_log_strategy_cycle
ON trade_log(account_id, portfolio_id, strategy_code, grid_cycle_id);

CREATE INDEX IF NOT EXISTS idx_trade_log_actual_code
ON trade_log(account_id, portfolio_id, actual_code);

CREATE INDEX IF NOT EXISTS idx_trade_log_execution_group
ON trade_log(trade_execution_group_id);

CREATE INDEX IF NOT EXISTS idx_trade_draft_active
ON trade_draft(account_id, portfolio_id, strategy_code, status);

CREATE INDEX IF NOT EXISTS idx_trade_draft_signature
ON trade_draft(account_id, portfolio_id, strategy_code, draft_signature);

CREATE INDEX IF NOT EXISTS idx_trade_execution_group
ON trade_execution_group(account_id, portfolio_id, strategy_code, created_at_utc);

CREATE INDEX IF NOT EXISTS idx_trade_tier_allocation_cycle
ON trade_tier_allocation(account_id, portfolio_id, strategy_code, grid_cycle_id);

CREATE INDEX IF NOT EXISTS idx_base_position_allocation_strategy
ON base_position_allocation(account_id, portfolio_id, strategy_code);

CREATE INDEX IF NOT EXISTS idx_grid_cycle_status
ON grid_cycle(account_id, portfolio_id, strategy_code, status);

CREATE INDEX IF NOT EXISTS idx_cash_plan_current
ON cash_allocation_plan_current(account_id, portfolio_id, strategy_code, plan_type);

CREATE INDEX IF NOT EXISTS idx_alert_signal_key
ON alert_event(account_id, portfolio_id, strategy_code, signal_key);

CREATE INDEX IF NOT EXISTS idx_market_quote_history
ON market_quote_history(instrument_id, snapshot_type, created_at_utc);

CREATE INDEX IF NOT EXISTS idx_audit_entity
ON audit_log(entity_type, entity_id, created_at_utc);

CREATE INDEX IF NOT EXISTS idx_system_log_time
ON system_log(service_name, created_at_utc);

CREATE INDEX IF NOT EXISTS idx_error_log_time
ON error_log(service_name, created_at_utc);

-- =========================================================
-- 13. 默认初始化数据
-- =========================================================

INSERT OR IGNORE INTO schema_version
(uid, version, migration_name, checksum, applied_at_utc)
VALUES
('00000000-0000-4000-8000-000000000001', 1, 'migration_001_initial_schema', NULL, strftime('%Y-%m-%dT%H:%M:%fZ','now'));

INSERT OR IGNORE INTO account
(uid, name, account_type, broker_name, base_currency, is_active, initial_cash_cents, memo, created_at_utc)
VALUES
('00000000-0000-4000-8000-000000000101', '默认真实账户', 'REAL', 'MANUAL', 'CNY', 1, 0, '系统初始化默认账户', strftime('%Y-%m-%dT%H:%M:%fZ','now'));

INSERT OR IGNORE INTO portfolio
(uid, name, description, risk_level, base_position_ratio_ppm, is_active,
 draft_expire_seconds_etf_buy, draft_expire_seconds_etf_sell, draft_expire_seconds_base,
 draft_expire_seconds_grid, draft_expire_seconds_take_profit, draft_expire_otc_same_day,
 created_at_utc)
VALUES
('00000000-0000-4000-8000-000000000201', '默认组合', '系统初始化默认组合', 'DEFAULT', 200000, 1,
 300, 300, 900, 300, 300, 1,
 strftime('%Y-%m-%dT%H:%M:%fZ','now'));

INSERT OR IGNORE INTO instrument
(uid, code, name, instrument_type, market_code, currency, enabled, memo, created_at_utc)
VALUES
('00000000-0000-4000-8000-000000000301', 'CASH', '现金', 'CASH', 'CASH', 'CNY', 1, '系统现金标的', strftime('%Y-%m-%dT%H:%M:%fZ','now'));

INSERT OR IGNORE INTO app_setting
(uid, key, value, value_type, memo, created_at_utc)
VALUES
('00000000-0000-4000-8000-000000000401', 'app.schema.major', '1', 'INTEGER', '初始数据库主版本', strftime('%Y-%m-%dT%H:%M:%fZ','now')),
('00000000-0000-4000-8000-000000000402', 'market.refresh_interval_etf_seconds', '5', 'INTEGER', 'ETF 行情默认刷新间隔', strftime('%Y-%m-%dT%H:%M:%fZ','now')),
('00000000-0000-4000-8000-000000000403', 'market.refresh_interval_index_seconds', '10', 'INTEGER', '指数行情默认刷新间隔', strftime('%Y-%m-%dT%H:%M:%fZ','now')),
('00000000-0000-4000-8000-000000000404', 'draft.expire_seconds_grid', '300', 'INTEGER', '狙击买入 Draft 默认有效期', strftime('%Y-%m-%dT%H:%M:%fZ','now'));

PRAGMA user_version = 1;

COMMIT;
