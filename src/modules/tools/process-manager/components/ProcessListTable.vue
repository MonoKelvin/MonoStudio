<template>
  <div class="table-wrapper" ref="tableWrapper">
    <div v-if="isLoading && processes.length === 0" class="loading-state">
      <BaseSpinbox size="lg" />
      <p>正在加载进程列表...</p>
      <div class="loading-dots">
        <span class="dot"></span>
        <span class="dot"></span>
        <span class="dot"></span>
      </div>
    </div>

    <div v-else-if="filteredProcesses.length === 0" class="empty-state">
      <div class="empty-icon">📋</div>
      <h3 class="empty-title">{{ processes.length === 0 ? '暂无进程' : '没有匹配的进程' }}</h3>
      <p class="empty-desc">{{ processes.length === 0 ? '系统可能没有运行任何进程' : '尝试调整搜索条件' }}</p>
      <div class="empty-actions">
        <BaseButton variant="primary" size="sm" @click="handleRefresh">
          刷新列表
        </BaseButton>
      </div>
    </div>

    <Transition v-if="filteredProcesses.length > 0" name="list-enter" mode="out-in">
      <div class="process-table">
        <!-- 表头 -->
        <div class="table-header">
          <div v-for="column in columns" :key="column.key" class="header-cell"
              :class="{ sortable: column.sortable }" :style="{ width: column.width }"
              @click="handleSort(column.key)">
            <span>{{ column.title }}</span>
            <span v-if="column.sortable" class="sort-icon">
              <span :class="getSortIconClass(column.key)">{{ getSortIcon(column.key) }}</span>
            </span>
          </div>
        </div>

        <!-- 表格内容 -->
        <div class="table-body" @scroll="handleScroll" :style="{ maxHeight: tableHeight }">
          <TransitionGroup name="list-item" tag="div" class="table-body-content">
            <ProcessListRow
              v-for="process in paginatedProcesses"
              :key="process.pid"
              :process="process"
              :column="column"
              :selected-pid="selectedProcess?.pid"
              :hovered-pid="hoveredProcess?.pid"
              @click="handleClick"
              @double-click="handleDoubleClick"
              @row-hover="handleRowHover"
              @suspend-resume="handleSuspendResume"
              @terminate="handleTerminate"
              @context-menu="handleContextMenu"
            />
          </TransitionGroup>
        </div>

        <!-- 分页 -->
        <Transition name="fade">
          <div v-if="filteredProcesses.length > PAGE_SIZE" class="pagination">
            <BaseButton variant="default" size="sm" @click="prevPage" :disabled="currentPage === 1"
                class="page-btn">
              上一页
            </BaseButton>
            <span class="page-info">
              第 {{ currentPage }} 页，共 {{ totalPages }} 页
            </span>
            <BaseButton variant="default" size="sm" @click="nextPage" :disabled="currentPage === totalPages"
                class="page-btn">
              下一页
            </BaseButton>
          </div>
        </Transition>
      </div>
    </Transition>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, onUnmounted, nextTick, watch } from 'vue'
import BaseButton from '../../../../components/base/BaseButton.vue'
import BaseSpinbox from '../../../../components/base/BaseSpinbox.vue'
import ProcessListRow from './ProcessListRow.vue'
import { COLUMN_CONFIG, PAGE_SIZE } from '../utils/processConstants'
import { useProcessSort } from '../composables/useProcessSort'
import { useProcessPagination } from '../composables/useProcessPagination'

const props = defineProps({
  processes: {
    type: Array,
    default: () => []
  },
  autoRefreshEnabled: {
    type: Boolean,
    default: true
  },
  isLoading: {
    type: Boolean,
    default: false
  },
  selectedProcess: {
    type: Object,
    default: null
  },
  hoveredProcess: {
    type: Object,
    default: null
  },
  systemStats: {
    type: Object,
    default: () => ({})
  },
  showStats: {
    type: Boolean,
    default: true
  },
  searchQuery: {
    type: String,
    default: ''
  }
})

