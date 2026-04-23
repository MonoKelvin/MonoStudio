const fs = require('fs/promises');
const pathManager = require('./userDataPathManager');

async function fileExists(filePath) {
  try {
    await fs.access(filePath);
    return true;
  } catch {
    return false;
  }
}

const userDataStorage = {
  async getInspirations() {
    try {
      const filePath = await pathManager.getInspirationsFilePath();
      if (await fileExists(filePath)) {
        const data = await fs.readFile(filePath, 'utf-8');
        return JSON.parse(data);
      }
      return [];
    } catch (e) {
      console.error('Failed to load inspirations:', e);
      return [];
    }
  },

  async saveInspirations(inspirations) {
    try {
      const filePath = await pathManager.getInspirationsFilePath();
      await fs.writeFile(filePath, JSON.stringify(inspirations, null, 2), 'utf-8');
      return { success: true };
    } catch (e) {
      console.error('Failed to save inspirations:', e);
      return { success: false, error: e.message };
    }
  },

  async getMusings() {
    try {
      const filePath = await pathManager.getMusingsFilePath();
      if (await fileExists(filePath)) {
        const data = await fs.readFile(filePath, 'utf-8');
        return JSON.parse(data);
      }
      return [];
    } catch (e) {
      console.error('Failed to load musings:', e);
      return [];
    }
  },

  async saveMusings(musings) {
    try {
      const filePath = await pathManager.getMusingsFilePath();
      await fs.writeFile(filePath, JSON.stringify(musings, null, 2), 'utf-8');
      return { success: true };
    } catch (e) {
      console.error('Failed to save musings:', e);
      return { success: false, error: e.message };
    }
  },

  async getWorkNotes() {
    try {
      const filePath = await pathManager.getWorkNotesFilePath();
      if (await fileExists(filePath)) {
        const data = await fs.readFile(filePath, 'utf-8');
        return JSON.parse(data);
      }
      return [];
    } catch (e) {
      console.error('Failed to load work notes:', e);
      return [];
    }
  },

  async saveWorkNotes(notes) {
    try {
      const filePath = await pathManager.getWorkNotesFilePath();
      await fs.writeFile(filePath, JSON.stringify(notes, null, 2), 'utf-8');
      return { success: true };
    } catch (e) {
      console.error('Failed to save work notes:', e);
      return { success: false, error: e.message };
    }
  },

  async getPassword() {
    try {
      const filePath = await pathManager.getPasswordFilePath();
      if (await fileExists(filePath)) {
        const data = await fs.readFile(filePath, 'utf-8');
        return JSON.parse(data);
      }
      return null;
    } catch (e) {
      console.error('Failed to load password:', e);
      return null;
    }
  },

  async savePassword(passwordData) {
    try {
      const filePath = await pathManager.getPasswordFilePath();
      await fs.writeFile(filePath, JSON.stringify(passwordData, null, 2), 'utf-8');
      return { success: true };
    } catch (e) {
      console.error('Failed to save password:', e);
      return { success: false, error: e.message };
    }
  },

  async getSecuritySettings() {
    try {
      const filePath = await pathManager.getSecuritySettingsFilePath();
      if (await fileExists(filePath)) {
        const data = await fs.readFile(filePath, 'utf-8');
        return JSON.parse(data);
      }
      return null;
    } catch (e) {
      console.error('Failed to load security settings:', e);
      return null;
    }
  },

  async saveSecuritySettings(settings) {
    try {
      const filePath = await pathManager.getSecuritySettingsFilePath();
      await fs.writeFile(filePath, JSON.stringify(settings, null, 2), 'utf-8');
      return { success: true };
    } catch (e) {
      console.error('Failed to save security settings:', e);
      return { success: false, error: e.message };
    }
  },

  async getUserDataPath() {
    return pathManager.getUserDataPath();
  },

  async getDefaultUserDataPath() {
    return pathManager.getDefaultPath();
  },

  async setUserDataPath(newPath) {
    const oldPath = await this.getUserDataPath();
    const result = await pathManager.setUserDataPath(newPath);
    const migrationResult = await pathManager.migrateData(oldPath, newPath);
    return {
      oldPath,
      newPath,
      migration: migrationResult
    };
  }
};

module.exports = userDataStorage;