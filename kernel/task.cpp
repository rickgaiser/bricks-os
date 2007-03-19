#include "kernel/task.h"
#include "asm/irq.h"
#include "iostream"


extern pt_regs * current_thread;
uint32_t  CTask::iTaskCount_(0);
CTask   * CTask::pCurrentTask_ = 0;
CTask   * CTask::taskTable_[MAX_TASK_COUNT];
IFileIO * CTask::pSTDIN_ = 0;
IFileIO * CTask::pSTDOUT_ = 0;

extern "C" void schedule();


// -----------------------------------------------------------------------------
extern "C" void
kill()
{
  CTask * pTemp = CTask::pCurrentTask_;

//  std::cout<<"suicide!"<<std::endl;

  if(pTemp != pTemp->next)
  {
    // Remove current task from list
    pTemp->prev->next = pTemp->next;
    pTemp->next->prev = pTemp->prev;

    // Set new current task
    CTask::pCurrentTask_ = pTemp->prev;

    // Delete task
    delete pTemp;

    // FIXME: How does this function return, there is nothing to return to.
    while(true){}

    // Reschedule so the next task will run
    schedule();
  }
  else
  {
    std::cout<<"ERROR: Can not kill last task"<<std::endl;
    while(true){}
  }
}

// -----------------------------------------------------------------------------
CTask::CTask(void * entry, size_t stack, size_t svcstack, int argc, char * argv[])
 : eState_(TS_UNINITIALIZED)
 , pStack_(0)
 , pSvcStack_(0)
{
  pFiles_[0] = pSTDIN_;
  pFiles_[1] = pSTDOUT_;
  pFiles_[2] = pSTDOUT_;

  for(int i(3); i < MAX_FILE_COUNT; i++)
    pFiles_[i] = 0;

  for(int i(0); i < MAX_CHANNEL_COUNT; i++)
    pChannel_[i] = 0;

  for(int i(0); i < MAX_CONNECTION_COUNT; i++)
    pConnection_[i] = 0;

  pTaskState_ = new pt_regs;
  if(stack != 0)
    pStack_ = new uint32_t[stack];
  if(svcstack != 0)
    pSvcStack_ = new uint32_t[svcstack];

  taskInit(pTaskState_, entry, pStack_ + stack, pSvcStack_ + svcstack, argc, reinterpret_cast<uint32_t>(argv));

  prev = this;
  next = this;
}

// -----------------------------------------------------------------------------
CTask::~CTask()
{
  if(pTaskState_ != 0)
    delete pTaskState_;
  if(pStack_ != 0)
    delete pStack_;
  if(pSvcStack_ != 0)
    delete pSvcStack_;
}

// -----------------------------------------------------------------------------
void
CTask::addTask(CTask * pTask)
{
  if(iTaskCount_ == 0)
  {
    for(int iTask(0); iTask < MAX_TASK_COUNT; iTask++)
      taskTable_[iTask] = 0;
    current_thread = pTask->pTaskState_;
    pCurrentTask_ = pTask;
    taskTable_[0] = pTask;
    iTaskCount_++;
  }
  else if(iTaskCount_ < MAX_TASK_COUNT)
  {
    pTask->next = pCurrentTask_->next;
    pTask->prev = pCurrentTask_;

    pCurrentTask_->next->prev = pTask;
    pCurrentTask_->next = pTask;

    for(int iTask(0); iTask < MAX_TASK_COUNT; iTask++)
    {
      if(taskTable_[iTask] == 0)
      {
        taskTable_[iTask] = pTask;
        break;
      }
    }

    iTaskCount_++;
  }
}

// -----------------------------------------------------------------------------
void
CTask::setStandardInput(IFileIO * pSTDIN)
{
  pSTDIN_ = pSTDIN;
}

// -----------------------------------------------------------------------------
void
CTask::setStandardOutput(IFileIO * pSTDOUT)
{
  pSTDOUT_ = pSTDOUT;
}
