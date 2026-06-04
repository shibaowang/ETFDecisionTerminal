# ShellAccounting Manual Entry Replay Negative Fixture Static Validator Authorization Gate

## Purpose

TASK-223 authorizes future negative fixture static validator policy only. It
defines the boundary for a future test-only static validator that may inspect
the TASK-222 negative fixture scaffold files and return sanitized issue objects.

TASK-223 does not implement negative validator. TASK-223 does not implement
parser, loader, reader. TASK-223 does not implement replay. TASK-223 does not
call AccountingEngine replay.

TASK-223 does not modify negative fixture JSON. TASK-223 does not modify
positive fixture JSON. TASK-223 does not modify TASK-219 static validator.

## Relationship To TASK-220 Through TASK-222

TASK-220 authorized the negative fixture policy boundary. TASK-221 authorized
the future negative fixture scaffold boundary without creating files. TASK-222
created the test-only negative fixture scaffold directory, index, and ten
`NEG_MRF*.json` files.

TASK-223 is the next authorization gate after those tasks. It authorizes only
the policy for a future static validator and keeps every runtime, replay,
fixture mutation, production, and broker boundary closed.

## Future Static Validator Scope

Future validator implementation, if separately authorized, may exist only as a
test-only static validator under:

```text
tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/
```

That future validator must remain a static source/fixture inspection tool. It
must not become a parser, loader, reader, replay harness, runtime data source,
database reader, service action, or production dependency.

## Future Input Boundary

Future validator input must be limited to these test-only files:

```text
tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json
tests/fixtures/manual_entry_replay_negative/NEG_MRF*.json
```

Future validator implementation must not read runtime DB files, production
paths, broker payloads, service-layer outputs, DataService responses, QML
payloads, credentials, endpoint configuration, command-line state, environment
secrets, or positive fixture JSON as mutable input.

## Future Output Contract

Future validator output must be a sanitized issue object. Each issue object
must include at least:

```text
negativeFixtureId
file
issueCode
severity
blocking
rule
message
```

The validator must not echo raw fixture payload. The validator must not output
credentials. The validator must not output endpoint. The validator must not
output broker payload. The validator must not output real order id. The
validator must not output raw SQL. The validator must not output exception
stack.

## Future Expected Issue Code Mapping

Future validator implementation must recognize the following sanitized issue
codes:

```text
NEG_FIXTURE_MISSING_REQUIRED_FIELD
NEG_FIXTURE_SCHEMA_VERSION_INVALID
NEG_FIXTURE_ID_MISMATCH
NEG_FIXTURE_DUPLICATE_ID
NEG_FIXTURE_UNAUTHORIZED_FILE
NEG_FIXTURE_UNAUTHORIZED_PATH
NEG_FIXTURE_RUNTIME_USE_TRUE
NEG_FIXTURE_PRODUCTION_USE_TRUE
NEG_FIXTURE_REPLAY_EXECUTED_TRUE
NEG_FIXTURE_SYNTHETIC_FLAG_FALSE
NEG_FIXTURE_FORBIDDEN_TOKEN
NEG_FIXTURE_RAW_SQL_TOKEN
NEG_FIXTURE_CREDENTIAL_TOKEN
NEG_FIXTURE_ENDPOINT_TOKEN
NEG_FIXTURE_BROKER_PAYLOAD_TOKEN
NEG_FIXTURE_REAL_ORDER_TOKEN
NEG_FIXTURE_DB_ARTIFACT_PRESENT
NEG_FIXTURE_PRODUCTION_PATH_REFERENCE
```

Unknown issue code must fail. Duplicate fixture id must fail. Unauthorized JSON
must fail. Unsafe scaffold flag must fail.

## Future Failure Semantics

Future validator implementation must fail closed. It must fail on missing
required fields, wrong schema version, id/file mismatch, duplicate fixture id,
unauthorized file, unauthorized path, runtime-use flags, production-use flags,
replay-executed flags, non-synthetic flags, forbidden tokens, raw SQL tokens,
credential tokens, endpoint tokens, broker payload tokens, real order tokens,
database artifacts, and production path references.

The future validator must report sanitized issue codes and messages only. It
must not use silent success for unknown, partial, malformed, unsafe, or
unauthorized negative fixture data.

## Privacy And Sanitization Boundary

Future validator implementation must preserve the TASK-222 synthetic-only
privacy boundary. It must not expose real account identifiers, raw UI payload,
full manual entry payload, full trade fact payload, full cash adjustment
payload, raw SQL, credentials, endpoint values, access tokens, secret keys,
private keys, passwords, exception stacks, broker payloads, real order ids, or
production database paths.

Allowed diagnostic content is limited to sanitized fixture id, file name, issue
code, severity, blocking flag, validator rule, and short sanitized message.

## No-Replay Boundary

TASK-223 does not implement replay. TASK-223 does not call AccountingEngine
replay. Future negative fixture static validator must not execute replay,
compute positions, compute cash, compute portfolio values, compute PnL, perform
readback, refresh read models, or write snapshots.

Negative fixture scaffold files are not replay input. They are not runtime
fixtures. They are not production data.

## No-Runtime-Dependency Boundary

TASK-223 does not modify production code. TASK-223 does not modify QML /
DataServiceActions / repositories / migrations. TASK-223 does not write runtime
SQL / SQLite / audit / ledger / snapshot. TASK-223 does not connect broker /
network / credentials / endpoint. TASK-223 does not place real orders or enable
automatic trading.

Future validator implementation must not depend on services, sockets,
DataService actions, runtime databases, broker SDK, network calls, credentials,
endpoints, production startup, or QML.

## Out-of-Scope Boundaries

Out of scope for TASK-223:

- negative validator implementation
- parser / loader / reader implementation
- replay implementation
- AccountingEngine replay calls
- runtime SQL / SQLite read/write
- audit / ledger / snapshot writes
- changes to negative fixture JSON
- changes to positive fixture JSON
- changes to TASK-219 static validator
- production code changes
- QML / DataServiceActions / repository / migration changes
- broker, network, credentials, endpoint, real order placement, or automatic
  trading

## Formal Conclusion And Next Task

TASK-223 authorizes future negative fixture static validator policy only.
TASK-223 does not authorize negative validator implementation.
TASK-223 does not authorize parser, loader, or reader implementation.
TASK-223 does not authorize replay implementation.
TASK-223 does not authorize AccountingEngine replay calls.
TASK-223 does not authorize runtime SQL / SQLite read/write.
TASK-223 does not authorize audit / ledger / snapshot writes.
TASK-223 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.
Recommended next task: TASK-224 manual entry replay negative fixture static validator implementation gate.
