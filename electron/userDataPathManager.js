const { app } = require('electron');
const path = require('path');
const fs = require('fs/promises');

const DATA_DIR_NAME = 'data';
const SECURITY_DIR_NAME = 'security';

let userDataPath = null;
let settings = null;

async function ensureDir(dirPath) {
  try {
    await fs.mkdir(dirPath, { recursive: true });
  } catch (e) {
    if (e.code !== 'EEXIST') {
      console.error('Failed to create directory:', dirPath, e);
    }
  }
}

async function fileExists(filePath) {
  try {
    await fs.access(filePath);
    return true;
  } catch {
    return false;
  }
}

async function getDefaultUserDataPath() {
  return app.getPath('userData');
}

async function loadAppSettings() {
  if (settings !== null) return settings;
  try {
    const settingsPath = path.join(await getDefaultUserDataPath(), 'settings.json');
    if (await fileExists(settingsPath)) {
      const data = await fs.readFile(settingsPath, 'utf-8');
      settings = JSON.parse(data);
    } else {
      settings = {};
    }
  } catch (e) {
    console.error('Failed to load app settings for userDataPath:', e);
    settings = {};
  }
  return settings;
}

async function saveAppSettings(newSettings) {
  try {
    const settingsPath = path.join(userDataPath || await getDefaultUserDataPath(), 'settings.json');
    await fs.writeFile(settingsPath, JSON.stringify(newSettings, null, 2), 'utf-8');
    settings = newSettings;
  } catch (e) {
    console.error('Failed to save app settings:', e);
  }
}

const pathManager = {
  async initialize(customPath = null) {
    if (customPath) {
      userDataPath = customPath;
    } else {
      userDataPath = await getDefaultUserDataPath();
    }

    await ensureDir(userDataPath);
    await ensureDir(path.join(userDataPath, DATA_DIR_NAME));
    await ensureDir(path.join(userDataPath, SECURITY_DIR_NAME));

    return userDataPath;
  },

  async getUserDataPath() {
    if (userDataPath === null) {
      await this.initialize();
    }
    return userDataPath;
  },

  async getDataDir() {
    const basePath = await this.getUserDataPath();
    return path.join(basePath, DATA_DIR_NAME);
  },

  async getSecurityDir() {
    const basePath = await this.getUserDataPath();
    return path.join(basePath, SECURITY_DIR_NAME);
  },

  async getSettingsFilePath() {
    return path.join(await this.getUserDataPath(), 'settings.json');
  },

  async getInspirationsFilePath() {
    return path.join(await this.getDataDir(), 'inspirations.json');
  },

  async getMusingsFilePath() {
    return path.join(await this.getDataDir(), 'musings.json');
  },

  async getWorkNotesFilePath() {
    return path.join(await this.getDataDir(), 'work-notes.json');
  },

  async getPasswordFilePath() {
    return path.join(await this.getSecurityDir(), 'password.json');
  },

  async getSecuritySettingsFilePath() {
    return path.join(await this.getSecurityDir(), 'security-settings.json');
  },

  async setUserDataPath(newPath) {
    const oldPath = userDataPath;
    await this.initialize(newPath);
    return { oldPath, newPath };
  },

  async getDefaultPath() {
    return getDefaultUserDataPath();
  },

  async copyFile(src, dest) {
    try {
      const srcExists = await fileExists(src);
      if (srcExists) {
        const data = await fs.readFile(src, 'utf-8');
        await ensureDir(path.dirname(dest));
        await fs.writeFile(dest, data, 'utf-8');
        return true;
      }
    } catch (e) {
      console.error('Failed to copy file:', src, '->', dest, e);
    }
    return false;
  },

  async copyDir(src, dest) {
    try {
      await ensureDir(dest);
      const entries = await fs.readdir(src, { withFileTypes: true });
      for (const entry of entries) {
        const srcPath = path.join(src, entry.name);
        const destPath = path.join(dest, entry.name);
        if (entry.isDirectory()) {
          await this.copyDir(srcPath, destPath);
        } else {
          await this.copyFile(srcPath, destPath);
        }
      }
      return true;
    } catch (e) {
      console.error('Failed to copy directory:', src, '->', dest, e);
      return false;
    }
  },

  async migrateData(oldPath, newPath) {
    const migrationResult = {
      success: true,
      migrated: [],
      failed: []
    };

    if (!oldPath || oldPath === newPath) {
      return migrationResult;
    }

    const itemsToMigrate = [
      {
        src: path.join(oldPath, 'settings.json'),
        dest: path.join(newPath, 'settings.json'),
        name: 'settings'
      },
      {
        src: path.join(oldPath, DATA_DIR_NAME),
        dest: path.join(newPath, DATA_DIR_NAME),
        isDir: true,
        name: 'data'
      },
      {
        src: path.join(oldPath, SECURITY_DIR_NAME),
        dest: path.join(newPath, SECURITY_DIR_NAME),
        isDir: true,
        name: 'security'
      }
    ];

    for (const item of itemsToMigrate) {
      try {
        const exists = await fileExists(item.src);
        if (!exists && item.isDir) {
          continue;
        }
        if (item.isDir) {
          const dirCopied = await this.copyDir(item.src, item.dest);
          if (dirCopied) {
            migrationResult.migrated.push(item.name);
          } else {
            migrationResult.failed.push(item.name);
            migrationResult.success = false;
          }
        } else {
          const fileCopied = await this.copyFile(item.src, item.dest);
          if (fileCopied) {
            migrationResult.migrated.push(item.name);
          }
        }
      } catch (e) {
        console.error('Migration failed for:', item.name, e);
        migrationResult.failed.push(item.name);
        migrationResult.success = false;
      }
    }

    return migrationResult;
  }
};

module.exports = pathManager;
