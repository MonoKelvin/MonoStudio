const { app } = require('electron');
const path = require('path');

/**
 * Electron 配置
 */
// 先定义应用数据目录
const APP_DATA_DIR = app.getPath('userData');

const ELECTRON_CONFIG = {
  // 应用数据目录
  APP_DATA_DIR: APP_DATA_DIR,
  // 设置目录和文件
  SETTINGS: {
    DIR: APP_DATA_DIR,
    FILE: path.join(APP_DATA_DIR, 'settings.json')
  },
  // 窗口状态文件
  WINDOW_STATE: {
    FILE: 'window-state.json'
  },
  // 桌面快捷方式隐藏记录文件
  DESKTOP: {
    HIDDEN_RECORD_FILE: 'desktop-shortcut-hidden.json'
  },
  // 系统项配置
  SYSTEM_ITEMS: [
    {
      id: 'system:this-pc',
      name: '此电脑',
      kind: 'system',
      systemKey: 'this-pc',
      clsid: '{20D04FE0-3AEA-1069-A2D8-08002B30309D}',
      openTarget: 'shell:MyComputerFolder'
    },
    {
      id: 'system:recycle-bin',
      name: '回收站',
      kind: 'system',
      systemKey: 'recycle-bin',
      clsid: '{645FF040-5081-101B-9F08-00AA002F954E}',
      openTarget: 'shell:RecycleBinFolder'
    }
  ]
};

module.exports = ELECTRON_CONFIG;
module.exports.APP_DATA_DIR = APP_DATA_DIR;