const { spawn } = require('child_process');
const path = require('path');
const fs = require('fs/promises');
const os = require('os');
const { app } = require('electron');

const WORKSPACE_DIR = 'workspaces';

async function getWorkspacesPath() {
  const userDataPath = app.getPath('userData');
  return path.join(userDataPath, WORKSPACE_DIR);
}

async function ensureWorkspacesDir() {
  const dir = await getWorkspacesPath();
  try {
    await fs.mkdir(dir, { recursive: true });
  } catch (e) {
  }
  return dir;
}

async function runPowerShell(script) {
  const tempDir = os.tmpdir();
  const tempFile = path.join(tempDir, `mono_ws_${Date.now()}_${Math.random().toString(36).substr(2, 9)}.ps1`);
  
  try {
    await fs.writeFile(tempFile, script, 'utf8');
    
    return new Promise((resolve, reject) => {
      const child = spawn('powershell', [
        '-NoProfile',
        '-ExecutionPolicy', 'Bypass',
        '-File', tempFile
      ], {
        maxBuffer: 1024 * 1024 * 10,
        windowsHide: true
      });

      let stdout = '';
      let stderr = '';

      child.stdout.on('data', (data) => {
        stdout += data.toString();
      });

      child.stderr.on('data', (data) => {
        stderr += data.toString();
      });

      child.on('close', (code) => {
        fs.unlink(tempFile).catch(() => {});
        if (code === 0) {
          resolve(stdout);
        } else {
          reject(new Error(`PowerShell failed (${code}): ${stderr}`));
        }
      });

      child.on('error', (error) => {
        fs.unlink(tempFile).catch(() => {});
        reject(error);
      });
    });
  } catch (e) {
    try {
      await fs.unlink(tempFile);
    } catch (err) {}
    throw e;
  }
}

async function getOpenWindows() {
  try {
    const script = `
$OutputEncoding = [Console]::OutputEncoding = [System.Text.UTF8Encoding]::UTF8
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class User32 {
  public delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);
  [DllImport("user32.dll")]
  public static extern bool EnumWindows(EnumWindowsProc enumProc, IntPtr lParam);
  [DllImport("user32.dll")]
  public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);
  [DllImport("user32.dll", CharSet = CharSet.Unicode)]
  public static extern int GetWindowText(IntPtr hWnd, StringBuilder lpString, int nMaxCount);
  [DllImport("user32.dll")]
  public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);
  [DllImport("user32.dll")]
  public static extern bool IsWindowVisible(IntPtr hWnd);
  [DllImport("user32.dll")]
  public static extern IntPtr GetParent(IntPtr hWnd);
  [DllImport("user32.dll")]
  public static extern int GetWindowLong(IntPtr hWnd, int nIndex);
  [DllImport("user32.dll")]
  public static extern bool IsIconic(IntPtr hWnd);
  [StructLayout(LayoutKind.Sequential)]
  public struct RECT { public int Left, Top, Right, Bottom; }
  public const int GWL_EXSTYLE = -20;
  public const int WS_EX_TOOLWINDOW = 0x00000080;
  public const int WS_EX_APPWINDOW = 0x00040000;
}
"@

$systemProcesses = @(
  "SystemSettings", "ApplicationFrameHost", "TextInputHost", "SearchHost",
  "ShellExperienceHost", "RuntimeBroker", "TaskHostw", "services",
  "System Idle Process", "csrss", "smss", "wininit", "winlogon",
  "dwm", "explorer", "conhost", "taskhostw", "sihost", "ctfmon",
  "dllhost", "msiexec", "spoolsv", "SecurityHealthService",
  "OneDrive", "Origin", "EpicGamesLauncher", "Steam", "SteamService",
  "RiotClient", "Valorant", "LeagueClient", "WeGame", "WeGameTouch",
  "NVIDIA Share", "NVIDIA Container", "AMD Radeon Settings",
  "Logitech Download Assistant", "Logitech Gaming Framework"
)

$windows = @()
$enumProc = [User32+EnumWindowsProc]{
  param($hWnd, $lParam)
  $parent = [User32]::GetParent($hWnd)
  if ($parent -eq [IntPtr]::Zero) {
    $exStyle = [User32]::GetWindowLong($hWnd, [User32]::GWL_EXSTYLE)
    if (($exStyle -band [User32]::WS_EX_TOOLWINDOW) -eq 0 -or ($exStyle -band [User32]::WS_EX_APPWINDOW) -ne 0) {
      $sb = New-Object System.Text.StringBuilder 256
      [User32]::GetWindowText($hWnd, $sb, 256) | Out-Null
      $title = $sb.ToString()
      if ($title -and $title.Trim() -ne '') {
        $isMinimized = [User32]::IsIconic($hWnd)
        $rect = New-Object User32+RECT
        [User32]::GetWindowRect($hWnd, [ref]$rect) | Out-Null
        $procId = 0
        [User32]::GetWindowThreadProcessId($hWnd, [ref]$procId) | Out-Null
        $width = $rect.Right - $rect.Left
        $height = $rect.Bottom - $rect.Top
        $x = $rect.Left
        $y = $rect.Top
        if ($isMinimized) {
          $x = 100
          $y = 100
          $width = 800
          $height = 600
        } elseif ($width -le 50 -or $height -le 50) {
          return $true
        }
        $proc = Get-Process -Id $procId -ErrorAction SilentlyContinue
        if ($proc) {
          $procName = $proc.ProcessName
          $skip = $false
          foreach ($sp in $systemProcesses) {
            if ($procName -like "*$sp*") {
              $skip = $true
              break
            }
          }
          if (-not $skip) {
            $obj = @{
              hwnd = $hWnd.ToInt64()
              title = $title
              path = if ($proc.Path) { $proc.Path } else { '' }
              processName = $procName
              x = $x
              y = $y
              width = $width
              height = $height
              pid = $procId
              isMinimized = $isMinimized
            }
            $script:windows += $obj
          }
        }
      }
    }
  }
  return $true
}
[User32]::EnumWindows($enumProc, [IntPtr]::Zero) | Out-Null
$windows | ConvertTo-Json -Compress
`;

    const result = await runPowerShell(script);
    if (result && result.trim()) {
      const parsed = JSON.parse(result);
      return Array.isArray(parsed) ? parsed : [parsed];
    }
    return [];
  } catch (e) {
    console.error('Failed to get open windows:', e);
    return [];
  }
}

