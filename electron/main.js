const { app, ipcMain, BrowserWindow, shell, globalShortcut } = require('electron');
const path = require('path');
const desktopIcons = require('./desktopIcons');
const { getDesktopItems, removeDesktopItem, restoreShortcut, chooseIconFile, updateDesktopItemIcon, openDesktopItem } = desktopIcons;
const { getSettings, saveSettings } = require('./settings');
const { createWindow } = require('./windowManager');
const fileSearch = require('./fileSearch');

// 存储搜索状态
let activeSearchIds = new Set();

ipcMain.on('window:minimize', (event) => {
  const win = BrowserWindow.fromWebContents(event.sender);
  if (win) win.minimize();
});

ipcMain.on('window:toggle-maximize', (event) => {
  const win = BrowserWindow.fromWebContents(event.sender);
  if (!win) return;
  if (win.isMaximized()) {
    win.unmaximize();
  } else {
    win.maximize();
  }
});

ipcMain.on('window:close', (event) => {
  const win = BrowserWindow.fromWebContents(event.sender);
  if (win) win.close();
});

ipcMain.on('window:set-always-on-top', (event, flag) => {
  const win = BrowserWindow.fromWebContents(event.sender);
  if (win) win.setAlwaysOnTop(flag);
});

app.whenReady().then(async () => {
  try {
    ipcMain.handle('desktop-icons:list', async () => getDesktopItems());
    ipcMain.handle('desktop-icons:remove', async (_, targetPath) => removeDesktopItem(targetPath));
    ipcMain.handle('desktop-icons:restore-shortcut', async (_, targetPath) => restoreShortcut(targetPath));
    ipcMain.handle('desktop-icons:pick-icon', async (event) => {
      const parentWindow = BrowserWindow.fromWebContents(event.sender);
      return chooseIconFile(parentWindow || undefined);
    });
    ipcMain.handle('desktop-icons:update-icon', async (_, targetPath, iconFilePath) => updateDesktopItemIcon(targetPath, iconFilePath));
    ipcMain.handle('desktop-icons:open-item', async (_, targetPath) => openDesktopItem(targetPath));
    ipcMain.handle('desktop-icons:open-folder', async (_, targetPath) => {
      const folderPath = path.dirname(targetPath);
      await shell.openPath(folderPath);
      return { success: true };
    });

    ipcMain.handle('file-search:get-drives', async () => {
      return fileSearch.getAvailableDrives();
    });

    ipcMain.handle('file-search:search', async (event, rootPath, options) => {
      const searchResult = await fileSearch.searchDirectory(rootPath, options, (progress) => {
        event.sender.send('file-search:progress', progress);
      });
      activeSearchIds.add(searchResult.searchId);
      return searchResult;
    });

    ipcMain.handle('file-search:cancel', async (_, searchId) => {
      fileSearch.cancelSearch(searchId);
      activeSearchIds.delete(searchId);
      return { success: true };
    });

    ipcMain.handle('file-search:open-file', async (_, filePath) => {
      return fileSearch.openFile(filePath);
    });

    ipcMain.handle('file-search:open-in-explorer', async (_, filePath) => {
      return fileSearch.openInExplorer(filePath);
    });

    ipcMain.handle('file-search:get-info', async (_, filePath) => {
      return fileSearch.getFileInfo(filePath);
    });

    ipcMain.handle('settings:get', async () => getSettings());
    ipcMain.handle('settings:save', async (_, settings) => saveSettings(settings));

    ipcMain.handle('devtools:toggle', async (_, enable) => {
      const mainWindow = BrowserWindow.getFocusedWindow();
      if (mainWindow) {
        if (enable) {
          mainWindow.webContents.openDevTools();
        } else {
          mainWindow.webContents.closeDevTools();
        }
      }
    });

    ipcMain.handle('devtools:is-open', async () => {
      const mainWindow = BrowserWindow.getAllWindows()[0];
      if (mainWindow) {
        return mainWindow.webContents.isDevToolsOpened();
      }
      return false;
    });

    ipcMain.on('devtools:listen-state', (event) => {
      const mainWindow = BrowserWindow.getAllWindows()[0];
      if (mainWindow) {
        mainWindow.webContents.on('devtools-opened', () => {
          event.sender.send('devtools:state-changed', true);
        });
        mainWindow.webContents.on('devtools-closed', () => {
          event.sender.send('devtools:state-changed', false);
        });
      }
    });

    await createWindow();

    // 注册全局快捷键 Ctrl+Shift+I 打开/关闭开发者工具
    const mainWindow = BrowserWindow.getAllWindows()[0];
    if (mainWindow) {
      globalShortcut.register('Ctrl+Shift+I', () => {
        const isDevToolsOpen = mainWindow.webContents.isDevToolsOpened();
        if (isDevToolsOpen) {
          mainWindow.webContents.closeDevTools();
        } else {
          mainWindow.webContents.openDevTools();
        }
      });
    }
  } catch (e) {
    console.error('[main] error:', e);
  }

  app.on('activate', async function () {
    if (BrowserWindow.getAllWindows().length === 0) await createWindow();
  });
});

app.on('window-all-closed', function () {
  // 清理所有搜索
  activeSearchIds.forEach(searchId => {
    fileSearch.cancelSearch(searchId);
  });
  activeSearchIds.clear();
  
  // 注销全局快捷键
  globalShortcut.unregisterAll();
  
  if (process.platform !== 'darwin') app.quit();
});