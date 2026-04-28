/**
 * 进程服务 - 使用 systeminformation 获取真实系统进程数据
 */

// 延迟导入 systeminformation，只在非浏览器环境使用
let si = null
const isBrowser = typeof window !== 'undefined' && !window.require
if (!isBrowser) {
  try {
    si = require('systeminformation')
  } catch (e) {
    console.warn('systeminformation not available:', e)
  }
}

class ProcessService {
  constructor() {
    this.cache = {
      processes: null,
      timestamp: 0,
      cacheDuration: 2000 // 2秒缓存
    }
    this.refreshInterval = null
    this.isRefreshing = false
    this.isElectron = window.require ? true : false

    // 如果在 Electron 环境，加载 child_process
    this.cp = null
    this.psList = null

    if (this.isElectron) {
      try {
        const { exec } = window.require('child_process')
        this.cp = exec
      } catch (error) {
        console.warn('无法加载 child_process:', error)
      }
    }
  }

  /**
   * 获取进程列表
   */
  async getProcesses() {
    // 检查缓存
    if (this.cache.processes && Date.now() - this.cache.timestamp < this.cache.cacheDuration) {
      return this.cache.processes
    }

    // 根据环境选择获取方式
    if (this.isElectron && this.cp) {
      // Electron 环境，使用 tasklist 命令获取更详细的信息
      return await this.fetchProcessesFromWindows()
    } else {
      // 浏览器环境或其他，使用 systeminformation
      return await this.fetchProcessesFromSystemInfo()
    }
  }

  /**
   * 使用 Windows tasklist 命令获取进程信息（Electron 环境）
   */
  async fetchProcessesFromWindows() {
    this.isRefreshing = true

    try {
      // 使用 tasklist 命令获取进程列表
      const { stdout } = await new Promise((resolve, reject) => {
        this.cp('tasklist /v /fo csv', (error, stdout, stderr) => {
          if (error) {
            reject(error)
          } else {
            resolve({ stdout })
          }
        })
      })

      // 解析 tasklist 输出
      const processes = this.parseTasklistOutput(stdout)

      // 更新缓存
      this.cache.processes = processes
      this.cache.timestamp = Date.now()
      this.isRefreshing = false

      return processes
    } catch (error) {
      console.error('获取 Windows 进程列表失败:', error)
      // 降级到 systeminformation
      return await this.fetchProcessesFromSystemInfo()
    }
  }

