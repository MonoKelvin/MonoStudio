<template>
  <div class="process-icon">
    {{ getIcon() }}
  </div>
</template>

<script>
export default {
  name: 'ProcessIcon',
  props: {
    name: {
      type: String,
      required: true
    }
  },
  setup(props) {
    const getIcon = () => {
      const lowerName = props.name.toLowerCase()
      if (lowerName.includes('chrome') || lowerName.includes('msedge')) return '🌐'
      if (lowerName.includes('code') || lowerName.includes('vscode')) return '💻'
      if (lowerName.includes('node')) return '🟢'
      if (lowerName.includes('python')) return '🐍'
      if (lowerName.includes('java')) return '☕'
      if (lowerName.includes('explorer')) return '🗂️'
      if (lowerName.includes('taskmgr')) return '📊'
      if (lowerName.includes('steam')) return '🎮'
      if (lowerName.includes('discord')) return '💬'
      if (lowerName.includes('spotify')) return '🎵'
      if (lowerName.includes('qq')) return '💬'
      if (lowerName.includes('wechat')) return '💚'
      if (lowerName.includes('electron')) return '⚡'
      if (lowerName.includes('cmd') || lowerName.includes('powershell')) return '⌨️'
      if (lowerName.includes('svchost')) return '⚙️'
      return '📄'
    }

    return { getIcon }
  }
}
</script>

<style scoped>
.process-icon {
  width: 20px;
  height: 20px;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 14px;
  flex-shrink: 0;
}
</style>