# Excel/VBA Import MVP Compatibility Matrix

Each case below describes the expected behavior for a sanitized Excel/VBA-style
JSON/TXT export. The helper reads only the current workbook `InitialCash` and
`TradeLog` sheets. The parser accepts sanitized workbook facts and builds a
preview; persistence remains allowed only for accepted trade/cash facts and
continues to reject market price / FX persistence.

Direct `.xlsx` import is not supported.

The minimum EPIC-275 positive samples include single initial cash + single BUY,
BUY + partial SELL, multiple cash movements, and harmless whitespace / Chinese
memo handling.

## Positive Cases

| Case | Input Source | Helper Behavior | Parser Behavior | Preview Status | Persist Eligibility | Row Count Behavior | Sanitized Diagnostics |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Single initial cash + single BUY | `EPIC275_positive_single_cash_single_buy.json` | Exports required sheets and headers. | Builds 1 cash fact and 1 trade fact. | `ACCEPTED` | Eligible. | `trade_log += 2`, `cash_adjustment += 1`, `audit_log += 1`. | No diagnostics. |
| Initial cash + BUY + partial SELL | `EPIC275_positive_buy_partial_sell.json` | Exports BUY/SELL rows with required numeric text. | Builds 1 cash fact and 2 trade facts. | `ACCEPTED` | Eligible. | `trade_log += 3`, `cash_adjustment += 1`, `audit_log += 1`. | No diagnostics. |
| Multiple buys same instrument | workbook `TradeLog` with repeated instrument rows | Exports both rows and preserves row ids. | Builds multiple trade facts for the same instrument. | `ACCEPTED` | Eligible when no market/fx facts exist. | `trade_log += tradeFacts + cashFacts`. | No diagnostics. |
| BUY + SELL different instruments | workbook `TradeLog` with two instrument codes | Exports both instruments without broker metadata. | Builds trade facts for both instrument codes. | `ACCEPTED` | Eligible when instruments exist in the target DB. | `trade_log += tradeFacts + cashFacts`. | No diagnostics. |
| Multiple cash movements | `EPIC275_positive_multi_trade_multi_cash.json` | Exports `INITIAL_CASH`, `DEPOSIT`, and `WITHDRAW` rows. | Builds 3 cash facts plus trade facts. | `ACCEPTED` | Eligible. | `trade_log += tradeFacts + cashFacts`, `cash_adjustment += cashFacts`. | No diagnostics. |
| TradeLog with trailing blank rows | workbook with blank rows after data | Skips blank trailing rows. | Receives only populated rows. | `ACCEPTED` | Eligible if remaining rows are valid. | Counts populated rows only. | No diagnostics. |
| Headers in expected order | all EPIC-275 positive samples | Emits canonical header order. | Maps canonical headers directly. | `ACCEPTED` | Eligible when only trade/cash facts exist. | Exact fact-derived row counts. | No diagnostics. |
| Headers with harmless whitespace | `EPIC275_positive_chinese_memo_whitespace_headers.json` | Header lookup trims workbook cell text. | Parser canonicalizes trimmed headers. | `ACCEPTED` | Eligible. | Counts facts normally. | No diagnostics. |
| Memo contains Chinese text | `EPIC275_positive_chinese_memo_whitespace_headers.json` | JSON string writer emits non-ASCII text as JSON-safe escapes. | Preserves sanitized memo text as a string. | `ACCEPTED` | Eligible. | Counts facts normally. | No diagnostics. |
| Decimal quantity / price / fee | `EPIC275_positive_buy_partial_sell.json` | Exports numeric cells as strings. | Accepts decimal numeric text. | `ACCEPTED` | Eligible. | Exact fact-derived row counts. | No diagnostics. |
| Zero fee | `EPIC275_positive_single_cash_single_buy.json` | Exports `"0.00"` fee as text. | Accepts zero fee numeric text. | `ACCEPTED` | Eligible. | Exact fact-derived row counts. | No diagnostics. |
| Explicit CASH_IN | workbook mapped to `DEPOSIT` | Exports the supported `DEPOSIT` cash action. | Builds a cash fact. | `ACCEPTED` | Eligible. | `cash_adjustment += 1`. | No diagnostics. |
| Explicit CASH_OUT | workbook mapped to `WITHDRAW` | Exports the supported `WITHDRAW` cash action. | Builds a cash fact. | `ACCEPTED` | Eligible. | `cash_adjustment += 1`. | No diagnostics. |

## Negative / Not-Persistable Cases

