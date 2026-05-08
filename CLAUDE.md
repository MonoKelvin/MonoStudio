# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Development Commands

### Core Development
- `npm run dev` - Run in development mode (starts both frontend dev server and Electron app)
- `npm run renderer:dev` - Frontend development only (Vite dev server on port 5173)
- `npm run electron:dev` - Electron development only (expects frontend running on localhost:5173)
- `npm run desktop:dev` - Same as `dev` - runs both concurrently
- `npm run build` - Build frontend for production (outputs to `dist/`)
- `npm run desktop:build` - Build full Electron application with packaging
- `npm run desktop:pack` - Build without packaging (creates executable in `dist_electron/`)

### Development Workflow
```bash
# Start development
npm run dev

# Make changes in src/
# The app auto-reloads for both frontend and Electron windows

# When ready to test production build
npm run build
npm run desktop:pack
```

## Architecture Overview

### Technology Stack
- **Electron 32.1.2** - Desktop application framework
- **Vue 3.5.13** - Frontend framework with Composition API
- **Vite 5.4.11** - Build tool and dev server
- **No testing framework** - This is a feature-focused app without automated tests

### Application Structure

**Main Process** (`electron/`)
- `main.js` - Entry point, manages windows, IPC, and system integrations
- `preload.js` - Security context for renderer process
- `settings.js` - Settings management using Electron's userData API
- `windowManager.js` - Multi-window management utilities
- `userData*.js` - User data path management
- `desktopIcons.js` - Windows desktop icon functionality
- `fileSearch.js` - System file search implementation
- `utils.js` - Shared utilities

**Frontend Process** (`src/`)
- **Shell** (`modules/shell/`) - Application shell components
  - `components/` - Title bar, sidebar navigation, workspace
  - `use*.js` - Composables for theme, tools, workbench management
- **Tools** (`modules/tools/`) - Feature modules as separate directories
  - Each tool has its own directory with `*.vue` and `*.js` files
  - Tools are dynamically loaded via `useTools.js`
- **Components** (`components/`) - Shared Vue components
  - `base/` - Reusable UI components (buttons, inputs, modals)
  - `settings/` - Settings-related components
- **Services** (`common/services/`) - Storage, toast, dialog services
- **Config** (`config/`) - Tool configuration and app settings

### Tool Architecture

**Tool Registration**
- Tools are defined in `src/config/toolConfig.js`
- Each tool requires: `id`, `name`, `description`, `category`
- Components are dynamically mapped in `useTools.js`
- Tools are loaded lazily when selected

**Tool Categories**
- 个人生活 (Personal Life) - Inspiration, Work Notes, Musings
- 实用工具 (Utilities) - Password Generator, Unit Converter, Time Tools
- 系统工具 (System) - Desktop Icons, File Search, System Info
- 开发工具 (Developer) - GUID Generator, JSON Formatter, Base64
- 媒体工具 (Media) - Image/Video/Audio Converters, Screen Capture
- 学习 (Learning) - Japanese Learning tools

**Personal Life Protection**
- Tools in "个人生活" category are locked behind password
- Protection modes: session, every access, time-based, inactive
- Settings stored in userData with configurable timeout

### Key Patterns

**Component Structure**
- Tools use `ToolView.vue` as main component
- Props: `tool-name`, `tool-description`, `theme-mode`, `theme-options`
- Events: `change-theme`, `change-accent-color`, etc.

**IPC Communication**
- Main and renderer communicate via Electron's IPC API
- Preload script exposes secure APIs to renderer
- Settings stored in userData directory

**State Management**
- Vue 3 Composition API throughout
- No centralized state management - composables handle local state
- Settings persisted via `electron-settings` package

**Styling**
- CSS modules with scoped styles
- Theme system with dark/light modes
- CSS custom properties for theming

### Build Output
- Frontend: `dist/` - Static assets for Electron
- Electron: `dist_electron/` - Packaged executable
- Portable build creates `MonoStudio-${version}.exe`

### Development Notes
- No formal testing framework exists
- Chinese language interface throughout
- Electron Builder configured for Windows portable executables
- Development server runs on port 5173
- Concurrently runs frontend and Electron dev servers
- Uses `wait-on` to ensure frontend is ready before starting Electron