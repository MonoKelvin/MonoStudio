<template>
  <BasePanel class="panel desktop-manager-panel">
    <section class="desktop-toolbar">
      <BaseInput
        :value="searchQuery"
        type="text"
        placeholder="搜索桌面项（文件 / 文件夹 / 快捷方式）"
        @input="searchQuery = $event"
      />
      <BaseButton class="desktop-refresh-btn" @click="loadDesktopItems">
        <img :src="refreshIcon" alt="" aria-hidden="true" />
        <span>刷新</span>
      </BaseButton>
    </section>
    <section class="desktop-filters">
      <BaseButton
        v-for="option in kindOptions"
        :key="option.value"
        size="sm"
        class="desktop-filter-btn"
        :variant="activeKind === option.value ? 'primary' : 'default'"
        @click="activeKind = option.value"
      >
        {{ option.label }}（{{ countByKind(option.value) }}）
      </BaseButton>
    </section>

    <section class="desktop-list">
      <article
        v-for="item in filteredItems"
        :key="item.id"
        class="desktop-item-card"
        v-tooltip:top="'双击打开'"
        @dblclick="handleOpenItem(item)"
      >
        <div class="desktop-item-icon">
          <img v-if="item.iconDataUrl" :src="item.iconDataUrl" alt="" />
          <span v-else>{{ fallbackIcon(item.kind) }}</span>
        </div>

        <div class="desktop-item-main">
          <div class="desktop-item-title-row">
            <strong>{{ item.name }}</strong>
            <small class="desktop-kind">{{ kindLabel(item.kind) }}</small>
            <small v-if="item.supportsIconModify" class="desktop-capability">可改图标</small>
            <small v-if="item.action === 'add'" class="desktop-removed-tag">已移除</small>
            <kbd v-if="getShortcutKey(item)" class="desktop-shortcut-key">{{ getShortcutKey(item) }}</kbd>
          </div>
          <a
            class="desktop-path"
            v-tooltip:top="getDisplayPath(item)"
            @click.stop="handleOpenFolder(item)"
            @dblclick.stop
          >{{ getDisplayPath(item) }}</a>
        </div>

        <div class="desktop-item-op">
          <BaseButton
            v-if="item.action === 'add' && item.kind === 'shortcut'"
            size="sm"
            class="desktop-icon-action"
            v-tooltip:top="'添加快捷方式（重新创建）'"
            @click="handleAddShortcut(item)"
            @dblclick.stop
          >
            <span aria-hidden="true" class="desktop-action-svg">
              <img :src="addActionIcon" alt="" />
            </span>
          </BaseButton>
          <template v-else>
            <BaseButton
              v-if="item.supportsIconModify"
              size="sm"
              class="desktop-icon-action"
              v-tooltip:top="item.kind === 'system' ? '修改系统图标' : '修改图标（仅快捷方式和文件夹）'"
              @click="handleChangeIcon(item)"
              @dblclick.stop
            >
              <span aria-hidden="true" class="desktop-action-svg">
                <img :src="editActionIcon" alt="" />
              </span>
            </BaseButton>
            <BaseButton
              v-if="item.kind !== 'system'"
              size="sm"
              class="desktop-icon-action desktop-icon-action-danger"
              v-tooltip:top="item.kind === 'shortcut' ? '移除快捷方式（可恢复）' : '移除并删除（不可恢复）'"
              @click="handleRemove(item)"
              @dblclick.stop
            >
              <span aria-hidden="true" class="desktop-action-svg">
                <img :src="deleteActionIcon" alt="" />
              </span>
            </BaseButton>
          </template>
        </div>
      </article>

      <div v-if="!loading && filteredItems.length === 0" class="empty">没有匹配结果</div>
      <div v-if="loading" class="empty">正在读取桌面项目...</div>
    </section>

    <footer class="panel-foot">
      <span>{{ statusText }}</span>
    </footer>
  </BasePanel>
</template>

<script>
import addActionIcon from '../../../assets/icons/add.svg';
import editActionIcon from '../../../assets/icons/edit.svg';
import deleteActionIcon from '../../../assets/icons/delete.svg';
import refreshIcon from '../../../assets/icons/refresh.svg';

