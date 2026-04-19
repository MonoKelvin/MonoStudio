const fs = require('fs/promises');
const ELECTRON_CONFIG = require('./config/electronConfig');

const SETTINGS_DIR = ELECTRON_CONFIG.SETTINGS.DIR;
const SETTINGS_FILE = ELECTRON_CONFIG.SETTINGS.FILE;

/**
 * 确保设置目录存在
 * @returns {Promise<void>}
 */
async function ensureSettingsDir() {
  try {
    await fs.mkdir(SETTINGS_DIR, { recursive: true });
  } catch (e) {
    console.error('Failed to create settings directory:', e);
  }
}

/**
 * 获取设置
 * @returns {Promise<Object>} - 设置对象
 */
async function getSettings() {
  try {
    await ensureSettingsDir();
    const data = await fs.readFile(SETTINGS_FILE, 'utf-8');
    return JSON.parse(data);
  } catch (e) {
    return {};
  }
}

/**
 * 保存设置
 * @param {Object} settings - 设置对象
 * @returns {Promise<{success: boolean, error?: string}>} - 操作结果
 */
async function saveSettings(settings) {
  try {
    await ensureSettingsDir();
    await fs.writeFile(SETTINGS_FILE, JSON.stringify(settings, null, 2), 'utf-8');
    return { success: true };
  } catch (e) {
    console.error('Failed to save settings:', e);
    return { success: false, error: e.message };
  }
}

module.exports = {
  getSettings,
  saveSettings
};