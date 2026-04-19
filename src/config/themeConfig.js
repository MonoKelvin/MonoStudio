/**
 * 主题配置
 */
export const THEME_CONFIG = {
  // 主题存储键
  THEME_KEY: 'mono-theme-mode',
  ACCENT_COLOR_KEY: 'mono-accent-color',
  // 默认强调色
  DEFAULT_ACCENT_COLOR: '#3b82f6', // 宝石蓝
  // 主题选项
  THEMES: [
    { value: 'light', label: '浅色' },
    { value: 'dark', label: '深色' },
    { value: 'system', label: '跟随系统' }
  ]
};