export default {
  name: 'DesktopIconToolView',
  data() {
    return {
      loading: false,
      desktopItems: [],
      searchQuery: '',
      activeKind: 'all',
      kindOptions: [
        { value: 'all', label: '全部' },
        { value: 'shortcut', label: '快捷方式' },
        { value: 'folder', label: '文件夹' },
        { value: 'file', label: '文件' }
      ],
      statusText: '等待读取桌面项目',
      addActionIcon,
      editActionIcon,
      deleteActionIcon,
      refreshIcon
    };
  },
  computed: {
    filteredItems() {
      const query = this.searchQuery.trim().toLowerCase();
      return this.desktopItems.filter((item) => {
        const kindMatch = this.activeKind === 'all' || item.kind === this.activeKind;
        const queryMatch = !query || item.name.toLowerCase().includes(query) || item.path.toLowerCase().includes(query);
        return kindMatch && queryMatch;
      });
    }
  },
  mounted() {
    this.loadDesktopItems();
  },
  methods: {
    getDesktopApi() {
      const windowDesktop = window.monoWindow?.desktop || {};
      const monoDesktop = window.monoDesktop || {};
      const merged = { ...windowDesktop, ...monoDesktop };
      return Object.keys(merged).length ? merged : null;
    },
    kindLabel(kind) {
      if (kind === 'shortcut') return '快捷方式';
      if (kind === 'folder') return '文件夹';
      if (kind === 'system') return '系统图标';
      return '文件';
    },
    fallbackIcon(kind) {
      if (kind === 'shortcut') return '↗';
      if (kind === 'folder') return '📁';
      if (kind === 'system') return '🖥';
      return '📄';
    },
    countByKind(kind) {
      if (kind === 'all') return this.desktopItems.length;
      return this.desktopItems.filter((item) => item.kind === kind).length;
    },
    getDisplayPath(item) {
      if (item.kind === 'shortcut' && item.targetPath) {
        return item.targetPath;
      }
      return item.path;
    },
    async loadDesktopItems() {
      const desktopApi = this.getDesktopApi();
      if (!desktopApi?.listItems) {
        this.statusText = '当前环境不支持桌面扫描，请重启桌面端应用后重试';
        return;
      }
      this.loading = true;
      try {
        this.desktopItems = await desktopApi.listItems();
        this.statusText = `已读取 ${this.desktopItems.length} 项`;
      } catch (error) {
        this.statusText = `读取失败：${error.message || String(error)}`;
      } finally {
        this.loading = false;
      }
    },

    async handleRemove(item) {
      const desktopApi = this.getDesktopApi();
      if (!desktopApi?.removeItem) {
        this.statusText = '当前环境不支持移除操作';
        return;
      }
      const isShortcut = item.kind === 'shortcut';
      const tip = isShortcut
        ? `确认移除快捷方式「${item.name}」？\n后续可在本工具中点击"添加"恢复。`
        : `确认永久删除「${item.name}」？\n该操作不可恢复。`;
      if (!window.confirm(tip)) return;

      try {
        await desktopApi.removeItem(item.path);
        this.statusText = isShortcut
          ? `已移除快捷方式：${item.name}（可恢复）`
          : `已删除：${item.name}`;
        await this.loadDesktopItems();
      } catch (error) {
        this.statusText = `移除失败：${error.message || String(error)}`;
      }
    },

    async handleAddShortcut(item) {
      const desktopApi = this.getDesktopApi();
      if (!desktopApi?.restoreShortcut) {
        this.statusText = '当前环境不支持快捷方式恢复';
        return;
      }
      try {
        await desktopApi.restoreShortcut(item.path);
        this.statusText = `已恢复快捷方式：${item.name}`;
        await this.loadDesktopItems();
      } catch (error) {
        this.statusText = `恢复失败：${error.message || String(error)}`;
      }
    },

    async handleChangeIcon(item) {
      const desktopApi = this.getDesktopApi();
      if (!desktopApi?.pickIconFile || !desktopApi?.updateItemIcon) {
        this.statusText = '当前环境不支持修改图标';
        return;
      }
      try {
        const iconPath = await desktopApi.pickIconFile();
        if (!iconPath) return;
        await desktopApi.updateItemIcon(item.path, iconPath);
        this.statusText = `已更新图标：${item.name}`;
        await this.loadDesktopItems();
      } catch (error) {
        this.statusText = `修改图标失败：${error.message || String(error)}`;
      }
    },

    async handleOpenItem(item) {
      const desktopApi = this.getDesktopApi();
      if (!desktopApi?.openItem) {
        this.statusText = '当前环境不支持双击打开';
        return;
      }
      try {
        const targetPath = (item.kind === 'shortcut' && item.targetPath) ? item.targetPath : item.path;
        await desktopApi.openItem(targetPath);
      } catch (error) {
        this.statusText = `打开失败：${error.message || String(error)}`;
      }
    },

    getShortcutKey(item) {
      return item.shortcutKey || item.hotkey || item.key || null;
    },

    async handleOpenFolder(item) {
      const desktopApi = this.getDesktopApi();
      if (!desktopApi?.openFolder) {
        this.statusText = '当前环境不支持打开文件夹';
        return;
      }
      try {
        const targetPath = (item.kind === 'shortcut' && item.targetPath) ? item.targetPath : item.path;
        await desktopApi.openFolder(targetPath);
      } catch (error) {
        this.statusText = `打开文件夹失败：${error.message || String(error)}`;
      }
    }
  }
};
</script>

<style scoped>
.desktop-filter-btn {
  border-radius: 12px !important;
  margin-right: 8px;
}

.desktop-filter-btn:last-child {
  margin-right: 0;
}
</style>