async function getRunningApps() {
  try {
    const script = `
$OutputEncoding = [Console]::OutputEncoding = [System.Text.UTF8Encoding]::UTF8
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Collections.Generic;
public class Win32 {
  public delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);
  [DllImport("user32.dll")]
  public static extern bool EnumWindows(EnumWindowsProc enumProc, IntPtr lParam);
  [DllImport("user32.dll")]
  public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);
  [DllImport("user32.dll")]
  public static extern bool IsWindowVisible(IntPtr hWnd);
  [DllImport("user32.dll")]
  public static extern IntPtr GetParent(IntPtr hWnd);
  [DllImport("user32.dll")]
  public static extern int GetWindowLong(IntPtr hWnd, int nIndex);
  [DllImport("user32.dll")]
  public static extern bool IsIconic(IntPtr hWnd);
  [DllImport("user32.dll", CharSet = CharSet.Unicode)]
  public static extern int GetWindowText(IntPtr hWnd, StringBuilder lpString, int nMaxCount);
  public const int GWL_EXSTYLE = -20;
  public const int WS_EX_TOOLWINDOW = 0x00000080;
  public const int WS_EX_APPWINDOW = 0x00040000;
}
"@

$apps = @{}
$enumProc = [Win32+EnumWindowsProc]{
  param($hWnd, $lParam)
  $parent = [Win32]::GetParent($hWnd)
  if ($parent -eq [IntPtr]::Zero) {
    $exStyle = [Win32]::GetWindowLong($hWnd, [Win32]::GWL_EXSTYLE)
    $isToolWindow = ($exStyle -band [Win32]::WS_EX_TOOLWINDOW) -ne 0
    $isAppWindow = ($exStyle -band [Win32]::WS_EX_APPWINDOW) -ne 0
    if (-not $isToolWindow -or $isAppWindow) {
      if ([Win32]::IsWindowVisible($hWnd)) {
        $sb = New-Object System.Text.StringBuilder 256
        [Win32]::GetWindowText($hWnd, $sb, 256) | Out-Null
        $title = $sb.ToString()
        if ($title) {
          $procId = 0
          [Win32]::GetWindowThreadProcessId($hWnd, [ref]$procId) | Out-Null
          if ($procId -gt 0 -and -not $apps.ContainsKey($procId)) {
            try {
              $proc = Get-Process -Id $procId -ErrorAction Stop
              $isMinimized = [Win32]::IsIconic($hWnd)
              $apps[$procId] = @{
                pid = $procId
                name = $proc.ProcessName
                title = $title
                path = if ($proc.Path) { $proc.Path } else { '' }
                isMinimized = $isMinimized
              }
            } catch {}
          }
        }
      }
    }
  }
  return $true
}
[Win32]::EnumWindows($enumProc, [IntPtr]::Zero) | Out-Null
$apps.Values | ConvertTo-Json -Compress
`;

    const result = await runPowerShell(script);
    if (result && result.trim()) {
      const parsed = JSON.parse(result);
      return Array.isArray(parsed) ? parsed : [parsed];
    }
    return [];
  } catch (e) {
    console.error('Failed to get running apps:', e);
    return [];
  }
}

