const path = require('path');
const fs = require('fs/promises');

/**
 * 检查文件是否存在
 * @param {string} filePath - 文件路径
 * @returns {Promise<boolean>} - 文件是否存在
 */
async function exists(filePath) {
  try {
    await fs.access(filePath);
    return true;
  } catch {
    return false;
  }
}

/**
 * 安全读取 JSON 文件
 * @param {string} filePath - 文件路径
 * @param {any} fallbackValue - 失败时的默认值
 * @returns {Promise<any>} - 读取的值或默认值
 */
async function readJsonSafe(filePath, fallbackValue) {
  try {
    const content = await fs.readFile(filePath, 'utf-8');
    return JSON.parse(content);
  } catch {
    return fallbackValue;
  }
}

/**
 * 安全写入 JSON 文件
 * @param {string} filePath - 文件路径
 * @param {any} value - 要写入的值
 * @returns {Promise<void>}
 */
async function writeJsonSafe(filePath, value) {
  await fs.mkdir(path.dirname(filePath), { recursive: true });
  await fs.writeFile(filePath, JSON.stringify(value, null, 2), 'utf-8');
}

/**
 * 获取桌面路径
 * @returns {string} - 桌面路径
 */
function getUserDesktopPath() {
  return path.join(process.env.USERPROFILE || process.env.HOME, 'Desktop');
}

/**
 * 根据文件类型获取项目类型
 * @param {fs.Dirent} dirent - 文件目录项
 * @param {string} fullPath - 完整路径
 * @returns {string} - 项目类型
 */
function toItemKind(dirent, fullPath) {
  if (dirent.isDirectory()) return 'folder';
  if (path.extname(fullPath).toLowerCase() === '.lnk') return 'shortcut';
  return 'file';
}

/**
 * 检查项目是否支持修改图标
 * @param {string} kind - 项目类型
 * @returns {boolean} - 是否支持修改图标
 */
function supportsIconModify(kind) {
  return kind === 'folder' || kind === 'shortcut';
}

module.exports = {
  exists,
  readJsonSafe,
  writeJsonSafe,
  getUserDesktopPath,
  toItemKind,
  supportsIconModify
};