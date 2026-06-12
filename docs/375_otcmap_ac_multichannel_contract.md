# EPIC-279 OTCMap A/C Multi-Channel Contract

The OTCMap payload is a sanitized JSON object, or an object with an `input`
object. It is accepted only as in-memory text. File paths, credentials,
endpoints, broker/order tokens, and automatic trading tokens fail closed.

## Input Fields

Required common fields:

- `accountId`
- `portfolioId`
- `strategyId`
- `strategyCode`
- `side`: `BUY` or `SELL`
- `idempotencyKey`
- `otcMapDigest`
- `channels[]`

BUY fields:

- `targetAmountText`
- `cashBalanceText`

SELL fields:

- `requestedSellQuantityText`
- `principalBaseText`
- `holdingCostText`
- `basePositionRatioText`

Each channel carries `instrumentId`, `instrumentCode`, `fundClass`,
`enabled`, `supportsBuy`, `supportsSell`, `priority`, `dailyBuyLimitText`,
`dailySellLimitText`, `minBuyAmountText`, `minSellAmountText`, `navText`,
`holdingQuantityText`, `holdingCostText`, and `currency`.

## Output Fields

Preview and create responses expose:

- `status`
- `accepted`
- `eligibleForTradeDraft`
- `legCount`
- `totalAmountText`
- `totalQuantityText`
- `totalNetCashImpactText`
- `baseProtectionPassed`
- `cashLimitApplied`
- `issueCodes[]`
- `legs[]`

Each leg exposes `instrumentCode`, `fundClass`, `side`, `quantityText`,
`amountText`, `navText`, `netCashImpactText`, `currency`, and priority.

## Persistence Contract

Draft creation requires explicit user confirmation, idempotency key, and digest.
The same key and digest returns duplicate without row growth. The same key and
different digest returns conflict without row growth.

The repository writes one `trade_draft`, multiple `trade_draft_leg` rows, and
one sanitized `audit_log` row in one transaction. It does not write
`trade_log`, `cash_adjustment`, persistent read models, broker state, endpoint
state, or order records.

