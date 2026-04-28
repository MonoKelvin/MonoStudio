<template>
  <BasePanel class="panel process-manager-panel">
    <div class="process-manager-container">
      <div class="tool-header">
        <h3>{{ toolName }}</h3>
        <div class="header-actions">
          <BaseButton @click="refreshBasicList" :disabled="isLoadingBasic" size="sm">
            <span class="refresh-icon" :class="{ rotating: isLoadingBasic }">↻</span>
            刷新
          </BaseButton>
        </div>
      </div>

      <div class="stats-bar" v-if="systemStats">
        <div class="stat-item">
          <span class="stat-label">CPU</span>
          <div class="stat-bar">
            <div class="stat-fill cpu-fill" :style="{ width: systemStats.cpuLoad + '%' }"></div>
          </div>
          <span class="stat-value">{{ systemStats.cpuLoad }}%</span>
        </div>
        <div class="stat-item">
          <span class="stat-label">内存</span>
          <div class="stat-bar">
            <div class="stat-fill memory-fill" :style="{ width: memoryPercent + '%' }"></div>
          </div>
          <span class="stat-value">{{ formatMemory(systemStats.memoryUsed) }} / {{ formatMemory(systemStats.memoryTotal) }}</span>
        </div>
      </div>

      <div class="toolbar">
        <BaseInput
          v-model="searchQuery"
          placeholder="搜索进程名称或 PID..."
          clearable
          class="search-input"
        />
        <span class="process-count">{{ sortedProcesses.length }} 个进程</span>
      </div>

      <div class="process-list-container">
        <div v-if="isLoadingBasic && processes.length === 0" class="loading-state">
          <BaseSpinbox />
          <p>正在加载进程...</p>
        </div>

        <div v-else-if="sortedProcesses.length === 0" class="empty-state">
          <div class="empty-icon">📋</div>
          <p>{{ searchQuery ? '没有匹配的进程' : '暂无进程' }}</p>
        </div>

        <div v-else class="process-table">
          <div class="table-header">
            <div class="header-cell name-cell" @click="sortBy('name')">
              <span>进程名称</span>
              <span class="sort-icon" :class="getSortIcon('name')">▾</span>
            </div>
            <div class="header-cell pid-cell" @click="sortBy('pid')">
              <span>PID</span>
              <span class="sort-icon" :class="getSortIcon('pid')">▾</span>
            </div>
            <div class="header-cell memory-cell" @click="sortBy('memory')">
              <span>内存</span>
              <span class="sort-icon" :class="getSortIcon('memory')">▾</span>
            </div>
            <div class="header-cell actions-cell">操作</div>
          </div>
          <div class="table-body">
            <div
              v-for="process in displayedProcesses"
              :key="process.pid"
              class="table-row"
              @click="selectProcess(process)"
              :class="{ selected: selectedProcess?.pid === process.pid }"
            >
              <div class="row-cell name-cell">
                <ProcessIcon :name="process.name" />
                <span class="process-name">{{ process.name }}</span>
                <span v-if="process.loading" class="loading-text">...</span>
              </div>
              <div class="row-cell pid-cell">{{ process.pid }}</div>
              <div class="row-cell memory-cell">
                <span v-if="process.loading" class="loading-text">加载中</span>
                <span v-else>{{ formatMemory(process.memory) }}</span>
              </div>
              <div class="row-cell actions-cell" @click.stop>
                <BaseButton
                  variant="danger"
                  size="sm"
                  @click="terminateProcess(process)"
                  :disabled="isSystemProcess(process)"
                >
                  结束
                </BaseButton>
              </div>
            </div>
          </div>
        </div>
      </div>

      <!-- 进程详情对话框 -->
      <div v-if="showDetail" class="detail-overlay" @click="showDetail = false">
        <div class="detail-dialog" @click.stop>
          <div class="detail-header">
            <h3>进程详情</h3>
            <BaseButton variant="default" size="sm" @click="showDetail = false">×</BaseButton>
          </div>
          <div class="detail-content">
            <div v-if="detailLoading" class="detail-loading">
              <BaseSpinbox />
              <p>加载中...</p>
            </div>
            <div v-else-if="detailInfo" class="detail-info">
              <div class="info-row">
                <span class="info-label">进程名称</span>
                <span class="info-value">{{ detailInfo.name }}</span>
              </div>
              <div class="info-row">
                <span class="info-label">PID</span>
                <span class="info-value">{{ detailInfo.pid }}</span>
              </div>
              <div class="info-row">
                <span class="info-label">内存</span>
                <span class="info-value">{{ formatMemory(detailInfo.memory) }}</span>
              </div>
              <div class="info-row">
                <span class="info-label">CPU</span>
                <span class="info-value">{{ detailInfo.cpu }}%</span>
              </div>
              <div class="info-row">
                <span class="info-label">线程数</span>
                <span class="info-value">{{ detailInfo.threads }}</span>
              </div>
              <div class="info-row">
                <span class="info-label">句柄数</span>
                <span class="info-value">{{ detailInfo.handles }}</span>
              </div>
              <div class="info-row">
                <span class="info-label">路径</span>
                <span class="info-value path-value">{{ detailInfo.path || '-' }}</span>
              </div>
              <div class="info-row">
                <span class="info-label">公司</span>
                <span class="info-value">{{ detailInfo.company || '-' }}</span>
              </div>
            </div>
          </div>
          <div class="detail-footer">
            <BaseButton
              variant="danger"
              @click="terminateProcess(selectedProcess); showDetail = false"
              :disabled="selectedProcess && isSystemProcess(selectedProcess)"
            >
              结束进程
            </BaseButton>
            <BaseButton variant="default" @click="showDetail = false">关闭</BaseButton>
          </div>
        </div>
      </div>
    </div>
  </BasePanel>
