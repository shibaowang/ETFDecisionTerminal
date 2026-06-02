#include "DataAccess/ShellAccountingManualEntryRepositoryScaffold.h"

namespace etfdt::data_access {
namespace {

ShellAccountingManualEntryPersistenceResult disabledResult()
{
    ShellAccountingManualEntryPersistenceResult result;
    result.issues.push_back("MANUAL_ENTRY_REPOSITORY_SCAFFOLD_WRITE_NOT_IMPLEMENTED");
    return result;
}

}  // namespace

ShellAccountingManualEntryPersistenceResult
ShellAccountingManualEntryRepositoryScaffold::persistManualTransaction(
    const ShellAccountingManualTransactionPersistenceCommand&)
    const
{
    return disabledResult();
}

ShellAccountingManualEntryPersistenceResult
ShellAccountingManualEntryRepositoryScaffold::persistManualCashMovement(
    const ShellAccountingManualCashMovementPersistenceCommand&)
    const
{
    return disabledResult();
}

}  // namespace etfdt::data_access
