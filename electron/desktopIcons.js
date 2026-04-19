const { app, shell, dialog } = require('electron');
const path = require('path');
const fs = require('fs/promises');
const { exec } = require('child_process');
const { promisify } = require('util');
const { exists, readJsonSafe, writeJsonSafe, getUserDesktopPath, toItemKind, supportsIconModify } = require('./utils');
const electronConfig = require('./config/electronConfig');

const desktopHiddenRecordFile = electronConfig.DESKTOP.HIDDEN_RECORD_FILE;
const execAsync = promisify(exec);

const SYSTEM_ITEMS = electronConfig.SYSTEM_ITEMS;

/**
 * 读取注册表值
 * @param {string} regPath - 注册表路径
 * @param {string} valueName - 值名称
 * @returns {Promise<{type: string, data: string}|null>} - 注册表值
 */
async function readRegistryValue(regPath, valueName = '') {
  const valueArg = valueName ? `/v "${valueName}"` : '/ve';
  try {
    const { stdout } = await execAsync(`reg query "${regPath}" ${valueArg}`);
    const lines = stdout.split(/\r?\n/).map((line) => line.trim()).filter(Boolean);
    const matched = lines.find((line) => line.includes('REG_'));
    if (!matched) return null;
    const parts = matched.split(/\s{2,}/).filter(Boolean);
    if (parts.length < 3) return null;
    return { type: parts[1], data: parts.slice(2).join(' ') };
  } catch {
    return null;
  }
}

/**
 * 读取注册表 DWORD 值
 * @param {string} regPath - 注册表路径
 * @param {string} valueName - 值名称
 * @returns {Promise<number|null>} - DWORD 值
 */
async function getRegistryDword(regPath, valueName) {
  const value = await readRegistryValue(regPath, valueName);
  if (!value || value.type !== 'REG_DWORD') return null;
  const parsed = Number.parseInt(value.data, 16);
  return Number.isNaN(parsed) ? null : parsed;
}

/**
 * 检查系统桌面项是否可见
 * @param {string} clsid - CLSID
 * @returns {Promise<boolean>} - 是否可见
 */
async function isSystemDesktopItemVisible(clsid) {
  if (process.platform !== 'win32') return false;
  const newStartPath = 'HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel';
  const classicPath = 'HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\ClassicStartMenu';
  const [newStartValue, classicValue] = await Promise.all([
    getRegistryDword(newStartPath, clsid),
    getRegistryDword(classicPath, clsid)
  ]);
  return newStartValue !== 1 && classicValue !== 1;
}

/**
 * 加载系统桌面项
 * @returns {Promise<Array>} - 系统桌面项列表
 */
async function loadSystemDesktopEntries() {
  const items = [];
  for (const item of SYSTEM_ITEMS) {
    const visible = await isSystemDesktopItemVisible(item.clsid);
    if (!visible) continue;
    items.push({
      id: item.id,
      name: item.name,
      path: item.openTarget,
      kind: item.kind,
      systemKey: item.systemKey,
      clsid: item.clsid,
      supportsIconModify: true,
      iconDataUrl: '',
      action: 'none'
    });
  }
  return items;
}

/**
 * 获取记录文件路径
 * @returns {string} - 记录文件路径
 */
function getRecordFilePath() {
  return path.join(electronConfig.APP_DATA_DIR, desktopHiddenRecordFile);
}

/**
 * 加载桌面项
 * @returns {Promise<Array>} - 桌面项列表
 */
async function loadDesktopEntries() {
  const desktopPath = getUserDesktopPath();
  const entries = await fs.readdir(desktopPath, { withFileTypes: true });
  const output = [];

  for (const entry of entries) {
    if (entry.name.toLowerCase() === 'desktop.ini') continue;
    const fullPath = path.join(desktopPath, entry.name);
    let targetPath = '';
    let kind = 'file';

    try {
      const stat = await fs.stat(fullPath);
      if (stat.isDirectory()) {
        kind = 'folder';
      } else if (path.extname(fullPath).toLowerCase() === '.lnk') {
        kind = 'shortcut';
        targetPath = fullPath;
      }
    } catch {
      continue;
    }

    output.push({
      id: fullPath,
      name: entry.name,
      path: fullPath,
      targetPath,
      kind,
      supportsIconModify: kind === 'folder' || kind === 'shortcut',
      iconDataUrl: ''
    });
  }

  return output;
}

