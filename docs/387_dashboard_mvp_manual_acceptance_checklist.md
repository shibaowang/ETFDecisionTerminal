# Dashboard MVP Manual Acceptance Checklist

1. Build the application and tests with the documented CMake command.
2. Start the local DataService using a temp or demo database path.
3. Start the Shell application.
4. Open the ShellAccounting Dashboard page.
5. Load the sanitized Excel/VBA sample JSON/TXT export.
6. Run Preview and confirm the ACCEPTED state.
7. Check the explicit persist confirmation box.
8. Persist the accepted preview.
9. Verify persisted trade and cash row counts are visible.
10. Verify post-write readback refresh status is visible.
11. Run portfolio replay and verify cash, position, and PnL summary fields.
12. Run market fixture refresh and verify provider status, price, high,
    drawdown, premium/discount, and issue fields.
13. Run strategy recommendation and verify action, reason, quantity, and amount.
14. Preview TradeDraft from recommendation, check explicit confirmation, and
    create an internal draft.
15. Preview OTCMap multi-leg split, check explicit confirmation, and create an
    internal multi-leg draft.
16. Verify duplicate and conflict states do not imply new executed rows.
17. Verify there is no broker, order-submission, or automatic-trading UI.
18. Verify provider disabled or stale states are visible when using the disabled
    or stale samples.
19. Reset Dashboard state.
20. Stop the local service and clean up the temp/demo workspace.