</template>

<script>
import { ref, computed, onMounted, onUnmounted, shallowRef } from 'vue'
import BasePanel from '../../../components/base/BasePanel.vue'
import BaseButton from '../../../components/base/BaseButton.vue'
import BaseInput from '../../../components/base/BaseInput.vue'
import BaseSpinbox from '../../../components/base/BaseSpinbox.vue'
import ProcessIcon from './components/ProcessIcon.vue'

export default {
  name: 'ProcessManagerToolView',
  components: {
    BasePanel,
    BaseButton,
    BaseInput,
    BaseSpinbox,
    ProcessIcon
  },
  props: {
    toolName: {
      type: String,
      default: '进程管理'
    },
    toolDescription: {
      type: String,
      default: '查看和管理系统进程'
    }
  },
  setup() {
    // 使用 shallowRef 避免深度响应，提高性能
    const processes = shallowRef([])
    const searchQuery = ref('')
    const isLoadingBasic = ref(false)
    const selectedProcess = ref(null)
    const showDetail = ref(false)
    const detailLoading = ref(false)
    const detailInfo = shallowRef(null)
    const systemStats = shallowRef(null)

    // 排序状态
    const sortKey = ref('name')
    const sortDirection = ref('asc')

    let basicRefreshTimer = null
    let statsRefreshTimer = null

    // 过滤后排序的进程列表
    const filteredProcesses = computed(() => {
      if (!searchQuery.value) return processes.value
      const query = searchQuery.value.toLowerCase()
      return processes.value.filter(p =>
        p.name.toLowerCase().includes(query) ||
        p.pid.toString().includes(query)
      )
    })

    // 排序后的进程列表
    const sortedProcesses = computed(() => {
      const list = [...filteredProcesses.value]
      const key = sortKey.value
      const direction = sortDirection.value

      list.sort((a, b) => {
        let aVal = a[key]
        let bVal = b[key]

        // 处理 undefined 和 null
        if (aVal == null) aVal = ''
        if (bVal == null) bVal = ''

        // 字符串比较
        if (typeof aVal === 'string') {
          aVal = aVal.toLowerCase()
          bVal = bVal.toLowerCase()
        }

        if (direction === 'asc') {
          return aVal > bVal ? 1 : -1
        } else {
          return aVal < bVal ? 1 : -1
        }
      })

      return list
    })

    // 只显示前100个进程，避免一次渲染太多
    const displayedProcesses = computed(() => {
      return sortedProcesses.value.slice(0, 100)
    })

    const memoryPercent = computed(() => {
      const stats = systemStats.value
      if (!stats || !stats.memoryTotal) return 0
      return Math.round((stats.memoryUsed / stats.memoryTotal) * 100)
    })

    const isSystemProcess = (process) => {
      const systemProcesses = ['system', 'kernel', 'svchost', 'csrss', 'lsass', 'wininit', 'services', 'registry']
      const name = process?.name?.toLowerCase() || ''
      return systemProcesses.some(sp => name.includes(sp))
    }

    const formatMemory = (bytes) => {
      if (!bytes) return '0 B'
      if (bytes < 1024) return bytes + ' B'
      if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(0) + ' KB'
      if (bytes < 1024 * 1024 * 1024) return (bytes / (1024 * 1024)).toFixed(1) + ' MB'
      return (bytes / (1024 * 1024 * 1024)).toFixed(2) + ' GB'
    }

    // 排序方法
    const sortBy = (key) => {
      if (sortKey.value === key) {
        sortDirection.value = sortDirection.value === 'asc' ? 'desc' : 'asc'
      } else {
        sortKey.value = key
        sortDirection.value = key === 'name' ? 'asc' : 'desc'
      }
    }

    const getSortIcon = (key) => {
      if (sortKey.value !== key) return ''
      return sortDirection.value === 'asc' ? 'sort-asc' : 'sort-desc'
    }

    // 快速获取基本信息
    const refreshBasicList = async () => {
      isLoadingBasic.value = true
      try {
        if (window.electronAPI?.processes?.listBasic) {
          const list = await window.electronAPI.processes.listBasic()
          processes.value = list
        }
      } catch (error) {
        console.error('获取进程列表失败:', error)
      }
      isLoadingBasic.value = false
    }

    // 异步加载详细信息
    const loadFullInfo = async () => {
      try {
        if (window.electronAPI?.processes?.listFull) {
          const fullList = await window.electronAPI.processes.listFull()
          const fullMap = new Map(fullList.map(p => [p.pid, p]))
          processes.value = processes.value.map(p => {
            const full = fullMap.get(p.pid)
            return full ? { ...p, memory: full.memory, cpu: full.cpu, loading: false } : { ...p, loading: false }
          })
        }
      } catch (error) {
        console.error('获取详细进程信息失败:', error)
        // 标记加载完成
        processes.value = processes.value.map(p => ({ ...p, loading: false }))
      }
    }

    // 获取系统统计
    const refreshSystemStats = async () => {
      try {
        if (window.electronAPI?.systemStats?.get) {
          systemStats.value = await window.electronAPI.systemStats.get()
        }
      } catch (error) {
        console.error('获取系统统计失败:', error)
      }
    }

    // 选择进程查看详情
    const selectProcess = async (process) => {
      selectedProcess.value = process
      showDetail.value = true
      detailLoading.value = true
      detailInfo.value = null

      try {
        if (window.electronAPI?.processes?.getDetail) {
          detailInfo.value = await window.electronAPI.processes.getDetail(process.pid)
        }
      } catch (error) {
        console.error('获取进程详情失败:', error)
      }
      detailLoading.value = false
    }

    // 终止进程
    const terminateProcess = async (process) => {
      if (!process || isSystemProcess(process)) return
      try {
        if (window.electronAPI?.processes?.kill) {
          await window.electronAPI.processes.kill(process.pid)
          await refreshBasicList()
        }
      } catch (error) {
        console.error('终止进程失败:', error)
      }
    }

    // 页面可见性处理
    let isVisible = true
    const handleVisibilityChange = () => {
      isVisible = !document.hidden
      if (isVisible) {
        startRefresh()
      } else {
        stopRefresh()
      }
    }

    const startRefresh = () => {
      stopRefresh()
      refreshBasicList()
      refreshSystemStats()
      setTimeout(loadFullInfo, 500) // 延迟加载详细数据

      basicRefreshTimer = setInterval(() => {
        if (isVisible) {
          refreshBasicList()
          setTimeout(loadFullInfo, 500)
        }
      }, 10000)

      statsRefreshTimer = setInterval(() => {
        if (isVisible) {
          refreshSystemStats()
        }
      }, 3000)
    }

    const stopRefresh = () => {
      if (basicRefreshTimer) {
        clearInterval(basicRefreshTimer)
        basicRefreshTimer = null
      }
      if (statsRefreshTimer) {
        clearInterval(statsRefreshTimer)
        statsRefreshTimer = null
      }
    }

    onMounted(() => {
      document.addEventListener('visibilitychange', handleVisibilityChange)
      startRefresh()
    })

    onUnmounted(() => {
      document.removeEventListener('visibilitychange', handleVisibilityChange)
      stopRefresh()
    })

    return {
      processes,
      searchQuery,
      isLoadingBasic,
      selectedProcess,
      showDetail,
      detailLoading,
      detailInfo,
      systemStats,
      sortedProcesses,
      displayedProcesses,
      memoryPercent,
      sortKey,
      sortDirection,
      isSystemProcess,
      formatMemory,
      sortBy,
      getSortIcon,
      refreshBasicList,
      selectProcess,
      terminateProcess
    }
  }
}
</script>