/**
 * 获取桌面项列表
 * @returns {Promise<Array>} - 桌面项列表
 */
async function getDesktopItems() {
  const recordFile = getRecordFilePath();
  const hiddenShortcuts = await readJsonSafe(recordFile, {});
  const desktopEntries = await loadDesktopEntries();
  const systemEntries = await loadSystemDesktopEntries();
  const result = [];
  const existingPaths = new Set();

  for (const item of desktopEntries) {
    existingPaths.add(item.path);
    if (item.kind === 'shortcut' && hiddenShortcuts[item.path]) {
      continue;
    }
    result.push({
      ...item,
      action: 'remove'
    });
  }

  for (const [shortcutPath, metadata] of Object.entries(hiddenShortcuts)) {
    if (existingPaths.has(shortcutPath)) continue;
    result.push({
      id: shortcutPath,
      name: path.basename(shortcutPath),
      path: shortcutPath,
      kind: 'shortcut',
      supportsIconModify: true,
      iconDataUrl: '',
      action: 'add',
      removedAt: metadata.removedAt || null
    });
  }

  result.push(...systemEntries);

  result.sort((a, b) => a.name.localeCompare(b.name, 'zh-CN'));
  return result;
}

/**
 * 移除桌面项
 * @param {string} targetPath - 目标路径
 * @returns {Promise<{success: boolean}>} - 操作结果
 */
async function removeDesktopItem(targetPath) {
  const desktopPath = getUserDesktopPath();
  const normalizedDesktop = path.resolve(desktopPath).toLowerCase();
  const resolvedTarget = path.resolve(targetPath);
  if (!resolvedTarget.toLowerCase().startsWith(normalizedDesktop)) {
    throw new Error('仅允许操作桌面目录内的项目');
  }

  const stat = await fs.stat(resolvedTarget);
  const isShortcut = path.extname(resolvedTarget).toLowerCase() === '.lnk' && stat.isFile();
  const recordFile = getRecordFilePath();
  const hiddenShortcuts = await readJsonSafe(recordFile, {});

  if (isShortcut) {
    const shortcutDetails = shell.readShortcutLink(resolvedTarget);
    hiddenShortcuts[resolvedTarget] = {
      removedAt: Date.now(),
      shortcutDetails
    };
    await writeJsonSafe(recordFile, hiddenShortcuts);
  }

  if (stat.isDirectory()) {
    await fs.rm(resolvedTarget, { recursive: true, force: false });
  } else {
    await fs.unlink(resolvedTarget);
  }

  return { success: true };
}

/**
 * 选择图标文件
 * @param {BrowserWindow} parentWindow - 父窗口
 * @returns {Promise<string|null>} - 选中的图标文件路径
 */
async function chooseIconFile(parentWindow) {
  const result = await dialog.showOpenDialog(parentWindow, {
    title: '选择图标文件',
    properties: ['openFile'],
    filters: [
      { name: '图标文件', extensions: ['ico', 'exe', 'dll', 'png', 'jpg', 'jpeg'] },
      { name: '所有文件', extensions: ['*'] }
    ]
  });
  if (result.canceled || !result.filePaths.length) return null;
  return result.filePaths[0];
}

/**
 * 应用文件夹图标
 * @param {string} folderPath - 文件夹路径
 * @param {string} iconFilePath - 图标文件路径
 * @returns {Promise<void>}
 */
async function applyFolderIcon(folderPath, iconFilePath) {
  if (process.platform !== 'win32') {
    throw new Error('当前仅支持 Windows 文件夹图标替换');
  }
  const stat = await fs.stat(folderPath);
  if (!stat.isDirectory()) {
    throw new Error('仅文件夹支持该操作');
  }

  const iconName = `.monostudio-folder-icon${path.extname(iconFilePath) || '.ico'}`;
  const localIconPath = path.join(folderPath, iconName);
  await fs.copyFile(iconFilePath, localIconPath);

  const escapedFolder = `"${folderPath}"`;
  const escapedIcon = `"${localIconPath}"`;
  await execAsync(`attrib +s ${escapedFolder}`);
  await execAsync(`attrib +h ${escapedIcon}`);
}

