import { onMounted } from 'vue';
import { useTheme } from './useTheme';
import { useTools } from './useTools';

/**
 * 工作台管理组合式 API
 * @returns {Object} 工作台相关状态和方法
 */
export function useWorkbench() {
  const {
    currentTheme,
    currentAccentColor,
    themes,
    setTheme,
    setAccentColor
  } = useTheme();

  const {
    searchQuery,
    selectedTool,
    tools,
    settingsEntry,
    personalLifeGate,
    personalLifeUnlocked,
    filteredTools,
    toolsByCategory,
    currentTool,
    setSelectedTool,
    initializeSelectedTool,
    unlockPersonalLife,
    lockPersonalLife
  } = useTools();

  onMounted(() => {
    initializeSelectedTool();
  });

  return {
    searchQuery,
    selectedTool,
    currentTheme,
    currentAccentColor,
    themes,
    tools,
    settingsEntry,
    personalLifeGate,
    personalLifeUnlocked,
    filteredTools,
    toolsByCategory,
    currentTool,
    setTheme,
    setAccentColor,
    setSelectedTool,
    unlockPersonalLife,
    lockPersonalLife
  };
}
