import { computed, markRaw, ref, watch } from 'vue';
import DesktopIconToolView from '../tools/desktop-icon/DesktopIconToolView.vue';
import FileSearchToolView from '../tools/file-search/FileSearchToolView.vue';
import SystemInfoToolView from '../tools/system-info/SystemInfoToolView.vue';
import NetworkToolsToolView from '../tools/network-tools/NetworkToolsToolView.vue';
import ProcessManagerToolView from '../tools/process-manager/ProcessManagerToolView.vue';
import GuidGeneratorToolView from '../tools/guid/GuidGeneratorToolView.vue';
import JsonFormatterToolView from '../tools/json-formatter/JsonFormatterToolView.vue';
import Base64EncoderToolView from '../tools/base64-encoder/Base64EncoderToolView.vue';
import RegexTesterToolView from '../tools/regex-tester/RegexTesterToolView.vue';
import ImageConverterToolView from '../tools/image/ImageConverterToolView.vue';
import VideoConverterToolView from '../tools/video-converter/VideoConverterToolView.vue';
import AudioToolsToolView from '../tools/audio-tools/AudioToolsToolView.vue';
import ScreenCaptureToolView from '../tools/screen-capture/ScreenCaptureToolView.vue';
import PasswordGeneratorToolView from '../tools/password-generator/PasswordGeneratorToolView.vue';
import UnitConverterToolView from '../tools/unit-converter/UnitConverterToolView.vue';
import TimeToolsToolView from '../tools/time-tools/TimeToolsToolView.vue';
import FileHasherToolView from '../tools/file-hasher/FileHasherToolView.vue';
import InspirationToolView from '../tools/inspiration/InspirationToolView.vue';
import WorkNotesToolView from '../tools/work-notes/WorkNotesToolView.vue';
import MusingsToolView from '../tools/musings/MusingsToolView.vue';
import SettingsToolView from '../tools/settings/SettingsToolView.vue';
import PasswordGate from '../../components/PasswordGate.vue';
import { TOOL_CONFIG } from '../../config/toolConfig';

const SELECTED_TOOL_KEY = TOOL_CONFIG.SELECTED_TOOL_KEY;
const PERSONAL_LIFE_CATEGORY = '个人生活';
const SECURITY_SETTINGS_KEY = 'personal-life-security-settings';
const LAST_UNLOCK_TIME_KEY = 'personal-life-last-unlock';

/**
 * 工具管理组合式 API
 * @returns {Object} 工具相关状态和方法
 */