<style scoped>
.process-manager-panel {
  height: 100%;
}

.process-manager-container {
  display: flex;
  flex-direction: column;
  height: 100%;
  padding: 16px;
  gap: 12px;
  font-size: 13px;
  color: var(--text-primary);
}

.tool-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.tool-header h3 {
  margin: 0;
  font-size: 16px;
  font-weight: 600;
  color: var(--text-primary);
}

.header-actions {
  display: flex;
  gap: 8px;
}

.refresh-icon {
  display: inline-block;
  margin-right: 4px;
}

.refresh-icon.rotating {
  animation: spin 1s linear infinite;
}

@keyframes spin {
  from { transform: rotate(0deg); }
  to { transform: rotate(360deg); }
}

.stats-bar {
  display: flex;
  gap: 24px;
  padding: 12px 16px;
  background: var(--surface-2);
  border-radius: var(--radius-md);
}

.stat-item {
  flex: 1;
  display: flex;
  align-items: center;
  gap: 10px;
}

.stat-label {
  font-size: 12px;
  color: var(--text-secondary);
  min-width: 28px;
}

.stat-bar {
  flex: 1;
  height: 6px;
  background: var(--surface-3);
  border-radius: 3px;
  overflow: hidden;
  max-width: 120px;
}

.stat-fill {
  height: 100%;
  transition: width 0.3s ease;
  border-radius: 3px;
}

