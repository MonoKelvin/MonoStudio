<template>
  <div class="process-list-container" ref="listContainer">
    <ProcessListToolbar
      ref="toolbarRef"
      :is-refreshing="isRefreshing"
      :auto-refresh-enabled="autoRefreshEnabled"
      :search-query="searchQuery"
      :columns="availableColumns"
      :show-stats="showStats"
      @refresh="handleRefresh"
      @toggle-auto-refresh="toggleAutoRefresh"
      @search="handleSearch"
      @toggle-stats="toggleStats"
      @toggle-column-menu="toggleColumnMenu"
      @column-toggle="handleColumnToggle"
    />

    <ProcessListStats
      v-if="showStats"
      :show="true"
      :cpu-load="systemStats.cpuLoad"
      :memory-used="systemStats.memoryUsed"
      :memory-total="systemStats.memoryTotal"
      :total-processes="processes.length"
    />

    <ProcessListTable
      ref="tableRef"
      :processes="processes"
      :auto-refresh-enabled="autoRefreshEnabled"
      :is-loading="isLoading"
      :selected-process="selectedProcess"
      :hovered-process="hoveredProcess"
      :system-stats="systemStats"
      :show-stats="showStats"
      :search-query="searchQuery"
      @refresh="handleRefresh"
      @toggle-auto-refresh="toggleAutoRefresh"
      @select-process="handleSelectProcess"
      @double-click="handleDoubleClick"
      @terminate="handleTerminate"
    />
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from 'vue'
import ProcessListToolbar from './ProcessListToolbar.vue'
import ProcessListStats from './ProcessListStats.vue'
import ProcessListTable from './ProcessListTable.vue'
import processService from '../services/ProcessService'
import dialogService from '../../../../common/services/dialogService'

const props = defineProps({
  toolName: {
    type: String,
    default: '进程管理'
  },
  toolDescription: {
    type: String,
    default: '查看和管理系统进程'
  }
})

const emit = defineEmits(['change-theme', 'change-accent-color', 'show-detail'])

// 状态
const processes = ref([])
const selectedProcess = ref(null)
const autoRefreshEnabled = ref(true)
const showStats = ref(true)
const isLoading = ref(false)
const isVisible = ref(true)
const refreshTimer = ref(null)
const hoveredProcess = ref(null)

// 组件引用
const listContainer = ref(null)
const toolbarRef = ref(null)
const tableRef = ref(null)

const availableColumns = [
  { key: 'name', title: '进程名称', visible: true, width: '250px' },
  { key: 'pid', title: 'PID', visible: true, width: '80px' },
  { key: 'memory', title: '内存', visible: true, width: '120px' },
  { key: 'cpu', title: 'CPU', visible: true, width: '100px' },
  { key: 'status', title: '状态', visible: true, width: '100px' },
  { key: 'actions', title: '操作', visible: true, width: '150px' }
]

const showColumnMenu = ref(false)

// 系统统计
const systemStats = ref({
  cpuLoad: 0,
  memoryUsed: 0,
  memoryTotal: 0,
  memoryFree: 0
})

// 页面可见性处理函数
const handleVisibilityChange = () => {
  isVisible.value = !document.hidden
  if (isVisible.value && autoRefreshEnabled.value) {
    loadProcesses()
  }
}

// 快捷键处理函数
const handleKeyPress = (event) => {
  if ((event.ctrlKey || event.metaKey) && event.key === 'r') {
    event.preventDefault()
    handleRefresh()
  }
  if ((event.ctrlKey || event.metaKey) && event.key === 'f') {
    event.preventDefault()
    toolbarRef.value?.focusSearch()
  }
  if (event.key === 'Escape') {
    showColumnMenu.value = false
  }
}

// 生命周期
onMounted(async () => {
  await loadProcesses()

  document.addEventListener('visibilitychange', handleVisibilityChange)
  document.addEventListener('keydown', handleKeyPress)

  if (autoRefreshEnabled.value) {
    startAutoRefresh()
  }
})

onUnmounted(() => {
  document.removeEventListener('visibilitychange', handleVisibilityChange)
  document.removeEventListener('keydown', handleKeyPress)
  stopAutoRefresh()
})

// 方法
const loadProcesses = async () => {
  if (isLoading.value) return

  try {
    isLoading.value = true
    processes.value = await processService.getProcesses()
    await loadSystemStats()
  } catch (error) {
    console.error('加载进程列表失败:', error)
    dialogService.error('加载进程列表失败', '错误')
  } finally {
    isLoading.value = false
  }
}

const loadSystemStats = async () => {
  try {
    const stats = await processService.getSystemStats()
    systemStats.value = stats
  } catch (error) {
    console.error('加载系统统计失败:', error)
  }
}

const handleRefresh = async () => {
  await loadProcesses()
  await loadSystemStats()
}

const toggleAutoRefresh = () => {
  autoRefreshEnabled.value = !autoRefreshEnabled.value

  if (autoRefreshEnabled.value) {
    startAutoRefresh()
  } else {
    stopAutoRefresh()
  }
}

const startAutoRefresh = () => {
  stopAutoRefresh()
  refreshTimer.value = setInterval(() => {
    if (isVisible.value) {
      loadProcesses()
    }
  }, 3000)
}

const stopAutoRefresh = () => {
  if (refreshTimer.value) {
    clearInterval(refreshTimer.value)
    refreshTimer.value = null
  }
}

const handleSelectProcess = (process) => {
  selectedProcess.value = process
  hoveredProcess.value = process
}

const handleDoubleClick = (process) => {
  selectedProcess.value = process
  showColumnMenu.value = false
  emit('show-detail', process)
}

const handleTerminate = async (process) => {
  if (!process) return

  try {
    const result = await processService.terminateProcess(process.pid)
    if (result.success) {
      processes.value = processes.value.filter(p => p.pid !== process.pid)
      dialogService.success('进程已成功终止', '成功')
    }
  } catch (error) {
    console.error('终止进程失败:', error)
    dialogService.error('终止进程失败', '错误')
  }
}

const toggleStats = () => {
  showStats.value = !showStats.value
}

const toggleColumnMenu = () => {
  showColumnMenu.value = !showColumnMenu.value
}

const handleColumnToggle = (column) => {
  showColumnMenu.value = false
}

// 暴露方法
defineExpose({
  loadProcesses,
  handleRefresh,
  toggleAutoRefresh
})
</script>

<style scoped>
.process-list-container {
  display: flex;
  flex-direction: column;
  height: 100%;
  background: var(--bg-primary);
  border-radius: var(--radius-md);
  overflow: hidden;
}
</style>
