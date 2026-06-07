# TASK-268 ShellAccounting Excel/VBA Import Accepted Preview Persist QML Wiring

TASK-268 wires the existing accepted Excel/VBA import preview persistence contract
into `ShellAccountingReadOnlyPage.qml`. The QML page may show a Persist accepted
preview control only after a preview has returned `ACCEPTED`, a Presenter-owned
preview digest is available, the sanitized preview payload is available, and the
preview fact summary contains at least one fact.

The QML control must call only the ShellAccounting Presenter. It must not refer
to DataServiceClient, DataServiceApi, SQLite, AccountingEngine, or the raw
DataService action name. The Presenter owns request normalization and
idempotency key generation.

## Persist Control Boundary

- Persist is disabled until the Presenter is available.
- Persist is disabled unless `lastExcelVbaImportPreviewStatus == ACCEPTED`.
- Persist is disabled unless `lastExcelVbaImportPreviewDigest` is non-empty.
- Persist is disabled unless `excelVbaImportPreviewPayloadAvailable` is true.
- Persist is disabled unless trade/cash/market price/fx fact counts total at
  least one fact.
- Persist is disabled while `excelVbaImportPersistBusy` is true.
- Persist is disabled until the user checks the explicit confirmation control.
- Reset preview clears the confirmation state and Presenter persist state.

## Presenter Boundary

TASK-268 exposes a QML-safe Presenter wrapper for the already authorized
TASK-267 C++ contract. The wrapper persists only the last accepted preview,
uses the Presenter-owned digest and sanitized payload, generates a stable
idempotency key from the digest, and fails closed when the accepted preview
state is incomplete.

The wrapper does not expose raw SQL, raw filesystem paths, credentials,
endpoints, DataServiceClient, DataServiceApi, SQLite, or AccountingEngine to
QML.

## Result Visibility

The QML panel displays sanitized persistence state:

- persist status
- persist summary
- persist issue codes
- duplicate import prevented flag
- idempotency conflict rejected flag

No automatic persistence is added. Persistence is triggered only by explicit
user click after confirmation.

## Forbidden Scope

TASK-268 does not modify startup wiring, DataServiceApi action implementation,
DataServiceClient, DataAccess, migrations, AccountingEngine, broker/network
credentials/endpoints, production file loader behavior, historical fixtures,
TradeDraft generation, strategy execution, real order placement, or automatic
trading.
