# 系统架构设计

## 当前状态

v0.1 草案。

## 后续补充方向

补充进程边界、启动依赖、故障恢复策略和本地服务通信细节。

## 技术路线

- 操作系统：Windows
- 技术底座：Qt / C++
- 界面路线：Qt Quick / QML + C++
- 架构方式：本地受控多进程
- 进程通信：Qt Local Socket + JSON
- 数据库：SQLite + WAL
- 构建系统：CMake
- 项目组织：Monorepo

## 进程划分

### ETFDecisionShell.exe

QML 主界面。只负责展示和交互，不写策略逻辑，不直接写数据库。

### ETFWatchdog.exe

负责启动服务、心跳检测、异常重启、崩溃日志。

### ETFDataService.exe

SQLite 唯一写入口。负责账务写入、数据迁移、备份恢复、Repository / DAO 层、数据库事务。

### ETFMarketService.exe

负责行情采集、多源降级、行情缓存、数据状态标记。

### ETFStrategyService.exe

负责底仓保护、狙击资金池、六档策略、TradeDraft、资金分配计划、场内/场外替代判断。

### ETFAlertService.exe

负责预警去重、桌面通知、PushPlus 推送。

## 服务启动顺序

1. ETFDataService
2. ETFMarketService
3. ETFStrategyService
4. ETFAlertService
5. ETFDecisionShell 连接并展示状态

## 核心架构原则

- QML 只做界面。
- C++ 服务层负责业务。
- DataService 是唯一数据库写入口。
- TradeLog 是唯一事实账本。
- position_snapshot、cash_snapshot、portfolio_summary 都是派生缓存。
- 服务异常恢复后，从 SQLite 重建业务状态。
