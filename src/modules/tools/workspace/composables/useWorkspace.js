import { ref, computed } from 'vue';

export function useWorkspace() {
  const openWindows = ref([]);
  const savedWorkspaces = ref([]);
  const isLoading = ref(false);
  const isRestoring = ref(false);
  const activeTab = ref('current');

  const windowCount = computed(() => openWindows.value.length);

  const fetchOpenWindows = async () => {
    isLoading.value = true;
    try {
      const windows = await window.monoWorkspace.getOpenWindows();
      openWindows.value = windows || [];
    } catch (e) {
      console.error('Failed to get open windows:', e);
      openWindows.value = [];
    } finally {
      isLoading.value = false;
    }
  };

  const fetchSavedWorkspaces = async () => {
    isLoading.value = true;
    try {
      const workspaces = await window.monoWorkspace.list();
      savedWorkspaces.value = workspaces || [];
    } catch (e) {
      console.error('Failed to get saved workspaces:', e);
      savedWorkspaces.value = [];
    } finally {
      isLoading.value = false;
    }
  };

  const saveCurrentWorkspace = async (name) => {
    if (!name || openWindows.value.length === 0) {
      return { success: false, error: '没有可保存的窗口' };
    }

    isLoading.value = true;
    try {
      const result = await window.monoWorkspace.save(name, openWindows.value);
      if (result.success) {
        await fetchSavedWorkspaces();
      }
      return result;
    } catch (e) {
      console.error('Failed to save workspace:', e);
      return { success: false, error: e.message };
    } finally {
      isLoading.value = false;
    }
  };

  const deleteWorkspace = async (workspace) => {
    isLoading.value = true;
    try {
      const result = await window.monoWorkspace.delete(workspace.filepath);
      if (result.success) {
        await fetchSavedWorkspaces();
      }
      return result;
    } catch (e) {
      console.error('Failed to delete workspace:', e);
      return { success: false, error: e.message };
    } finally {
      isLoading.value = false;
    }
  };

  const restoreWorkspace = async (workspace) => {
    isRestoring.value = true;
    try {
      const workspaceData = await window.monoWorkspace.load(workspace.filepath);
      if (!workspaceData) {
        return { success: false, error: '无法加载工作空间数据' };
      }

      const results = await window.monoWorkspace.restore(workspaceData);
      return { success: true, results };
    } catch (e) {
      console.error('Failed to restore workspace:', e);
      return { success: false, error: e.message };
    } finally {
      isRestoring.value = false;
    }
  };

  const formatTimestamp = (timestamp) => {
    const date = new Date(timestamp);
    const now = new Date();
    const diff = now - date;

    if (diff < 60000) return '刚刚';
    if (diff < 3600000) return `${Math.floor(diff / 60000)} 分钟前`;
    if (diff < 86400000) return `${Math.floor(diff / 3600000)} 小时前`;
    if (diff < 604800000) return `${Math.floor(diff / 86400000)} 天前`;

    return date.toLocaleDateString('zh-CN', {
      year: 'numeric',
      month: '2-digit',
      day: '2-digit',
      hour: '2-digit',
      minute: '2-digit'
    });
  };

  return {
    openWindows,
    savedWorkspaces,
    isLoading,
    isRestoring,
    activeTab,
    windowCount,
    fetchOpenWindows,
    fetchSavedWorkspaces,
    saveCurrentWorkspace,
    deleteWorkspace,
    restoreWorkspace,
    formatTimestamp
  };
}