.cpu-fill {
  background: linear-gradient(90deg, var(--success), var(--warning), var(--danger));
}

.memory-fill {
  background: var(--accent-color);
}

.stat-value {
  font-size: 12px;
  color: var(--text-primary);
  min-width: 90px;
  text-align: right;
  font-family: monospace;
}

.toolbar {
  display: flex;
  align-items: center;
  gap: 12px;
}

.search-input {
  width: 220px;
}

.process-count {
  font-size: 12px;
  color: var(--text-secondary);
}

.process-list-container {
  flex: 1;
  overflow: hidden;
  background: var(--surface-2);
  border-radius: var(--radius-md);
  border: 1px solid var(--border-color);
}

.loading-state,
.empty-state {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  height: 200px;
  gap: 12px;
  color: var(--text-secondary);
}

.empty-icon {
  font-size: 32px;
  opacity: 0.4;
}

.process-table {
  display: flex;
  flex-direction: column;
  height: 100%;
}

.table-header {
  display: flex;
  background: var(--surface-3);
  border-bottom: 1px solid var(--border-color);
  font-size: 13px;
  font-weight: 600;
}

.header-cell {
  padding: 12px 12px;
  color: var(--text-secondary);
  user-select: none;
}

.header-cell.sortable {
  cursor: pointer;
  transition: color 0.2s;
}

