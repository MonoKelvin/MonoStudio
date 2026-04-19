import { ref, watch } from 'vue';

const DEFAULT_SETTINGS = {
  theme: 'system',
  accentColor: '#3b82f6',
  sidebarWidth: 220,
  selectedTool: 'desktop-icon',
  openDevTools: false
};

let settingsCache = null;
let initPromise = null;

async function loadSettings() {
  if (settingsCache !== null) {
    return settingsCache;
  }

  if (initPromise) {
    return initPromise;
  }

  initPromise = new Promise(async (resolve) => {
    try {
      const result = await window.electronAPI.settings.get();
      settingsCache = { ...DEFAULT_SETTINGS, ...result };
    } catch (e) {
      console.error('Failed to load settings:', e);
      settingsCache = { ...DEFAULT_SETTINGS };
    }
    resolve(settingsCache);
  });

  return initPromise;
}

async function persistSettings(settings) {
  try {
    await window.electronAPI.settings.save(settings);
  } catch (e) {
    console.error('Failed to save settings:', e);
  }
}

export function useSettings() {
  const isLoaded = ref(false);
  const settings = ref({ ...DEFAULT_SETTINGS });

  async function initialize() {
    const loaded = await loadSettings();
    settings.value = { ...loaded };
    isLoaded.value = true;
  }

  function updateSetting(key, value) {
    settings.value[key] = value;
  }

  function updateAndSave(key, value) {
    updateSetting(key, value);
    persistSettings(settings.value);
  }

  // Watch for changes and auto-save
  watch(settings, (newSettings) => {
    if (isLoaded.value) {
      persistSettings(newSettings);
    }
  }, { deep: true });

  return {
    settings,
    isLoaded,
    initialize,
    updateSetting,
    updateAndSave
  };
}

export async function getInitialSettings() {
  return loadSettings();
}