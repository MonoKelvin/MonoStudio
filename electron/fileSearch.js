const fs = require('fs');
const path = require('path');
const { exec } = require('child_process');
const { app } = require('electron');

const BATCH_SIZE = 100;
const CONCURRENT_DIRS = 10;

let searchIdCounter = 0;
let activeSearches = new Map();

async function getAvailableDrives() {
  return new Promise((resolve) => {
    const drives = [];

    if (process.platform === 'win32') {
      exec('wmic logicaldisk get name,size,freespace,volumename,drivetype /format:csv', { encoding: 'utf8' }, (error, stdout) => {
        if (error) {
          exec('powershell -Command "Get-PSDrive -PSProvider FileSystem | Select-Object Name, Used, Free | ConvertTo-Json"', { encoding: 'utf8' }, (err, out) => {
            if (!err && out) {
              try {
                const psDrives = JSON.parse(out);
                const driveList = Array.isArray(psDrives) ? psDrives : [psDrives];
                for (const d of driveList) {
                  if (d.Name && d.Free !== undefined) {
                    drives.push({
                      name: d.Name + '\\',
                      label: d.Name + '\\',
                      freeSpace: d.Free,
                      totalSpace: d.Free + (d.Used || 0)
                    });
                  }
                }
              } catch (e) {
                // ignore parse error
              }
            }
            if (drives.length === 0) {
              // 只添加 C: 盘作为默认值，避免显示不存在的磁盘
              drives.push({ name: 'C:\\', label: 'C:\\ (系统)' });
            }
            resolve(drives);
          });
          return;
        }

        const lines = stdout.trim().split('\n').filter(line => line.trim() && !line.startsWith('Node'));
        for (const line of lines) {
          const parts = line.split(',');
          if (parts.length >= 2) {
            const driveName = parts[1] ? parts[1].trim() : null;
            if (driveName && driveName.match(/^[A-Z]:$/i)) {
              const driveLetter = driveName + '\\';
              let freeSpace = 0;
              let totalSpace = 0;
              let volumeName = '';

              if (parts[2]) freeSpace = parseInt(parts[2]) || 0;
              if (parts[3]) totalSpace = parseInt(parts[3]) || 0;
              if (parts[4]) volumeName = parts[4].trim();

              drives.push({
                name: driveLetter,
                label: volumeName ? `${driveLetter} (${volumeName})` : driveLetter,
                freeSpace,
                totalSpace
              });
            }
          }
        }

        if (drives.length === 0) {
          // 只添加 C: 盘作为默认值，避免显示不存在的磁盘
          drives.push({ name: 'C:\\', label: 'C:\\ (系统)' });
        }

        resolve(drives);
      });
    } else {
      drives.push({ name: '/', label: '/' });
      resolve(drives);
    }
  });
}

function cancelSearch(searchId) {
  const search = activeSearches.get(searchId);
  if (search) {
    search.isCancelled = true;
  }
}