const emit = defineEmits(['refresh', 'toggle-auto-refresh', 'select-process', 'double-click', 'terminate'])

const columns = ref(COLUMN_CONFIG)
const showColumnMenu = ref(false)
const tableHeight = ref('100%')
const tableWrapper = ref(null)

// 使用组合式函数
const {
  sortKey,
  sortDirection,
  handleSort,
  getSortIcon,
  getSortIconClass
} = useProcessSort()

const filteredProcessesRef = computed(() => filteredProcesses.value)

const {
  currentPage,
  totalPages,
  paginatedProcesses,
  prevPage,
  nextPage,
  resetPage
} = useProcessPagination(filteredProcessesRef)

// 监听搜索变化，重置分页
watch(() => props.searchQuery, () => {
  resetPage()
})

// 过滤和排序
const filteredProcesses = computed(() => {
  let result = [...props.processes]

  // 搜索过滤
  if (props.searchQuery) {
    const query = props.searchQuery.toLowerCase()
    result = result.filter(process =>
      process.name.toLowerCase().includes(query) ||
      (process.path && process.path.toLowerCase().includes(query)) ||
      process.pid.toString().includes(query) ||
      (process.command_line && process.command_line.toLowerCase().includes(query))
    )
  }

  // 排序
  if (sortKey.value) {
    result.sort((a, b) => {
      let aVal = a[sortKey.value]
      let bVal = b[sortKey.value]

      if (typeof aVal === 'string') {
        aVal = aVal.toLowerCase()
        bVal = bVal.toLowerCase()
      }

      if (sortDirection.value === 'asc') {
        return aVal > bVal ? 1 : -1
      } else {
        return aVal < bVal ? 1 : -1
      }
    })
  }

  return result
})

// 方法
const handleRefresh = () => {
  emit('refresh')
}

const handleClick = (process) => {
  emit('select-process', process)
}

const handleDoubleClick = (process) => {
  emit('double-click', process)
}

const handleRowHover = (pid) => {
  // 可以在这里记录当前悬停的进程
}

const handleScroll = () => {
  // 可以在这里实现无限滚动或加载更多
}

const handleTerminate = async (process) => {
  emit('terminate', process)
}

const handleContextMenu = (event, process) => {
  // 可以在这里显示自定义右键菜单
  console.log('右键菜单:', process)
}

const handleColumnToggle = (column) => {
  // 列显示/隐藏逻辑在父组件中处理
  showColumnMenu.value = false
}

const handleSearch = (value) => {
  setSearch(value)
  resetPage()
}

// 计算表格高度
const calculateTableHeight = () => {
  nextTick(() => {
    if (tableWrapper.value) {
      const containerHeight = tableWrapper.value.parentElement?.clientHeight || 500
      const controlsHeight = 200 // 估算的控制区域高度
      const paginationHeight = 50
      tableHeight.value = `${containerHeight - controlsHeight - paginationHeight}px`
    }
  })
}

// 生命周期
onMounted(() => {
  window.addEventListener('resize', calculateTableHeight)
  calculateTableHeight()
})

onUnmounted(() => {
  window.removeEventListener('resize', calculateTableHeight)
})

// 暴露方法
defineExpose({
  calculateTableHeight,
  handleRefresh,
  handleSort
})
</script>

<style scoped>
.table-wrapper {
  flex: 1;
  overflow: hidden;
  display: flex;
  flex-direction: column;
}

.loading-state,
.empty-state {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  height: 300px;
  color: var(--text-secondary);
  gap: 16px;
}

.loading-state p,
.empty-state p {
  font-size: 14px;
  color: var(--text-tertiary);
}

.loading-dots {
  display: flex;
  gap: 4px;
  margin-top: 12px;
}

.dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background: var(--accent-color);
  animation: bounce 1.4s infinite ease-in-out both;
}

.dot:nth-child(1) {
  animation-delay: -0.32s;
}

