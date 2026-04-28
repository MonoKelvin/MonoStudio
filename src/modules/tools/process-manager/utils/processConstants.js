/**
 * 进程管理常量
 */

export const SYSTEM_PROCESSES = [
  'system',
  'kernel',
  'svchost',
  'csrss',
  'lsass',
  'wininit',
  'services'
]

export const COLUMN_CONFIG = [
  { key: 'name', title: '进程名称', visible: true, width: '250px' },
  { key: 'pid', title: 'PID', visible: true, width: '80px' },
  { key: 'memory', title: '内存', visible: true, width: '120px' },
  { key: 'cpu', title: 'CPU', visible: true, width: '100px' },
  { key: 'status', title: '状态', visible: true, width: '100px' },
  { key: 'actions', title: '操作', visible: true, width: '150px' }
]

export const STATUS_MAP = {
  running: '运行中',
  sleeping: '睡眠',
  stopped: '已停止',
  unknown: '未知'
}

export const PRIORITY_MAP = {
  high: '高',
  above_normal: '高于正常',
  normal: '正常',
  below_normal: '低于正常',
  idle: '空闲'
}

export const PAGE_SIZE = 50
export const AUTO_REFRESH_INTERVAL = 3000
export const MEMORY_HIGH_THRESHOLD = 500 * 1024 * 1024 // 500MB
export const CPU_HIGH_THRESHOLD = 80
