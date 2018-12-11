#include "TaskParseWrapper.h"

#include "DialogView.h"
#include "PersistentStorage.h"
#include "Blackboard.h"
#include "utility.h"

TaskParseWrapper::TaskParseWrapper(std::weak_ptr<PersistentStorage> storage, std::shared_ptr<DialogView> dialogView)
	: m_storage(storage)
	, m_dialogView(dialogView)
{
}

void TaskParseWrapper::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	int sourceFileCount = 0;
	blackboard->get("source_file_count", sourceFileCount);

	m_dialogView->clearDialogs();
	m_dialogView->updateIndexingDialog(0, 0, sourceFileCount, { });

	m_start = utility::durationStart();

	if (sourceFileCount > 0)
	{
		if (std::shared_ptr<PersistentStorage> storage = m_storage.lock())
		{
			storage->setMode(SqliteIndexStorage::STORAGE_MODE_WRITE);
		}
	}
}

Task::TaskState TaskParseWrapper::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	return m_taskRunner->update(blackboard);
}

void TaskParseWrapper::doExit(std::shared_ptr<Blackboard> blackboard)
{
	float duration = utility::duration(m_start);
	blackboard->update<float>("index_time", [duration](float currentDuration) { return currentDuration + duration; });
}

void TaskParseWrapper::doReset(std::shared_ptr<Blackboard> blackboard)
{
	m_taskRunner->reset();
}