export function useTools() {
  const searchQuery = ref('');
  const selectedTool = ref('desktop-icon');
  const previousTool = ref('desktop-icon');
  const personalLifeUnlocked = ref(false);
  const securitySettings = ref({
    requirePasswordOnInactive: false,
    requirePasswordEveryTime: false,
    requirePasswordByTime: false,
    passwordTimeout: 30, // 默认30分钟
    keepSession: true
  });
  const lastUnlockTime = ref(0);

  const settingsEntry = {
    ...TOOL_CONFIG.SETTINGS_ENTRY,
    component: markRaw(SettingsToolView)
  };

  const personalLifeGate = {
    id: 'personal-life-gate',
    name: '个人生活',
    description: '',
    category: null,
    isLockGate: true,
    component: markRaw(PasswordGate)
  };

  const tools = TOOL_CONFIG.TOOLS.map(tool => ({
    ...tool,
    component: markRaw({
      'desktop-icon': DesktopIconToolView,
      'file-search': FileSearchToolView,
      'system-info': SystemInfoToolView,
      'network-tools': NetworkToolsToolView,
      'process-manager': ProcessManagerToolView,
      'guid-generator': GuidGeneratorToolView,
      'json-formatter': JsonFormatterToolView,
      'base64-encoder': Base64EncoderToolView,
      'regex-tester': RegexTesterToolView,
      'image-converter': ImageConverterToolView,
      'video-converter': VideoConverterToolView,
      'audio-tools': AudioToolsToolView,
      'screen-capture': ScreenCaptureToolView,
      'password-generator': PasswordGeneratorToolView,
      'unit-converter': UnitConverterToolView,
      'time-tools': TimeToolsToolView,
      'file-hasher': FileHasherToolView,
      'inspiration': InspirationToolView,
      'work-notes': WorkNotesToolView,
      'musings': MusingsToolView
    }[tool.id])
  }));

  const filteredTools = computed(() => {
    const query = searchQuery.value.trim().toLowerCase();
    if (!query) return tools;
    return tools.filter((tool) => {
      return tool.name.toLowerCase().includes(query) || tool.description.toLowerCase().includes(query);
    });
  });

  const toolsByCategory = computed(() => {
    const query = searchQuery.value.trim().toLowerCase();
    let filtered = tools;

    if (query) {
      filtered = tools.filter((tool) => {
        return tool.name.toLowerCase().includes(query) || tool.description.toLowerCase().includes(query);
      });
    }

    const grouped = {};
    filtered.forEach(tool => {
      const category = tool.category || '其他';
      if (!grouped[category]) {
        grouped[category] = [];
      }
      grouped[category].push(tool);
    });

    return grouped;
  });

  const currentTool = computed(() => {
    if (selectedTool.value === settingsEntry.id) return settingsEntry;
    if (selectedTool.value === personalLifeGate.id) return personalLifeGate;
    return tools.find((tool) => tool.id === selectedTool.value) || tools[0];
  });

  /**
   * 加载安全设置
   */
  function loadSecuritySettings() {
    const stored = localStorage.getItem(SECURITY_SETTINGS_KEY);
    if (stored) {
      securitySettings.value = { ...securitySettings.value, ...JSON.parse(stored) };
    }
  }

  /**
   * 加载上次解锁时间
   */
  function loadLastUnlockTime() {
    const stored = localStorage.getItem(LAST_UNLOCK_TIME_KEY);
    if (stored) {
      lastUnlockTime.value = parseInt(stored);
    }
  }

  /**
   * 检查是否需要验证密码
   * @returns {boolean} 是否需要验证
   */
  function needsPasswordVerification() {
    if (!personalLifeUnlocked.value) return true;
    
    const { protectionMode, passwordTimeout } = securitySettings.value;
    
    if (protectionMode === 'every') return true;
    
    if (protectionMode === 'time') {
      const now = Date.now();
      const timeoutMs = passwordTimeout * 60 * 1000;
      return (now - lastUnlockTime.value) > timeoutMs;
    }
    
    if (protectionMode === 'inactive') {
      // 这里可以添加检测软件是否处于激活状态的逻辑
      // 暂时简化处理，返回false
      return false;
    }
    
    return false;
  }

  /**
   * 设置选中的工具
   * @param {string} toolId - 工具 ID
   */
  function setSelectedTool(toolId) {
    const tool = tools.find(t => t.id === toolId);
    if (tool && tool.category === PERSONAL_LIFE_CATEGORY) {
      if (!personalLifeUnlocked.value || needsPasswordVerification()) {
        selectedTool.value = personalLifeGate.id;
      } else {
        if (toolId !== settingsEntry.id && toolId !== personalLifeGate.id) {
          previousTool.value = toolId;
        }
        selectedTool.value = toolId;
      }
    } else {
      if (toolId === settingsEntry.id && selectedTool.value !== settingsEntry.id) {
        previousTool.value = selectedTool.value;
      } else if (toolId !== settingsEntry.id && toolId !== personalLifeGate.id) {
        previousTool.value = toolId;
      }
      selectedTool.value = toolId;
    }
  }

  function unlockPersonalLife() {
    personalLifeUnlocked.value = true;
    lastUnlockTime.value = Date.now();
    localStorage.setItem(LAST_UNLOCK_TIME_KEY, lastUnlockTime.value.toString());
    selectedTool.value = previousTool.value;
  }

  /**
   * 初始化选中的工具
   */
  function initializeSelectedTool() {
    loadSecuritySettings();
    loadLastUnlockTime();
    
    const saved = localStorage.getItem(SELECTED_TOOL_KEY);
    if (saved) {
      // 检查保存的工具是否存在
      const toolExists = tools.some(tool => tool.id === saved) || saved === settingsEntry.id;
      if (toolExists) {
        selectedTool.value = saved;
      }
    }
  }

  /**
   * 保存选中的工具
   */
  function saveSelectedTool() {
    localStorage.setItem(SELECTED_TOOL_KEY, selectedTool.value);
  }

  // 监听 selectedTool 变化并保存
  watch(selectedTool, () => {
    saveSelectedTool();
  });

  return {
    searchQuery,
    selectedTool,
    previousTool,
    tools,
    settingsEntry,
    personalLifeGate,
    personalLifeUnlocked,
    filteredTools,
    toolsByCategory,
    currentTool,
    setSelectedTool,
    initializeSelectedTool,
    unlockPersonalLife
  };
}
