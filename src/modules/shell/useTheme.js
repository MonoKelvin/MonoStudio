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
    
    // 主题变化时重新计算强调色
    applyAccentColor();
  }

  /**
   * 应用强调色
   */
  function applyAccentColor() {
    document.documentElement.style.setProperty('--accent-color', currentAccentColor.value);
    
    // 计算并应用 hover 和 active 颜色
    const resolvedTheme = currentTheme.value === 'system'
      ? (systemThemeMedia.value?.matches ? 'dark' : 'light')
      : currentTheme.value;
    
    const accentColor = currentAccentColor.value;
    const hoverColor = getHoverColor(accentColor, resolvedTheme === 'dark');
    const activeColor = getActiveColor(accentColor);
    
    document.documentElement.style.setProperty('--accent-hover', hoverColor);
    document.documentElement.style.setProperty('--accent-active', activeColor);
  }

  /**
   * 获取 hover 颜色
   * @param {string} color - 主色调
   * @param {boolean} isDark - 是否为深色主题
   * @returns {string} hover 颜色
   */
  function getHoverColor(color, isDark) {
    if (isDark) {
      // 深色主题：hover 颜色亮一点
      return lightenColor(color, 10);
    } else {
      // 浅色主题：hover 颜色暗一点
      return darkenColor(color, 10);
    }
  }

  /**
   * 获取 active 颜色
   * @param {string} color - 主色调
   * @returns {string} active 颜色
   */
  function getActiveColor(color) {
    // active 颜色总是比主色暗一点
    return darkenColor(color, 15);
  }

  /**
   * 使颜色变亮
   * @param {string} color - 十六进制颜色
   * @param {number} percent - 百分比
   * @returns {string} 变亮后的颜色
   */
  function lightenColor(color, percent) {
    const num = parseInt(color.replace("#", ""), 16);
    const amt = Math.round(2.55 * percent);
    const R = (num >> 16) + amt;
    const G = (num >> 8 & 0x00FF) + amt;
    const B = (num & 0x0000FF) + amt;
    return "#" + (0x1000000 + (R < 255 ? R < 1 ? 0 : R : 255) * 0x10000 +
      (G < 255 ? G < 1 ? 0 : G : 255) * 0x100 +
      (B < 255 ? B < 1 ? 0 : B : 255)).toString(16).slice(1);
  }

  /**
   * 使颜色变暗
   * @param {string} color - 十六进制颜色
   * @param {number} percent - 百分比
   * @returns {string} 变暗后的颜色
   */
  function darkenColor(color, percent) {
    const num = parseInt(color.replace("#", ""), 16);
    const amt = Math.round(2.55 * percent);
    const R = (num >> 16) - amt;
    const G = (num >> 8 & 0x00FF) - amt;
    const B = (num & 0x0000FF) - amt;
    return "#" + (0x1000000 + (R > 255 ? 255 : R < 0 ? 0 : R) * 0x10000 +
      (G > 255 ? 255 : G < 0 ? 0 : G) * 0x100 +
      (B > 255 ? 255 : B < 0 ? 0 : B)).toString(16).slice(1);
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