async function getWindowDetails(hwnd) {
  try {
    const script = `
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class WinAPI {
  [DllImport("user32.dll")]
  public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);
  [DllImport("user32.dll")]
  public static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int X, int Y, int cx, int cy, uint uFlags);
  [DllImport("user32.dll")]
  public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
  [DllImport("user32.dll")]
  public static extern IntPtr FindWindow(string lpClassName, string lpWindowName);
  [DllImport("user32.dll")]
  public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);
  [DllImport("user32.dll")]
  public static extern int GetWindowText(IntPtr hWnd, StringBuilder lpString, int nMaxCount);
  [StructLayout(LayoutKind.Sequential)]
  public struct RECT { public int Left, Top, Right, Bottom; }
  public const uint SWP_NOZORDER = 0x0004;
  public const uint SWP_SHOWWINDOW = 0x0040;
  public const int SW_RESTORE = 9;
}
"@
$hwnd = [IntPtr]${hwnd}
$rect = New-Object WinAPI+RECT
[WinAPI]::GetWindowRect($hwnd, [ref]$rect) | Out-Null
$procId = 0
[WinAPI]::GetWindowThreadProcessId($hwnd, [ref]$procId) | Out-Null
$proc = Get-Process -Id $procId -ErrorAction SilentlyContinue
@{
  hwnd = $hwnd
  x = $rect.Left
  y = $rect.Top
  width = $rect.Right - $rect.Left
  height = $rect.Bottom - $rect.Top
  title = ""
  path = if ($proc) { $proc.Path } else { "" }
  processName = if ($proc) { $proc.ProcessName } else { "" }
  pid = $procId
} | ConvertTo-Json -Compress
`;

    const result = await runPowerShell(script);
    if (result && result.trim()) {
      return JSON.parse(result);
    }
    return null;
  } catch (e) {
    console.error('Failed to get window details:', e);
    return null;
  }
}

async function setWindowPosition(hwnd, x, y, width, height) {
  try {
    const script = `
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class WinAPI {
  [DllImport("user32.dll")]
  public static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int X, int Y, int cx, int cy, uint uFlags);
  [DllImport("user32.dll")]
  public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
  public const uint SWP_NOZORDER = 0x0004;
  public const uint SWP_SHOWWINDOW = 0x0040;
  public const int SW_RESTORE = 9;
}
"@
$hwnd = [IntPtr]${hwnd}
[WinAPI]::ShowWindow($hwnd, 9) | Out-Null
Start-Sleep -Milliseconds 100
[WinAPI]::SetWindowPos($hwnd, [IntPtr]::Zero, ${x}, ${y}, ${width}, ${height}, (WinAPI.SWP_NOZORDER -bor WinAPI.SWP_SHOWWINDOW)) | Out-Null
"ok"
`;

    await runPowerShell(script);
    return { success: true };
  } catch (e) {
    console.error('Failed to set window position:', e);
    return { success: false, error: e.message };
  }
}

async function launchApplication(exePath, args = [], workingDir = null) {
  return new Promise((resolve) => {
    try {
      const options = {
        detached: true,
        stdio: 'ignore'
      };
      if (workingDir) {
        options.cwd = workingDir;
      }

      const child = spawn(exePath, args, options);
      child.unref();

      setTimeout(() => {
        resolve({ success: true, pid: child.pid });
      }, 500);
    } catch (e) {
      resolve({ success: false, error: e.message });
    }
  });
}

