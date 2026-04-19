import { ref, onMounted, onBeforeUnmount, watch } from 'vue';
import { THEME_CONFIG } from '../../config/themeConfig';

const THEME_KEY = THEME_CONFIG.THEME_KEY;
const ACCENT_COLOR_KEY = THEME_CONFIG.ACCENT_COLOR_KEY;
const DEFAULT_ACCENT_COLOR = THEME_CONFIG.DEFAULT_ACCENT_COLOR; // 宝石蓝

/**
 * 主题管理组合式 API
 * @returns {Object} 主题相关状态和方法
 */
export function useTheme() {
  const currentTheme = ref('system');
  const currentAccentColor = ref(DEFAULT_ACCENT_COLOR);
  const systemThemeMedia = ref(null);

  const themes = THEME_CONFIG.THEMES;

  /**
   * 应用主题
   */
  function applyTheme() {
    const resolvedTheme = currentTheme.value === 'system'
      ? (systemThemeMedia.value?.matches ? 'dark' : 'light')
      : currentTheme.value;
    document.documentElement.setAttribute('data-theme', resolvedTheme);
  }

  /**
   * 应用强调色
   */
  function applyAccentColor() {
    document.documentElement.style.setProperty('--accent-color', currentAccentColor.value);
  }

  /**
   * 设置主题
   * @param {string} theme - 主题名称
   */
  function setTheme(theme) {
    currentTheme.value = theme;
    localStorage.setItem(THEME_KEY, theme);
    applyTheme();
  }

  /**
   * 设置强调色
   * @param {string} color - 强调色
   */
  function setAccentColor(color) {
    currentAccentColor.value = color;
    localStorage.setItem(ACCENT_COLOR_KEY, color);
    applyAccentColor();
  }

  /**
   * 初始化主题
   */
  function initializeTheme() {
    const saved = localStorage.getItem(THEME_KEY);
    if (saved === 'light' || saved === 'dark' || saved === 'system') {
      currentTheme.value = saved;
    }
    applyTheme();
  }

  /**
   * 初始化强调色
   */
  function initializeAccentColor() {
    const saved = localStorage.getItem(ACCENT_COLOR_KEY);
    if (saved) {
      currentAccentColor.value = saved;
    }
    applyAccentColor();
  }

  /**
   * 处理系统主题变化
   */
  function handleSystemThemeChanged() {
    if (currentTheme.value === 'system') applyTheme();
  }

  onMounted(() => {
    systemThemeMedia.value = window.matchMedia('(prefers-color-scheme: dark)');
    initializeTheme();
    initializeAccentColor();
    
    if (systemThemeMedia.value.addEventListener) {
      systemThemeMedia.value.addEventListener('change', handleSystemThemeChanged);
    } else if (systemThemeMedia.value.addListener) {
      systemThemeMedia.value.addListener(handleSystemThemeChanged);
    }
  });

  onBeforeUnmount(() => {
    if (!systemThemeMedia.value) return;
    if (systemThemeMedia.value.removeEventListener) {
      systemThemeMedia.value.removeEventListener('change', handleSystemThemeChanged);
    } else if (systemThemeMedia.value.removeListener) {
      systemThemeMedia.value.removeListener(handleSystemThemeChanged);
    }
  });

  return {
    currentTheme,
    currentAccentColor,
    themes,
    setTheme,
    setAccentColor
  };
}