#include "kernel/task.h"
#include "asm/irq.h"
#include "iostream"


uint32_t  CTask::iTaskCount_(0);
CTask   * CTask::pCurrentTask_ = 0;
IFileIO * CTask::pSTDIN_ = 0;
IFileIO * CTask::pSTDOUT_ = 0;


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

  pTaskState_ = new pt_regs;
  if(stack != 0)
    pStack_ = new uint32_t[stack];
  if(svcstack != 0)
    pSvcStack_ = new uint32_t[svcstack];

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
    pCurrentTask_ = pTask;
    iTaskCount_++;
  }
  else if(iTaskCount_ < MAX_TASK_COUNT)
  {
    pTask->next = pCurrentTask_->next;
    pTask->prev = pCurrentTask_;

    pCurrentTask_->next->prev = pTask;
    pCurrentTask_->next = pTask;

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