.header-cell.sortable:hover {
  color: var(--text-primary);
}

.header-cell.name-cell {
  flex: 1;
  display: flex;
  align-items: center;
  gap: 6px;
  cursor: pointer;
}

.header-cell.pid-cell {
  width: 80px;
  text-align: center;
  cursor: pointer;
}

.header-cell.memory-cell {
  width: 100px;
  text-align: right;
  cursor: pointer;
}

.header-cell.actions-cell {
  width: 80px;
  text-align: center;
}

.sort-icon {
  font-size: 10px;
  opacity: 0.5;
}

.sort-icon.sort-asc,
.sort-icon.sort-desc {
  opacity: 1;
  color: var(--accent-color);
}

.table-body {
  overflow-y: auto;
  max-height: calc(100% - 44px);
}

.table-row {
  display: flex;
  border-bottom: 1px solid var(--border-color);
  transition: background 0.15s;
  cursor: pointer;
}

.table-row:hover {
  background: var(--surface-3);
}

.table-row.selected {
  background: color-mix(in srgb, var(--accent-color) 12%, var(--surface-2));
}

.row-cell {
  padding: 10px 12px;
  display: flex;
  align-items: center;
  font-size: 13px;
  color: var(--text-primary);
}

.name-cell {
  flex: 1;
  gap: 8px;
  min-width: 0;
}

.process-name {
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}

.loading-text {
  color: var(--text-tertiary);
  font-size: 12px;
}

.pid-cell {
  width: 80px;
  justify-content: center;
  font-family: monospace;
  font-size: 12px;
}

.memory-cell {
  width: 100px;
  justify-content: flex-end;
  font-family: monospace;
  font-size: 12px;
}

.actions-cell {
  width: 80px;
  justify-content: center;
}

/* 详情对话框 */
.detail-overlay {
  position: fixed;
  inset: 0;
  background: rgba(0, 0, 0, 0.5);
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 1000;
}

.detail-dialog {
  background: var(--surface-2);
  border-radius: var(--radius-lg);
  width: 420px;
  max-height: 80vh;
  display: flex;
  flex-direction: column;
  box-shadow: 0 20px 40px rgba(0, 0, 0, 0.3);
  border: 1px solid var(--border-color);
}

.detail-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 14px 16px;
  border-bottom: 1px solid var(--border-color);
}

.detail-header h3 {
  margin: 0;
  font-size: 15px;
  font-weight: 600;
}

.detail-content {
  flex: 1;
  overflow-y: auto;
  padding: 16px;
}

.detail-loading {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 12px;
  padding: 32px;
  color: var(--text-secondary);
}

.detail-info {
  display: flex;
  flex-direction: column;
}

.info-row {
  display: flex;
  padding: 10px 0;
  border-bottom: 1px solid var(--border-color);
}

.info-label {
  width: 70px;
  color: var(--text-secondary);
  font-size: 13px;
  flex-shrink: 0;
}

.info-value {
  flex: 1;
  font-size: 13px;
  color: var(--text-primary);
  word-break: break-all;
}

.path-value {
  font-family: monospace;
  font-size: 11px;
}

.detail-footer {
  display: flex;
  justify-content: flex-end;
  gap: 8px;
  padding: 14px 16px;
  border-top: 1px solid var(--border-color);
}
</style>