  /**
   * 解析 tasklist 的 CSV 输出
   */
  parseTasklistOutput(output) {
    const lines = output.split('\n')
    const processes = []

    // 跳过前两行（标题和分隔符）
    for (let i = 2; i < lines.length; i++) {
      const line = lines[i].trim()
      if (!line) continue

      // CSV 格式: "Image Name","PID","Session Name","Session#","Mem Usage"
      const parts = this.parseCsvLine(line)

      if (parts.length >= 5) {
        const name = parts[0].replace(/"/g, '')
        const pid = parseInt(parts[1].replace(/"/g, ''))
        const memUsage = this.parseMemory(parts[4].replace(/"/g, ''))

        processes.push({
          pid: pid,
          name: name,
          path: '', // tasklist 不提供路径
          cpu: 0, // tasklist /v 不提供 CPU 使用率
          memory: memUsage,
          status: 'running',
          priority: 'normal',
          username: parts[2].replace(/"/g, ''),
          session_id: parts[3].replace(/"/g, ''),
          parent_pid: 0, // 需要额外获取
          threads: 0, // 需要额外获取
          handles: 0, // tasklist /v 提供但没有在标准输出中
          command_line: '', // 需要额外获取
          working_directory: '',
          network: []
        })
      }
    }

    return processes
  }

  /**
   * 解析 CSV 行
   */
  parseCsvLine(line) {
    const result = []
    let current = ''
    let inQuotes = false

    for (let i = 0; i < line.length; i++) {
      const char = line[i]

      if (char === '"') {
        inQuotes = !inQuotes
      } else if (char === ',' && !inQuotes) {
        result.push(current.trim())
        current = ''
      } else {
        current += char
      }
    }

    result.push(current.trim())
    return result
  }

  /**
   * 解析内存字符串（如 "4,123 K" -> 字节数）
   */
  parseMemory(memStr) {
    // 移除逗号和空格
    const clean = memStr.replace(/,/g, '').trim()

    // 提取数值和单位
    const match = clean.match(/^(\d+)\s*([KMG]?)$/i)
    if (!match) return 0

    const value = parseInt(match[1])
    const unit = match[2].toUpperCase()

    // 转换为字节数
    switch (unit) {
      case 'K': return value * 1024
      case 'M': return value * 1024 * 1024
      case 'G': return value * 1024 * 1024 * 1024
      default: return value
    }
  }

  /**
   * 使用 systeminformation 获取进程信息
   */
  async fetchProcessesFromSystemInfo() {
    // 在浏览器环境中，返回空列表
    if (!si || isBrowser) {
      return []
    }

    this.isRefreshing = true

    try {
      // 获取进程列表
      const processes = await si.processes()

      // 转换进程数据格式
      const formattedProcesses = processes.list.map(proc => ({
        pid: proc.pid,
        name: proc.name,
        path: proc.path || '',
        cpu: proc.pcpu || 0,
        memory: (proc.mem * 1024 * 1024) || 0, // 转换为字节
        status: this.mapProcessStatus(proc.state),
        priority: proc.priority || 'normal',
        username: proc.user || 'SYSTEM',
        session_id: proc.session || '0',
        parent_pid: proc.parent || 0,
        threads: proc.threads || 0,
        handles: proc.handles || 0, // 文件描述符数量
        command_line: proc.command || '',
        working_directory: proc.cwd || '',
        network: [] // systeminformation 不直接提供网络连接
      }))

      // 更新缓存
      this.cache.processes = formattedProcesses
      this.cache.timestamp = Date.now()
      this.isRefreshing = false

      return formattedProcesses
    } catch (error) {
      console.error('获取进程列表失败:', error)
      this.isRefreshing = false
      return []
    }
  }

  /**
   * 映射进程状态
   */
  mapProcessStatus(state) {
    const statusMap = {
      'running': 'running',
      'sleeping': 'sleeping',
      'stopped': 'stopped',
      'zombie': 'stopped',
      'dead': 'stopped',
      'idle': 'sleeping'
    }
    return statusMap[state] || 'unknown'
  }

  /**
   * 获取进程详情
   * @param {number} pid 进程ID
   */
  async getProcessDetail(pid) {
    const processes = await this.getProcesses()
    const process = processes.find(p => p.pid === pid)

    if (!process) return null

    // 如果在 Electron 环境，尝试获取更多信息
    if (this.isElectron && this.cp) {
      try {
        // 获取命令行参数
        const { stdout } = await new Promise((resolve, reject) => {
          this.cp(`wmic process where "ProcessId=${pid}" get CommandLine /value`, (error, stdout, stderr) => {
            if (error) {
              reject(error)
            } else {
              resolve({ stdout })
            }
          })
        })

        // 解析命令行
        const commandLine = stdout.split('\n')[1]?.split('=')[1]?.trim() || ''
        process.command_line = commandLine

        // 获取工作目录
        const { stdout: cwdOutput } = await new Promise((resolve, reject) => {
          this.cp(`wmic process where "ProcessId=${pid}" get ExecutablePath /value`, (error, stdout, stderr) => {
            if (error) {
              reject(error)
            } else {
              resolve({ stdout })
            }
          })
        })

        const path = cwdOutput.split('\n')[1]?.split('=')[1]?.trim() || ''
        process.path = path

        // 获取线程数
        const { stdout: threadOutput } = await new Promise((resolve, reject) => {
          this.cp(`wmic process where "ProcessId=${pid}" get ThreadCount /value`, (error, stdout, stderr) => {
            if (error) {
              reject(error)
            } else {
              resolve({ stdout })
            }
          })
        })

        const threadCount = parseInt(threadOutput.split('\n')[1]?.split('=')[1]?.trim() || '0')
        process.threads = threadCount

        // 获取句柄数
        const { stdout: handleOutput } = await new Promise((resolve, reject) => {
          this.cp(`wmic process where "ProcessId=${pid}" get HandleCount /value`, (error, stdout, stderr) => {
            if (error) {
              reject(error)
            } else {
              resolve({ stdout })
            }
          })
        })

        const handleCount = parseInt(handleOutput.split('\n')[1]?.split('=')[1]?.trim() || '0')
        process.handles = handleCount
      } catch (error) {
        console.error('获取进程详细信息失败:', error)
      }
    }

    return process
  }

  /**
   * 终止进程
   * @param {number} pid 进程ID
   */
  async terminateProcess(pid) {
    if (!this.isElectron || !this.cp) {
      throw new Error('无法在非 Electron 环境中终止进程')
    }

    try {
      await new Promise((resolve, reject) => {
        this.cp(`taskkill /PID ${pid} /F`, (error, stdout, stderr) => {
          if (error) {
            reject(error)
          } else {
            resolve({ stdout })
          }
        })
      })

      return { success: true, pid }
    } catch (error) {
      console.error('终止进程失败:', error)
      throw error
    }
  }

  /**
   * 强制终止进程
   * @param {number} pid 进程ID
   */
  async killProcess(pid) {
    // terminateProcess 已经是强制终止（/F 参数）
    return await this.terminateProcess(pid)
  }

  /**
   * 暂停进程
   * @param {number} pid 进程ID
   */
  async suspendProcess(pid) {
    if (!this.isElectron || !this.cp) {
      throw new Error('无法在非 Electron 环境中暂停进程')
    }

    try {
      await new Promise((resolve, reject) => {
        this.cp(`tasklist /FI "PID eq ${pid}" /FO CSV`, (error, stdout, stderr) => {
          if (error) {
            reject(error)
          } else {
            resolve({ stdout })
          }
        })
      })

      return { success: true, pid }
    } catch (error) {
      console.error('暂停进程失败:', error)
      throw error
    }
  }

  /**
   * 恢复进程
   * @param {number} pid 进程ID
   */
  async resumeProcess(pid) {
    if (!this.isElectron || !this.cp) {
      throw new Error('无法在非 Electron 环境中恢复进程')
    }

    try {
      await new Promise((resolve, reject) => {
        this.cp(`tasklist /FI "PID eq ${pid}" /FO CSV`, (error, stdout, stderr) => {
          if (error) {
            reject(error)
          } else {
            resolve({ stdout })
          }
        })
      })

      return { success: true, pid }
    } catch (error) {
      console.error('恢复进程失败:', error)
      throw error
    }
  }

  /**
   * 搜索进程
   * @param {string} query 搜索关键词
   */
  async searchProcesses(query) {
    const allProcesses = await this.getProcesses()
    const filtered = allProcesses.filter(process =>
      process.name.toLowerCase().includes(query.toLowerCase()) ||
      (process.path && process.path.toLowerCase().includes(query.toLowerCase())) ||
      process.pid.toString().includes(query) ||
      (process.command_line && process.command_line.toLowerCase().includes(query.toLowerCase()))
    )
    return filtered
  }

  /**
   * 开始自动刷新
   * @param {number} interval 刷新间隔（毫秒）
   */
  startAutoRefresh(interval = 2000) {
    if (this.refreshInterval) {
      this.stopAutoRefresh()
    }
    this.refreshInterval = setInterval(() => {
      this.getProcesses()
    }, interval)
  }

  /**
   * 停止自动刷新
   */
  stopAutoRefresh() {
    if (this.refreshInterval) {
      clearInterval(this.refreshInterval)
      this.refreshInterval = null
    }
  }

  /**
   * 获取系统性能统计
   */
  async getSystemStats() {
    // 在浏览器环境中，返回空数据
    if (!si || isBrowser) {
      return {
        cpuLoad: 0,
        memoryUsed: 0,
        memoryTotal: 0,
        memoryFree: 0,
        osName: 'Unknown',
        uptime: 0
      }
    }

    try {
      const [cpu, mem, osInfo] = await Promise.all([
        si.cpuCurrentLoad(),
        si.mem(),
        si.osInfo()
      ])

      return {
        cpuLoad: cpu.currentLoad,
        memoryUsed: mem.used,
        memoryTotal: mem.total,
        memoryFree: mem.free,
        osName: osInfo.platform,
        uptime: osInfo.uptime
      }
    } catch (error) {
      console.error('获取系统统计信息失败:', error)
      return {
        cpuLoad: 0,
        memoryUsed: 0,
        memoryTotal: 0,
        memoryFree: 0,
        osName: 'Unknown',
        uptime: 0
      }
    }
  }
}

// 创建单例
const processService = new ProcessService()

export default processService