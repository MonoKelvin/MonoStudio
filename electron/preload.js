const { contextBridge, ipcRenderer, shell } = require('electron');

const desktopBridge = {
  listItems: () => ipcRenderer.invoke('desktop-icons:list'),
  removeItem: (targetPath) => ipcRenderer.invoke('desktop-icons:remove', targetPath),
  restoreShortcut: (targetPath) => ipcRenderer.invoke('desktop-icons:restore-shortcut', targetPath),
  pickIconFile: () => ipcRenderer.invoke('desktop-icons:pick-icon'),
  updateItemIcon: (targetPath, iconFilePath) => ipcRenderer.invoke('desktop-icons:update-icon', targetPath, iconFilePath),
  openItem: (targetPath) => ipcRenderer.invoke('desktop-icons:open-item', targetPath),
  openFolder: (targetPath) => ipcRenderer.invoke('desktop-icons:open-folder', targetPath)
};

const settingsBridge = {
  get: () => ipcRenderer.invoke('settings:get'),
  save: (settings) => ipcRenderer.invoke('settings:save', settings)
};

const devtoolsBridge = {
  toggle: (enable) => ipcRenderer.invoke('devtools:toggle', enable),
  isOpen: () => ipcRenderer.invoke('devtools:is-open'),
  onStateChanged: (callback) => {
    ipcRenderer.send('devtools:listen-state');
    ipcRenderer.on('devtools:state-changed', (_, isOpen) => callback(isOpen));
  }
};

const fileSearchBridge = {
  getDrives: () => ipcRenderer.invoke('file-search:get-drives'),
  search: (rootPath, options) => ipcRenderer.invoke('file-search:search', rootPath, options),
  cancel: (searchId) => ipcRenderer.invoke('file-search:cancel', searchId),
  openFile: (filePath) => ipcRenderer.invoke('file-search:open-file', filePath),
  openInExplorer: (filePath) => ipcRenderer.invoke('file-search:open-in-explorer', filePath),
  getInfo: (filePath) => ipcRenderer.invoke('file-search:get-info', filePath),
  onProgress: (callback) => {
    ipcRenderer.on('file-search:progress', (_, progress) => callback(progress));
  },
  removeProgressListener: () => {
    ipcRenderer.removeAllListeners('file-search:progress');
  }
};

const systemInfoBridge = {
  getSystemInfo: () => ipcRenderer.invoke('system-info:get'),
  getBattery: () => ipcRenderer.invoke('system-info:get-battery')
};

contextBridge.exposeInMainWorld('monoWindow', {
  minimize: () => ipcRenderer.send('window:minimize'),
  toggleMaximize: () => ipcRenderer.send('window:toggle-maximize'),
  close: () => ipcRenderer.send('window:close'),
  setAlwaysOnTop: (flag) => ipcRenderer.send('window:set-always-on-top', flag),
  desktop: desktopBridge
});

contextBridge.exposeInMainWorld('monoDesktop', desktopBridge);
contextBridge.exposeInMainWorld('electronAPI', {
  settings: settingsBridge,
  devtools: devtoolsBridge,
  systemInfo: systemInfoBridge,
  shell: {
    openExternal: (url) => shell.openExternal(url)
  }
});
contextBridge.exposeInMainWorld('monoFileSearch', fileSearchBridge);