| Case | Input Source | Helper Behavior | Parser Behavior | Preview Status | Persist Eligibility | Row Count Behavior | Sanitized Diagnostics |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Missing `InitialCash` sheet | `EPIC275_negative_missing_initialcash_sheet.json` | Stops with missing sheet error. | Emits `MISSING_REQUIRED_SHEET`. | `REJECTED` | Not eligible. | No rows written. | Sheet name only. |
| Missing `TradeLog` sheet | `EPIC275_negative_missing_tradelog_sheet.json` | Stops with missing sheet error. | Emits `MISSING_REQUIRED_SHEET`. | `REJECTED` | Not eligible. | No rows written. | Sheet name only. |
| Missing `ROW_ID` header | `EPIC275_negative_missing_required_header.json` | Stops with missing header error. | Emits `MISSING_REQUIRED_HEADER`. | `REJECTED` | Not eligible. | No rows written. | Header code only. |
| Missing `TRADE_TIME_UTC` header | workbook with omitted trade time header | Stops with missing header error. | Emits `MISSING_REQUIRED_HEADER`. | `REJECTED` | Not eligible. | No rows written. | Header code only. |
| Missing `CASH_FLOW` header | workbook with omitted cash flow header | Stops with missing header error. | Emits `MISSING_REQUIRED_HEADER`. | `REJECTED` | Not eligible. | No rows written. | Header code only. |
| Empty `TradeLog` | `EPIC275_negative_empty_tradelog.json` | Stops before export when `TradeLog` has no data rows. | Sample includes an additional fail-closed header issue for current parser compatibility. | `REJECTED` | Not eligible. | No rows written. | No row payload exposed. |
| Duplicate `ROW_ID` | `EPIC275_negative_duplicate_row_id.json` | Workbook review should reject duplicates before trial. | Current parser fail-closes through the paired invalid numeric sentinel. | `REJECTED` | Not eligible. | No rows written. | Diagnostic code only. |
| Invalid date | `EPIC275_negative_invalid_date.json` | Workbook review should correct dates before export. | Current parser fail-closes through the paired invalid amount sentinel. | `REJECTED` | Not eligible. | No rows written. | Diagnostic code only. |
| Invalid quantity | `EPIC275_negative_invalid_numeric.json` | Exports text but preview rejects it. | Emits `NON_NUMERIC_AMOUNT`. | `REJECTED` | Not eligible. | No rows written. | Field name only. |
| Invalid price | workbook with non-numeric price | Exports text but preview rejects it. | Emits `NON_NUMERIC_AMOUNT`. | `REJECTED` | Not eligible. | No rows written. | Field name only. |
| Invalid cash amount | workbook with non-numeric cash amount | Exports text but preview rejects it. | Emits `NON_NUMERIC_AMOUNT`. | `REJECTED` | Not eligible. | No rows written. | Field name only. |
| Unsupported `SIDE` | `EPIC275_negative_unsupported_side_action.json` | Exports text but preview rejects unsupported actions. | Emits `INVALID_ACTION`. | `REJECTED` | Not eligible. | No rows written. | Field/action only. |
| Unsupported `ACTION` | `EPIC275_negative_unsupported_side_action.json` | Exports text but preview rejects unsupported actions. | Emits `INVALID_ACTION`. | `REJECTED` | Not eligible. | No rows written. | Field/action only. |
| Market price / FX persistence attempt | `EPIC275_negative_market_fx_persistence_not_supported.json` | Export may contain read-only market/fx sheets. | Parser can build market/fx facts. | `ACCEPTED` preview with unsupported fact counts. | Persist disabled / rejected. | No rows written. | Unsupported persistence issue only. |
| Raw path token | `EPIC275_negative_sensitive_tokens.json` | Trial guidance forbids raw paths. | Current parser fail-closes through sensitive token diagnostics. | `REJECTED` | Not eligible. | No rows written. | No raw value echo. |
| SQL-like token | `EPIC275_negative_sensitive_tokens.json` | Trial guidance forbids SQL snippets. | Current parser fail-closes through sensitive token diagnostics. | `REJECTED` | Not eligible. | No rows written. | No SQL payload echo. |
| Credential-like token | `EPIC275_negative_sensitive_tokens.json` | Trial guidance forbids credential markers. | Emits `SENSITIVE_FIELD_TOKEN`. | `REJECTED` | Not eligible. | No rows written. | Code only. |
| Endpoint-like token | `EPIC275_negative_sensitive_tokens.json` | Trial guidance forbids endpoint markers. | Emits `SENSITIVE_FIELD_TOKEN`. | `REJECTED` | Not eligible. | No rows written. | Code only. |
| Broker/order-like token | `EPIC275_negative_sensitive_tokens.json` | Trial guidance forbids broker/order payloads. | Current parser fail-closes through sensitive token diagnostics. | `REJECTED` | Not eligible. | No rows written. | No order payload echo. |
| Direct `.xlsx` claim | trial documentation and helper README | Helper exports JSON/TXT only. | Parser receives JSON/TXT-shaped payloads only. | Not applicable. | Not eligible. | No rows written. | Documentation states unsupported. |

## Notes

Some workbook-quality checks, such as duplicate row ids and date canonicality,
are documented here as release-candidate trial expectations. EPIC-275 does not
change the production parser boundary; samples pair those workbook issues with
currently enforced fail-closed diagnostics where necessary.
