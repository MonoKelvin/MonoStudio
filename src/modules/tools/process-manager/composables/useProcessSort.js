/**
 * 进程排序逻辑
 */
import { ref, computed } from 'vue'

export function useProcessSort() {
  const sortKey = ref('cpu')
  const sortDirection = ref('desc')

  const handleSort = (key) => {
    if (sortKey.value === key) {
      sortDirection.value = sortDirection.value === 'asc' ? 'desc' : 'asc'
    } else {
      sortKey.value = key
      sortDirection.value = 'desc'
    }
  }

  const getSortIcon = (columnKey) => {
    if (sortKey.value !== columnKey) return '⇅'
    return sortDirection.value === 'asc' ? '↑' : '↓'
  }

  const getSortIconClass = (columnKey) => {
    if (sortKey.value !== columnKey) return 'sort-default'
    return sortDirection.value === 'asc' ? 'sort-asc' : 'sort-desc'
  }

  return {
    sortKey,
    sortDirection,
    handleSort,
    getSortIcon,
    getSortIconClass
  }
}
