<template>
  <header class="window-titlebar">
    <div class="window-drag-region" @dblclick="$emit('toggle-maximize')">
      <div class="brand">
        <span class="brand-logo">◆</span>
        <div class="brand-meta">
          <strong>MonoStudio</strong>
          <small>Desktop Toolbox</small>
        </div>
      </div>
    </div>
    <div class="titlebar-tools no-drag">
      <div class="window-controls">
        <button
          class="win-btn pin"
          :class="{ active: isPinned }"
          v-tooltip:top="'置顶'"
          @click="$emit('toggle-pin')"
        >
          <img :src="pinIcon" alt="" class="win-icon" />
        </button>
        <button class="win-btn" v-tooltip:top="'最小化'" @click="$emit('minimize')">
          <img :src="minimizeIcon" alt="" class="win-icon" />
        </button>
        <button class="win-btn" v-tooltip:top="isMaximized ? '恢复' : '最大化'" @click="$emit('toggle-maximize')">
          <img :src="maximizeIcon" alt="" class="win-icon" />
        </button>
        <button class="win-btn close" v-tooltip:top="'关闭'" @click="$emit('close')">
          <img :src="closeIcon" alt="" class="win-icon" />
        </button>
      </div>
    </div>
  </header>
</template>

<script>
import minimizeIcon from '../../../assets/icons/win-minimize.svg';
import maximizeIcon from '../../../assets/icons/win-maximize.svg';
import restoreIcon from '../../../assets/icons/win-restore.svg';
import closeIcon from '../../../assets/icons/win-close.svg';
import pinIcon from '../../../assets/icons/pin.svg';

export default {
  name: 'TitleBar',
  props: {
    isMaximized: {
      type: Boolean,
      default: false
    },
    isPinned: {
      type: Boolean,
      default: false
    }
  },
  data() {
    return {
      minimizeIcon,
      closeIcon,
      pinIcon
    };
  },
  computed: {
    maximizeIcon() {
      return this.isMaximized ? restoreIcon : maximizeIcon;
    }
  }
};
</script>