async function searchDirectory(rootPath, options, progressCallback) {
  const {
    keyword,
    extension,
    maxResults = 1000,
    caseSensitive = false,
    recursive = true
  } = options;

  const searchId = ++searchIdCounter;
  const results = [];
  const normalizedKeyword = caseSensitive ? keyword : keyword.toLowerCase();
  
  // 存储搜索状态
  const searchState = {
    searchId,
    isCancelled: false,
    totalFilesScanned: 0,
    totalDirsScanned: 0,
    pendingDirs: [rootPath],
    processedDirs: 0
  };
  
  activeSearches.set(searchId, searchState);

  function matchesCriteria(fileName) {
    const nameToCheck = caseSensitive ? fileName : fileName.toLowerCase();

    if (extension) {
      const ext = extension.startsWith('.') ? extension.toLowerCase() : '.' + extension.toLowerCase();
      if (!nameToCheck.endsWith(ext)) {
        return false;
      }
    }

    if (normalizedKeyword) {
      const baseName = path.basename(fileName, path.extname(fileName));
      const searchTargets = [nameToCheck, baseName.toLowerCase()];
      if (!searchTargets.some(target => target.includes(normalizedKeyword))) {
        return false;
      }
    }

    return true;
  }

  async function processDirectory(dirPath) {
    if (searchState.isCancelled) return;

    searchState.processedDirs++;

    try {
      const entries = await fs.promises.readdir(dirPath, { withFileTypes: true });
      const subdirs = [];

      for (const entry of entries) {
        if (searchState.isCancelled) break;

        const fullPath = path.join(dirPath, entry.name);

        try {
          if (entry.isFile()) {
            searchState.totalFilesScanned++;

            if (matchesCriteria(entry.name)) {
              try {
                const stat = await fs.promises.stat(fullPath);
                results.push({
                  name: entry.name,
                  path: fullPath,
                  size: formatFileSize(stat.size),
                  sizeBytes: stat.size,
                  modified: stat.mtime.toISOString(),
                  extension: path.extname(entry.name)
                });

                if (results.length >= maxResults) {
                  searchState.isCancelled = true;
                  return { complete: true, results: results.slice(0, maxResults) };
                }

                if (results.length % BATCH_SIZE === 0 && progressCallback) {
                  progressCallback({
                    searchId,
                    resultsCount: results.length,
                    filesScanned: searchState.totalFilesScanned,
                    dirsScanned: searchState.processedDirs,
                    currentPath: dirPath,
                    totalDirs: searchState.pendingDirs.length + searchState.processedDirs
                  });
                }
              } catch (statErr) {
                // 忽略文件stat错误
              }
            }
          } else if (entry.isDirectory() && recursive) {
            const hidePrefixes = ['$', '.', '__', '@'];
            if (!hidePrefixes.some(prefix => entry.name.startsWith(prefix))) {
              subdirs.push(fullPath);
            }
          }
        } catch (err) {
          if (err.code === 'EACCES' || err.code === 'EPERM') {
            continue;
          }
        }
      }

      if (subdirs.length > 0 && !searchState.isCancelled) {
        searchState.pendingDirs.push(...subdirs);
      }
    } catch (err) {
      if (err.code === 'EACCES' || err.code === 'ENOENT') {
        return;
      }
    }

    return { complete: !searchState.isCancelled, results };
  }

  // 并发处理目录
  while (searchState.pendingDirs.length > 0 && !searchState.isCancelled) {
    const batchDirs = searchState.pendingDirs.splice(0, CONCURRENT_DIRS);
    const batchPromises = batchDirs.map(dir => processDirectory(dir));
    
    await Promise.all(batchPromises);
    
    if (progressCallback) {
      progressCallback({
        searchId,
        resultsCount: results.length,
        filesScanned: searchState.totalFilesScanned,
        dirsScanned: searchState.processedDirs,
        currentPath: searchState.pendingDirs[0] || rootPath,
        totalDirs: searchState.pendingDirs.length + searchState.processedDirs
      });
    }
  }

  // 清理搜索状态
  activeSearches.delete(searchId);

  return {
    searchId,
    results: results.slice(0, maxResults),
    stats: {
      totalFilesScanned: searchState.totalFilesScanned,
      totalDirsScanned: searchState.processedDirs,
      resultsFound: results.length,
      completed: !searchState.isCancelled
    }
  };
}

function formatFileSize(bytes) {
  if (bytes === 0) return '0 B';
  const k = 1024;
  const sizes = ['B', 'KB', 'MB', 'GB', 'TB'];
  const i = Math.floor(Math.log(bytes) / Math.log(k));
  return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
}

async function getFileInfo(filePath) {
  try {
    const stat = await fs.promises.stat(filePath);
    return {
      name: path.basename(filePath),
      path: filePath,
      size: formatFileSize(stat.size),
      sizeBytes: stat.size,
      created: stat.birthtime.toISOString(),
      modified: stat.mtime.toISOString(),
      extension: path.extname(filePath),
      isDirectory: stat.isDirectory()
    };
  } catch (err) {
    return null;
  }
}

async function openFile(filePath) {
  const { shell } = require('electron');
  try {
    await shell.openPath(filePath);
    return { success: true };
  } catch (err) {
    return { success: false, error: err.message };
  }
}

async function openInExplorer(filePath) {
  const { shell } = require('electron');
  shell.showItemInFolder(filePath);
  return { success: true };
}

module.exports = {
  getAvailableDrives,
  searchDirectory,
  cancelSearch,
  getFileInfo,
  openFile,
  openInExplorer
};