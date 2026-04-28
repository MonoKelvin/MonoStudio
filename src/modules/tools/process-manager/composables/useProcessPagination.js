/**
 * 进程分页逻辑
 */
import { ref, computed } from 'vue'
import { PAGE_SIZE } from '../utils/processConstants'

export function useProcessPagination(totalItems) {
  const currentPage = ref(1)

  const totalPages = computed(() => {
    return Math.ceil(totalItems.value / PAGE_SIZE)
  })

  const paginatedItems = computed(() => {
    const start = (currentPage.value - 1) * PAGE_SIZE
    const end = start + PAGE_SIZE
    return totalItems.value.slice(start, end)
  })

  const prevPage = () => {
    if (currentPage.value > 1) {
      currentPage.value--
    }
  }

  const nextPage = () => {
    if (currentPage.value < totalPages.value) {
      currentPage.value++
    }
  }

  const goToPage = (page) => {
    if (page >= 1 && page <= totalPages.value) {
      currentPage.value = page
    }
  }

  const resetPage = () => {
    currentPage.value = 1
  }

  return {
    currentPage,
    totalPages,
    paginatedItems,
    prevPage,
    nextPage,
    goToPage,
    resetPage
  }
}
