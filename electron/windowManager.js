const { app, BrowserWindow, Menu } = require('electron');
const path = require('path');
const fs = require('fs/promises');
const { getSettings } = require('./settings');
const ELECTRON_CONFIG = require('./config/electronConfig');

const WINDOW_STATE_FILE = path.join(app.getPath('userData'), ELECTRON_CONFIG.WINDOW_STATE.FILE);

/**
 * 获取窗口状态
 * @returns {Promise<Object|null>} - 窗口状态
 */
async function getWindowState() {
  try {
    const data = await fs.readFile(WINDOW_STATE_FILE, 'utf-8');
    return JSON.parse(data);
  } catch (e) {
    return null;
  }
}

/**
 * 保存窗口状态
 * @param {BrowserWindow} window - 窗口实例
 * @returns {Promise<void>}
 */
async function saveWindowState(window) {
  try {
    const state = {
      x: window.getPosition()[0],
      y: window.getPosition()[1],
      width: window.getSize()[0],
      height: window.getSize()[1]
    };
    await fs.writeFile(WINDOW_STATE_FILE, JSON.stringify(state, null, 2), 'utf-8');
  } catch (e) {
    console.error('Failed to save window state:', e);
  }
}

/**
 * 创建主窗口
 * @returns {Promise<BrowserWindow>} - 主窗口实例
 */
async function createWindow() {
  let windowState = null;
  try {
    windowState = await getWindowState();
  } catch (e) {
    console.error('Failed to get window state:', e);
  }
  
  const mainWindow = new BrowserWindow({
    x: windowState?.x,
    y: windowState?.y,
    width: windowState?.width || 1000,
    height: windowState?.height || 600,
    minWidth: 800,
    minHeight: 500,
    show: false,
    frame: false,
    roundedCorners: false,
    thickFrame: true,
    transparent: false,
    backgroundColor: '#0b0d10',
    autoHideMenuBar: true,
    titleBarStyle: 'hidden',
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      nodeIntegration: false,
      contextIsolation: true
    }
  });
  
  // 监听窗口移动和调整大小事件，保存窗口状态
  mainWindow.on('move', () => saveWindowState(mainWindow));
  mainWindow.on('resize', () => saveWindowState(mainWindow));
  Menu.setApplicationMenu(null);

  const devServerUrl = process.env.VITE_DEV_SERVER_URL || (!app.isPackaged ? 'http://localhost:5173' : '');
  if (devServerUrl) {
    mainWindow.loadURL(devServerUrl);
    const settings = await getSettings();
    if (settings.openDevTools) {
      mainWindow.webContents.openDevTools();
    }
  } else {
    mainWindow.loadFile(path.join(__dirname, '../dist/index.html'));
  }

  mainWindow.once('ready-to-show', () => {
    mainWindow.show();
  });

  mainWindow.webContents.on('did-fail-load', (_, errorCode, errorDescription, validatedURL) => {
    console.error('[renderer-load-failed]', { errorCode, errorDescription, validatedURL });
  });

  mainWindow.webContents.on('render-process-gone', (_, details) => {
    console.error('[renderer-process-gone]', details);
  });

  mainWindow.webContents.on('console-message', (_, level, message, line, sourceId) => {
    const levelMap = { 0: 'info', 1: 'warn', 2: 'error', 3: 'debug' };
    const tag = levelMap[level] || 'log';
    console[tag](`[renderer-console:${tag}] ${message} (${sourceId}:${line})`);
  });

  return mainWindow;
}

module.exports = {
  createWindow
};