.dot:nth-child(2) {
  animation-delay: -0.16s;
}

@keyframes bounce {
  0%, 80%, 100% {
    transform: scale(0);
    opacity: 0.5;
  }
  40% {
    transform: scale(1);
    opacity: 1;
  }
}

.empty-icon {
  font-size: 48px;
  opacity: 0.3;
  animation: float 3s ease-in-out infinite;
}

.empty-title {
  margin: 0 0 8px;
  font-size: 16px;
  font-weight: 600;
  color: var(--text-secondary);
  animation: fadeIn 0.5s ease-out;
}

.empty-desc {
  margin: 0 0 16px;
  font-size: 14px;
  color: var(--text-tertiary);
  animation: fadeIn 0.5s ease-out 0.1s both;
}

.empty-actions {
  animation: fadeIn 0.5s ease-out 0.2s both;
}

@keyframes float {
  0%, 100% {
    transform: translateY(0);
  }
  50% {
    transform: translateY(-8px);
  }
}

@keyframes fadeIn {
  from {
    opacity: 0;
    transform: translateY(4px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

.process-table {
  display: flex;
  flex-direction: column;
  height: 100%;
}

.table-header {
  display: flex;
  background: var(--bg-tertiary);
  border-bottom: 1px solid var(--border-color);
  position: sticky;
  top: 0;
  z-index: 1;
}

.header-cell {
  display: flex;
  align-items: center;
  padding: 12px 16px;
  font-size: 13px;
  font-weight: 500;
  color: var(--text-secondary);
  user-select: none;
}

.header-cell.sortable {
  cursor: pointer;
  transition: background-color 0.2s ease;
}

.header-cell.sortable:hover {
  background: color-mix(in srgb, var(--bg-elevated) 50%, var(--hover-bg) 50%);
}

.sort-icon {
  margin-left: 4px;
  font-size: 12px;
  transition: color 0.2s ease;
}

.sort-asc {
  color: var(--accent-color);
}

.sort-desc {
  color: var(--accent-color);
}

.sort-default {
  color: var(--text-tertiary);
}

.table-body {
  overflow-y: auto;
  height: 100%;
}

.table-body-content {
  transition: all 0.3s ease;
}

/* 列表进入动画 */
.list-enter-active {
  transition: opacity 0.3s ease, transform 0.3s ease;
}

.list-enter-from {
  opacity: 0;
  transform: translateY(10px);
}

.list-leave-active {
  transition: opacity 0.2s ease, transform 0.2s ease;
  position: absolute;
  width: 100%;
}

.list-leave-to {
  opacity: 0;
  transform: translateX(20px);
}

/* 列表项动画 */
.list-item-enter-active {
  transition: all 0.3s ease;
}

.list-item-enter-from {
  opacity: 0;
  transform: translateX(-20px);
}

.list-item-leave-active {
  transition: all 0.2s ease;
}

.list-item-leave-to {
  opacity: 0;
  transform: translateX(20px);
}

/* 淡入淡出动画 */
.fade-enter-active,
.fade-leave-active {
  transition: opacity 0.2s ease, transform 0.2s ease;
}

.fade-enter-from,
.fade-leave-to {
  opacity: 0;
  transform: translateY(-4px);
}

/* 分页 */
.pagination {
  display: flex;
  justify-content: center;
  align-items: center;
  padding: 16px;
  background: var(--bg-elevated);
  border-top: 1px solid var(--border-color);
  gap: 16px;
}

.page-btn {
  min-width: 80px;
}

.page-info {
  font-size: 14px;
  color: var(--text-secondary);
}

/* 滚动条样式 */
.table-body::-webkit-scrollbar {
  width: 8px;
}

.table-body::-webkit-scrollbar-track {
  background: var(--bg-primary);
}

.table-body::-webkit-scrollbar-thumb {
  background: var(--border-color);
  border-radius: 4px;
}

.table-body::-webkit-scrollbar-thumb:hover {
  background: var(--text-tertiary);
}
</style>
