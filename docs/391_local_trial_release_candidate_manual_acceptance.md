# EPIC-282 Local Trial Manual Acceptance Checklist

- [ ] Build passed.
- [ ] Full CTest passed.
- [ ] Local trial workspace created.
- [ ] Demo DB initialized at `.demo/local_trial_rc/data/etfdt_local_trial.sqlite`.
- [ ] DataService started with the local trial socket.
- [ ] Shell started.
- [ ] Dashboard visible.
- [ ] Dashboard page title is Chinese: `ShellAccounting 本地试用看板`.
- [ ] Module titles are Chinese: `Excel/VBA 导入`, `持仓/现金重算`,
      `行情数据`, `策略建议`, `交易草案`, `场外 A/C 多通道`.
- [ ] Excel/VBA import sample preview accepted.
- [ ] Import persist explicitly confirmed and row counts visible.
- [ ] Portfolio replay visible.
- [ ] Market data fixture refresh visible.
- [ ] Strategy BUY / SELL / HOLD / WAIT states visible.
- [ ] TradeDraft create requires explicit confirmation.
- [ ] TradeDraft warning states in Chinese:
      `这是内部草案，不是订单，不会提交券商。`
- [ ] OTCMap preview shows multi-leg result.
- [ ] OTCMap draft requires explicit confirmation.
- [ ] OTCMap draft created as internal draft, not order.
- [ ] Duplicate sample does not grow rows.
- [ ] Conflict sample does not grow rows.
- [ ] Provider disabled state visible.
- [ ] Market provider wording states in Chinese:
      `默认使用样例数据/禁用行情源，不会自动联网。`
- [ ] No live auto-refresh is active.
- [ ] No broker UI is shown.
- [ ] No order submit UI is shown.
- [ ] No automatic trading UI is shown.
- [ ] No production DB touched.
- [ ] Cleanup successful.

English technical IDs may still appear in status enums, JSON keys, file names,
or CTest output. The acceptance focus is whether local trial users can
understand the visible page labels, safety warnings, and manual workflow in
Chinese without changing the underlying contracts.