/**
 * 更新桌面项图标
 * @param {string} targetPath - 目标路径
 * @param {string} iconFilePath - 图标文件路径
 * @returns {Promise<{success: boolean}>} - 操作结果
 */
async function updateDesktopItemIcon(targetPath, iconFilePath) {
  if (targetPath.startsWith('system:')) {
    const systemItem = SYSTEM_ITEMS.find((item) => item.id === targetPath);
    if (!systemItem) throw new Error('未知系统图标项');
    const regPath = `HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\CLSID\\${systemItem.clsid}\\DefaultIcon`;
    const iconValue = `"${iconFilePath}",0`;
    await execAsync(`reg add "${regPath}" /ve /t REG_SZ /d "${iconValue}" /f`);
    if (systemItem.systemKey === 'recycle-bin') {
      await execAsync(`reg add "${regPath}" /v empty /t REG_SZ /d "${iconValue}" /f`);
      await execAsync(`reg add "${regPath}" /v full /t REG_SZ /d "${iconValue}" /f`);
    }
    return { success: true };
  }

  const stat = await fs.stat(targetPath);
  const isShortcut = stat.isFile() && path.extname(targetPath).toLowerCase() === '.lnk';
  const isFolder = stat.isDirectory();

  if (!isShortcut && !isFolder) {
    throw new Error('仅快捷方式和文件夹支持修改图标');
  }

  if (isShortcut) {
    const current = shell.readShortcutLink(targetPath);
    const ok = shell.writeShortcutLink(targetPath, 'update', {
      ...current,
      icon: iconFilePath,
      iconIndex: 0
    });
    if (!ok) throw new Error('快捷方式图标更新失败');
    return { success: true };
  }

  await applyFolderIcon(targetPath, iconFilePath);
  return { success: true };
}

/**
 * 打开桌面项
 * @param {string} targetPath - 目标路径
 * @returns {Promise<{success: boolean}>} - 操作结果
 */
async function openDesktopItem(targetPath) {
  const systemItem = SYSTEM_ITEMS.find((item) => item.id === targetPath);
  if (systemItem) {
    const result = await shell.openPath(systemItem.openTarget);
    if (result) throw new Error(result);
    return { success: true };
  }

  const itemExists = await exists(targetPath);
  if (!itemExists) {
    throw new Error('目标不存在，无法打开');
  }

  const stat = await fs.stat(targetPath);
  const isShortcut = stat.isFile() && path.extname(targetPath).toLowerCase() === '.lnk';
  let openPath = targetPath;

  if (isShortcut) {
    const shortcut = shell.readShortcutLink(targetPath);
    if (!shortcut.target) {
      throw new Error('快捷方式无有效目标');
    }
    openPath = shortcut.target;
  }

  const result = await shell.openPath(openPath);
  if (result) {
    throw new Error(result);
  }
  return { success: true };
}

/**
 * 恢复快捷方式
 * @param {string} targetPath - 目标路径
 * @returns {Promise<{success: boolean}>} - 操作结果
 */
async function restoreShortcut(targetPath) {
  const recordFile = getRecordFilePath();
  const hiddenShortcuts = await readJsonSafe(recordFile, {});
  const record = hiddenShortcuts[targetPath];
  if (!record || !record.shortcutDetails) {
    throw new Error('未找到可恢复的快捷方式记录');
  }

  const targetExists = await exists(record.shortcutDetails.target);
  if (!targetExists) {
    throw new Error('快捷方式目标不存在，无法恢复');
  }

  const ok = shell.writeShortcutLink(targetPath, 'create', record.shortcutDetails);
  if (!ok) {
    throw new Error('恢复快捷方式失败');
  }

  delete hiddenShortcuts[targetPath];
  await writeJsonSafe(recordFile, hiddenShortcuts);
  return { success: true };
}

module.exports = {
  getDesktopItems,
  removeDesktopItem,
  restoreShortcut,
  chooseIconFile,
  updateDesktopItemIcon,
  openDesktopItem
};