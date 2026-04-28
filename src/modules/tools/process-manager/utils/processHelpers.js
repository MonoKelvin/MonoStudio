/**
 * 进程管理工具函数
 */

import { SYSTEM_PROCESSES, STATUS_MAP, PRIORITY_MAP, MEMORY_HIGH_THRESHOLD, CPU_HIGH_THRESHOLD } from './processConstants'

/**
 * 格式化内存大小
 */
export function formatMemory(bytes) {
  if (!bytes) return '0 B'
  if (bytes < 1024) return bytes + ' B'
  if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(1) + ' KB'
  if (bytes < 1024 * 1024 * 1024) return (bytes / (1024 * 1024)).toFixed(1) + ' MB'
  return (bytes / (1024 * 1024 * 1024)).toFixed(1) + ' GB'
}

/**
 * 获取内存使用百分比
 */
export function getMemoryPercentage(process) {
  if (!process || !process.memory) return 0
  const maxMemory = 1024 * 1024 * 1024 // 1GB as reference
  return Math.min((process.memory / maxMemory) * 100, 100)
}

/**
 * 判断是否为系统进程
 */
export function isSystemProcess(process) {
  if (!process) return true
  return SYSTEM_PROCESSES.some(sp =>
    process.name.toLowerCase().includes(sp) ||
    (process.path && process.path.toLowerCase().includes('windows system32'))
  )
}

/**
 * 获取状态类名
 */
export function getStatusClass(process) {
  if (process.status === 'running') return 'status-running'
  if (process.status === 'sleeping') return 'status-sleeping'
  if (process.status === 'stopped') return 'status-stopped'
  return 'status-unknown'
}

/**
 * 获取状态文本
 */
export function getStatusText(process) {
  return STATUS_MAP[process?.status] || '未知'
}

/**
 * 截断路径
 */
export function truncatePath(path) {
  if (!path || path.length <= 50) return path
  return '...' + path.slice(-47)
}

/**
 * 判断是否可以暂停进程
 */
export function canSuspendProcess(process) {
  return process && process.status === 'running' && !isSystemProcess(process)
}

/**
 * 添加进程的暂停状态
 */
export function addSuspendState(processes) {
  return processes.map(p => ({
    ...p,
    isSuspended: p.isSuspended || false
  }))
}
