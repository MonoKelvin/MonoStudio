const { app, ipcMain, BrowserWindow, shell, globalShortcut } = require('electron');
const path = require('path');
const os = require('os');
const fs = require('fs');
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

    ipcMain.handle('system-info:get', async () => {
      try {
        // 操作系统信息
        const osInfo = {
          type: os.type(),
          version: os.version(),
          arch: os.arch(),
          platform: process.platform,
          release: os.release(),
          hostname: os.hostname(),
          uptime: Math.floor(os.uptime() / 3600) + '小时 ' + Math.floor((os.uptime() % 3600) / 60) + '分钟'
        };

        // 硬件信息
        const cpus = os.cpus();
        const totalMemory = os.totalmem();
        const freeMemory = os.freemem();
        
        // 电池信息
        let batteryInfo = '不支持';
        try {
          // Electron的powerMonitor没有getBatteryInfo方法
          // 电池信息在桌面应用中通常不可靠，因此直接设为不支持
        } catch (error) {
          console.error('获取电池信息失败:', error);
          batteryInfo = '不支持';
        }
        
        const hardwareInfo = {
          cpu: cpus[0].model,
          cpuCores: cpus.length,
          memory: `${(totalMemory / (1024 * 1024 * 1024)).toFixed(2)} GB 总内存`,
          freeMemory: `${(freeMemory / (1024 * 1024 * 1024)).toFixed(2)} GB 可用`,
          screen: '未知 (需要在渲染进程中获取)',
          battery: batteryInfo
        };

        // 网络信息
        const networkInfo = {
          localIp: getLocalIP(),
          publicIp: await getPublicIP(),
          ping: await getNetworkPing()
        };

        // 存储信息
        const storageInfo = {
          drives: getStorageInfo()
        };

        // 应用信息
        const appInfo = {
          version: app.getVersion(),
          path: app.getAppPath(),
          electronVersion: process.versions.electron,
          nodeVersion: process.versions.node
        };

        return {
          os: osInfo,
          hardware: hardwareInfo,
          network: networkInfo,
          storage: storageInfo,
          app: appInfo
        };
      } catch (error) {
        console.error('获取系统信息失败:', error);
        return {
          os: {
            type: '未知',
            version: '未知',
            arch: '未知',
            platform: '未知',
            release: '未知',
            hostname: '未知',
            uptime: '未知'
          },
          hardware: {
            cpu: '未知',
            cpuCores: '未知',
            memory: '未知',
            freeMemory: '未知',
            screen: '未知',
            battery: '未知'
          },
          network: {
            localIp: '未知',
            publicIp: '未知',
            ping: '未知'
          },
          storage: {
            drives: []
          },
          app: {
            version: '未知',
            path: '未知',
            electronVersion: '未知',
            nodeVersion: '未知'
          }
        };
      }
    });

    // 获取本地IP地址
    function getLocalIP() {
      const interfaces = os.networkInterfaces();
      for (const interfaceName in interfaces) {
        const interface = interfaces[interfaceName];
        for (const addr of interface) {
          if (addr.family === 'IPv4' && !addr.internal) {
            return addr.address;
          }
        }
      }
      return '未知';
    }

    // 获取公网IP地址
    async function getPublicIP() {
      try {
        const https = require('https');
        return new Promise((resolve) => {
          https.get('https://api.ipify.org', (res) => {
            let data = '';
            res.on('data', (chunk) => {
              data += chunk;
            });
            res.on('end', () => {
              resolve(data.trim() || '未知');
            });
          }).on('error', () => {
            resolve('未知');
          });
        });
      } catch (e) {
        console.error('获取公网IP失败:', e);
        return '未知';
      }
    }

    // 获取网络延迟
    async function getNetworkPing() {
      try {
        const { execSync } = require('child_process');
        let command;
        if (process.platform === 'win32') {
          command = 'ping -n 1 -w 1000 www.baidu.com';
        } else {
          command = 'ping -c 1 -W 1 www.baidu.com';
        }
        const output = execSync(command, { encoding: 'utf8' });
        // 简单解析ping结果，实际项目中可以使用更复杂的解析
        if (output.includes('time=')) {
          const match = output.match(/time=(\d+)ms/);
          if (match && match[1]) {
            return `${match[1]} ms`;
          }
        }
        return '未知';
      } catch (e) {
        console.error('获取网络延迟失败:', e);
        return '未知';
      }
    }

    // 获取存储信息
    function getStorageInfo() {
      const drives = [];
      
      if (process.platform === 'win32') {
        const driveLetters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'.split('');
        for (const letter of driveLetters) {
          const drive = `${letter}:`;
          try {
            const stats = fs.statSync(drive);
            if (stats.isDirectory()) {
              const { execSync } = require('child_process');
              try {
                const output = execSync(`wmic logicaldisk where "DeviceID='${drive}'" get Size,FreeSpace,VolumeName`, { encoding: 'utf8' });
                const lines = output.trim().split('\n');
                if (lines.length >= 2) {
                  const dataLine = lines[1].trim().split(/\s+/);
                  const totalSize = parseInt(dataLine[0]) || 0;
                  const freeSpace = parseInt(dataLine[1]) || 0;
                  const usedSpace = totalSize - freeSpace;
                  const percent = totalSize > 0 ? Math.round((usedSpace / totalSize) * 100) : 0;
                  const volumeName = dataLine.length >= 3 && dataLine[2] ? dataLine[2].trim() : '';
                  
                  drives.push({
                    mount: drive,
                    name: volumeName,
                    total: totalSize > 0 ? `${(totalSize / (1024 * 1024 * 1024)).toFixed(2)} GB` : '未知',
                    used: totalSize > 0 ? `${(usedSpace / (1024 * 1024 * 1024)).toFixed(2)} GB` : '未知',
                    percent: percent.toString()
                  });
                }
              } catch (e) {
                console.error(`获取${drive}磁盘信息失败:`, e);
              }
            }
          } catch (e) {
            // 驱动器不存在或无法访问
          }
        }
      } else if (process.platform === 'darwin') {
        // macOS 系统
        try {
          const { execSync } = require('child_process');
          const output = execSync('df -h /', { encoding: 'utf8' });
          const lines = output.trim().split('\n');
          if (lines.length >= 2) {
            const dataLine = lines[1].trim().split(/\s+/);
            if (dataLine.length >= 5) {
              const total = dataLine[1];
              const used = dataLine[2];
              const percent = dataLine[4].replace('%', '');
              
              drives.push({
                mount: '/',
                total: total,
                used: used,
                percent: percent
              });
            } else {
              drives.push({
                mount: '/',
                total: '未知',
                used: '未知',
                percent: '未知'
              });
            }
          } else {
            drives.push({
              mount: '/',
              total: '未知',
              used: '未知',
              percent: '未知'
            });
          }
        } catch (e) {
          console.error('获取macOS存储信息失败:', e);
          drives.push({
            mount: '/',
            total: '未知',
            used: '未知',
            percent: '未知'
          });
        }
      } else if (process.platform === 'linux') {
        // Linux 系统
        try {
          const { execSync } = require('child_process');
          const output = execSync('df -h', { encoding: 'utf8' });
          const lines = output.trim().split('\n');
          
          for (let i = 1; i < lines.length; i++) {
            const dataLine = lines[i].trim().split(/\s+/);
            if (dataLine.length >= 6) {
              const mountPoint = dataLine[5];
              // 只添加主要的挂载点
              if (mountPoint === '/' || mountPoint.startsWith('/home')) {
                const total = dataLine[1];
                const used = dataLine[2];
                const percent = dataLine[4].replace('%', '');
                
                drives.push({
                  mount: mountPoint,
                  total: total,
                  used: used,
                  percent: percent
                });
              }
            }
          }
        } catch (e) {
          console.error('获取Linux存储信息失败:', e);
          // 回退到简单实现
          try {
            const stats = fs.statSync('/');
            drives.push({
              mount: '/',
              total: '未知',
              used: '未知',
              percent: '未知'
            });
          } catch (e) {
            console.error('获取Linux根目录信息失败:', e);
          }
        }
      }
      
      // 如果没有找到任何驱动器，添加一个默认项
      if (drives.length === 0) {
        drives.push({
          mount: '未知',
          total: '未知',
          used: '未知',
          percent: '未知'
        });
      }
      
      return drives;
    }

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