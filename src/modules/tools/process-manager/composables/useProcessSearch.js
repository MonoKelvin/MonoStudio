/**
 * 进程搜索逻辑
 */
import { ref } from 'vue'

export function useProcessSearch() {
  const searchQuery = ref('')

  const handleSearch = (value) => {
    return value // 返回处理后的值，由父组件使用
  }

  const clearSearch = () => {
    searchQuery.value = ''
  }

  const setSearch = (value) => {
    searchQuery.value = value
  }

  return {
    searchQuery,
    handleSearch,
    clearSearch,
    setSearch
  }
}
