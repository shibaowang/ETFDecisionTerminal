# position.list Real Implementation Readiness Checklist

## Document Purpose

This document is used to decide whether real `position.list` implementation can
begin.

It does not implement code and does not change the current guard.

## Go / No-Go Checklist

- [ ] `position.list` guard has been merged to `main`.
- [ ] No-write harness has been merged to `main`.
- [ ] DataAccess trade facts reader has been merged to `main`.
- [ ] `TradeFactRow` -> `TradeFactDto` mapping has been designed.
- [ ] Cash facts source gap has been handled or a degrade strategy has been
  chosen.
- [ ] Market price source gap has been handled or `UNAVAILABLE` strategy has
  been chosen.
- [ ] FX rate source gap has been handled or unsupported strategy has been
  chosen.
- [ ] AccountingEngine integration boundary has been confirmed.
- [ ] DataService action failure behavior has been confirmed.
- [ ] No-write table count tests have been designed.
- [ ] QML is not connected.
- [ ] User explicitly authorized real action implementation.

## Recommended First-stage Scope

Recommended first stage:

- read-only
- DTO-only mapping
- trade facts query input
- empty ledger
- single BUY
- missing market price warning
- multi-currency unsupported error
- no snapshot writes
- no TradeDraft
- no QML

The first stage should not support every complex scenario. It should not
connect market price or FX rate services. It should not connect QML.

## No-Go Conditions

Do not enter implementation if:

- cash facts source is unclear and there is no degrade strategy
- no-write tests are not ready
- DataService to AccountingEngine mapping is not determined
- the task intends to write snapshots
- the task intends to connect QML directly
- the task intends to generate trade suggestions
- the task intends to write TradeDraft

## Next Candidate Tasks

Possible next tasks:

- TASK-095: PositionList DataService-to-AccountingEngine mapping contract tests
- TASK-095: `position.list` real action no-write test skeleton
- TASK-095: cash facts source schema review
- TASK-095: ShellServices accounting controller contract docs

Any real implementation requires separate explicit authorization.
