# EPIC-282 Local Trial Release Candidate Full Delivery

EPIC-282 packages the completed Dashboard MVP into a local trial release
candidate. The release candidate is a delivery wrapper around existing modules;
it is not a new trading capability.

## Included Modules

- Excel/VBA sanitized JSON/TXT import preview and accepted-preview persistence.
- Portfolio replay / position / cash / PnL read-only summary.
- Market data fixture refresh and historical-high summary.
- Strategy recommendation read-only BUY / SELL / HOLD / WAIT summary.
- Internal TradeDraft manual recommendation flow.
- OTCMap A/C multi-channel internal draft flow.
- Dashboard MVP page and local service E2E checks.
- Repo-local trial workspace, sample pack, scripts, runbook, checklist, and
  cleanup guidance.

## Excluded Modules

- Direct `.xlsx` import.
- Live market provider default networking.
- Broker integration, credentials, private endpoints, real order placement, and
  automatic trading.
- Installer, updater, system service installation, registry writes, and
  administrator-only setup.
- Production DB mutation outside explicit user-selected application flows.
- New migrations or changed `trade_log` / `cash_adjustment` semantics.

## Local Trial Boundary

The local trial uses `.demo/local_trial_rc` by default. The demo database is
`.demo/local_trial_rc/data/etfdt_local_trial.sqlite`. Users may pass another
explicit path, but the scripts never select a production database by default.

Market-data trial flows use fixture or disabled providers by default. TradeDraft
and OTCMap outputs are internal drafts, not orders.

## Release Blockers

Stop the release candidate if any validation needs a migration, broker,
credential, private endpoint, real order placement, automatic trading, live
provider default networking, direct QML access to DataServiceClient / SQLite /
network / engine APIs, changed accounting write semantics, weakened tests, or
directory-level allowlist broadening.

## Pass / Fail Criteria

Pass requires the RC CTest, Dashboard MVP CTest, all listed regression CTests,
full CTest, transport 50-run, `git diff --check`, and clean final status. The
manual checklist must be executable from the runbook using repo-local paths.

Fail any run that touches production DB paths, downloads dependencies, requires
administrator rights, leaves hidden services running, exposes raw private data,
or presents a draft as an executable order.

## Final Trial Path

Build the repo, create the local trial workspace, initialize the demo DB, start
the local DataService, start the Shell, open the Dashboard, run the sample
flows, verify duplicate/conflict behavior, preserve evidence, and clean up the
trial workspace.
