#include "ThreadManager.h"
#include <opencv2/core/utility.hpp>
#include <signal.h>
#include <thread>

/// singleton instance
pnx::ThreadManager thread_manager;

/**
 * @brief signal handler to handle exit signal
 * @param signum
 */
void signalHandler(int signum) {
  pthread_cancel(thread_manager.GenerateId);
  pthread_cancel(thread_manager.ProcessId);
  pthread_cancel(thread_manager.ReceiveId);
  pthread_cancel(thread_manager.SendId);
  thread_manager.Exit();

  exit(signum);
}

int main(int argc, char *argv[]) {
  // 注册信号处理函数,用于处理退出信号,保证线程释放\关闭相机
  signal(SIGINT, signalHandler);
  signal(SIGHUP, signalHandler);

  // 初始化
  thread_manager.InitAll();

  // 生成线程
  std::thread generate_thread(&pnx::ThreadManager::GenerateThread,
                              std::ref(thread_manager));
  thread_manager.GenerateId = generate_thread.native_handle();

  // 处理线程
  std::thread process_thread(&pnx::ThreadManager::ProcessThread,
                             std::ref(thread_manager));
  thread_manager.ProcessId = process_thread.native_handle();

  // 接收线程
  // std::thread receive_thread(&pnx::ThreadManager::ReceiveThread,
  //                            std::ref(thread_manager));
  // thread_manager.ReceiveId = receive_thread.native_handle();

  // // 发送线程
  // std::thread send_thread(&pnx::ThreadManager::SendThread,
  //                         std::ref(thread_manager));
  // thread_manager.SendId = send_thread.native_handle();

  // 等待主线程
  generate_thread.join();
  process_thread.join();
  // receive_thread.join();
  // send_thread.join();

  return 0;
}
