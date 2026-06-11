# Excel/VBA Import MVP Export Helper Mapping

This document maps workbook sheets and columns to the MVP JSON/TXT import
contract. The helper reads these columns from the current workbook and emits
string cells in the same order.

## Root Contract

```json
{
  "schemaVersion": "excel-vba-export/v1",
  "source": "sanitized-excel-vba-export",
  "sheets": [
    {
      "name": "InitialCash",
      "sheetName": "InitialCash",
      "headers": [],
      "rows": []
    }
  ]
}
```

Direct `.xlsx` import is not supported. The supported path is Excel/VBA-side
export to sanitized JSON/TXT, then MVP preview/import.

## InitialCash Sheet

Required columns:

- `ROW_ID`
- `TIME_UTC`
- `ACCOUNT_CODE`
- `PORTFOLIO_CODE`
- `ACTION`
- `AMOUNT`
- `CURRENCY`
- `MEMO`

Supported MVP values:

- `ACTION`: `INITIAL_CASH`, `CASH_IN`, or `CASH_OUT`
- `ACCOUNT_CODE`: sanitized account code such as `DEMO_ACCOUNT`
- `PORTFOLIO_CODE`: sanitized portfolio code such as `DEMO_PORTFOLIO`

## TradeLog Sheet

Required columns:

- `ROW_ID`
- `TRADE_TIME_UTC`
- `ACCOUNT_CODE`
- `PORTFOLIO_CODE`
- `INSTRUMENT_CODE`
- `SIDE`
- `QUANTITY`
- `PRICE`
- `AMOUNT`
- `FEE`
- `CASH_FLOW`
- `CURRENCY`
- `SOURCE`
- `MEMO`

Supported MVP values:

- `SIDE`: `BUY` or `SELL`
- `SOURCE`: sanitized label such as `SANITIZED_SAMPLE`
- `INSTRUMENT_CODE`: sanitized instrument code such as `DEMO_ETF`

## Privacy And Sanitization

The export must not include raw user account names, production file paths,
SQL text, credentials, endpoints, stack traces, broker payloads, real order
identifiers, automatic trading controls, or secrets. Use sanitized labels
before exporting.