async function saveWorkspace(name, windows) {
  try {
    await ensureWorkspacesDir();
    const dir = await getWorkspacesPath();
    const timestamp = Date.now();
    const filename = `${name.replace(/[^a-zA-Z0-9\u4e00-\u9fa5_-]/g, '_')}_${timestamp}.json`;
    const filepath = path.join(dir, filename);

    const workspace = {
      name,
      timestamp,
      windows: windows.map(w => ({
        title: w.title,
        path: w.path,
        processName: w.processName,
        x: w.x,
        y: w.y,
        width: w.width,
        height: w.height,
        args: w.args || []
      }))
    };

    await fs.writeFile(filepath, JSON.stringify(workspace, null, 2), 'utf-8');
    return { success: true, filepath };
  } catch (e) {
    console.error('Failed to save workspace:', e);
    return { success: false, error: e.message };
  }
}

async function loadWorkspaces() {
  try {
    await ensureWorkspacesDir();
    const dir = await getWorkspacesPath();

    const files = await fs.readdir(dir);
    const workspaces = [];

    for (const file of files) {
      if (file.endsWith('.json')) {
        try {
          const content = await fs.readFile(path.join(dir, file), 'utf-8');
          const data = JSON.parse(content);
          workspaces.push({
            id: file.replace('.json', ''),
            name: data.name,
            timestamp: data.timestamp,
            windowCount: data.windows?.length || 0,
            filepath: path.join(dir, file)
          });
        } catch (e) {
          console.warn('Failed to load workspace file:', file, e);
        }
      }
    }

    return workspaces.sort((a, b) => b.timestamp - a.timestamp);
  } catch (e) {
    console.error('Failed to load workspaces:', e);
    return [];
  }
}

async function loadWorkspaceData(filepath) {
  try {
    const content = await fs.readFile(filepath, 'utf-8');
    return JSON.parse(content);
  } catch (e) {
    console.error('Failed to load workspace data:', e);
    return null;
  }
}

async function deleteWorkspace(filepath) {
  try {
    await fs.unlink(filepath);
    return { success: true };
  } catch (e) {
    console.error('Failed to delete workspace:', e);
    return { success: false, error: e.message };
  }
}

async function restoreWorkspace(workspace) {
  const results = [];

  for (const win of workspace.windows) {
    try {
      if (win.path) {
        await launchApplication(win.path, win.args || []);
        await new Promise(r => setTimeout(r, 1000));

        const hwnds = await findWindowByTitle(win.title);
        if (hwnds.length > 0) {
          await setWindowPosition(hwnds[0], win.x, win.y, win.width, win.height);
          results.push({ title: win.title, success: true });
        } else {
          results.push({ title: win.title, success: false, error: 'Window not found after launch' });
        }
      } else {
        results.push({ title: win.title, success: false, error: 'No executable path' });
      }
    } catch (e) {
      results.push({ title: win.title, success: false, error: e.message });
    }
  }

  return results;
}

async function findWindowByTitle(title) {
  try {
    const script = `
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class FindWin {
  public delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);
  [DllImport("user32.dll")]
  public static extern bool EnumWindows(EnumWindowsProc enumProc, IntPtr lParam);
  [DllImport("user32.dll")]
  public static extern int GetWindowText(IntPtr hWnd, StringBuilder lpString, int nMaxCount);
  [DllImport("user32.dll")]
  public static extern bool IsWindowVisible(IntPtr hWnd);
}
"@
$result = @()
$enumProc = [FindWin+EnumWindowsProc]{
  param($hwnd, $lparam)
  $sb = New-Object System.Text.StringBuilder 256
  [FindWin]::GetWindowText($hwnd, $sb, 256) | Out-Null
  $text = $sb.ToString()
  if ([FindWin]::IsWindowVisible($hwnd) -and $text -like "*${title}*") {
    $script:result += $hwnd.ToInt64()
  }
  return $true
}
[FindWin]::EnumWindows($enumProc, [IntPtr]::Zero) | Out-Null
$result | ConvertTo-Json -Compress
`;

    const output = await runPowerShell(script);
    if (output && output.trim()) {
      const parsed = JSON.parse(output);
      return Array.isArray(parsed) ? parsed.map(n => Number(n)) : [Number(parsed)];
    }
    return [];
  } catch (e) {
    console.error('Failed to find window by title:', e);
    return [];
  }
}

module.exports = {
  getOpenWindows,
  getRunningApps,
  getWindowDetails,
  setWindowPosition,
  launchApplication,
  saveWorkspace,
  loadWorkspaces,
  loadWorkspaceData,
  deleteWorkspace,
  restoreWorkspace
};