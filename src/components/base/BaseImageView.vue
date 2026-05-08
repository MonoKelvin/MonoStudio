<template>
  <div class="base-image-view" :class="{ 'fullscreen': isFullscreen }">
    <!-- 工具栏 -->
    <div class="image-toolbar" v-if="showToolbar">
      <div class="toolbar-left">
        <BaseButton
          size="sm"
          variant="default"
          @click="handleClose"
          class="close-btn"
        >
          ✕
        </BaseButton>
        <span class="image-info">{{ imageInfo }}</span>
      </div>
      <div class="toolbar-right">
        <BaseButton
          size="sm"
          variant="default"
          @click="handleZoomOut"
          :disabled="scale <= 0.25"
          title="缩小"
        >
          −
        </BaseButton>
        <span class="scale-info">{{ Math.round(scale * 100) }}%</span>
        <BaseButton
          size="sm"
          variant="default"
          @click="handleZoomIn"
          :disabled="scale >= 3"
          title="放大"
        >
          +
        </BaseButton>
        <BaseButton
          size="sm"
          variant="default"
          @click="handleReset"
          title="重置"
        >
          ⟲
        </BaseButton>
        <BaseButton
          size="sm"
          variant="default"
          @click="toggleFullscreen"
          title="全屏"
        >
          {{ isFullscreen ? '⊠' : '⛶' }}
        </BaseButton>
      </div>
    </div>

    <!-- 图片内容 -->
    <div class="image-container" @wheel.prevent="handleWheel">
      <img
        ref="imageRef"
        :src="src"
        :alt="alt"
        :style="imageStyle"
        @load="handleImageLoad"
        @error="handleImageError"
      />
      <div v-if="isLoading" class="loading-overlay">
        <BaseSpinbox size="md" />
      </div>
      <div v-if="hasError" class="error-overlay">
        <p>图片加载失败</p>
      </div>
    </div>

    <!-- 底部操作栏 -->
    <div class="image-actions" v-if="showActions && !hasError">
      <BaseButton
        variant="primary"
        size="sm"
        @click="handleSave"
        class="action-btn"
      >
        <span>💾</span>
        另存为
      </BaseButton>
      <BaseButton
        variant="default"
        size="sm"
        @click="handleCopy"
        class="action-btn"
      >
        <span>📋</span>
        复制
      </BaseButton>
    </div>
  </div>
</template>

<script>
import { ref, computed, watch } from 'vue'
import BaseButton from './BaseButton.vue'
import BaseSpinbox from './BaseSpinbox.vue'
import { toastService } from '../../common/services/toastService'

export default {
  name: 'BaseImageView',
  components: {
    BaseButton,
    BaseSpinbox
  },
  props: {
    src: {
      type: String,
      required: true
    },
    alt: {
      type: String,
      default: '图片'
    },
    showToolbar: {
      type: Boolean,
      default: true
    },
    showActions: {
      type: Boolean,
      default: true
    },
    fileName: {
      type: String,
      default: null
    }
  },
  emits: ['close', 'save', 'copy'],
  setup(props, { emit }) {
    const imageRef = ref(null)
    const scale = ref(1)
    const isFullscreen = ref(false)
    const isLoading = ref(true)
    const hasError = ref(false)
    const imageWidth = ref(0)
    const imageHeight = ref(0)

    const imageStyle = computed(() => ({
      transform: `scale(${scale.value})`,
      transformOrigin: 'center center'
    }))

    const imageInfo = computed(() => {
      if (!imageWidth.value || !imageHeight.value) return ''
      return `${imageWidth.value} × ${imageHeight.value}`
    })

    const handleImageLoad = (e) => {
      isLoading.value = false
      hasError.value = false
      imageWidth.value = e.target.naturalWidth
      imageHeight.value = e.target.naturalHeight
    }

    const handleImageError = () => {
      isLoading.value = false
      hasError.value = true
    }

    const handleZoomIn = () => {
      scale.value = Math.min(scale.value + 0.25, 3)
    }

    const handleZoomOut = () => {
      scale.value = Math.max(scale.value - 0.25, 0.25)
    }

    const handleReset = () => {
      scale.value = 1
    }

    const handleWheel = (e) => {
      const delta = e.deltaY > 0 ? -0.1 : 0.1
      scale.value = Math.min(Math.max(scale.value + delta, 0.25), 3)
    }

    const toggleFullscreen = () => {
      isFullscreen.value = !isFullscreen.value
    }

    const handleClose = () => {
      emit('close')
    }

    const handleSave = async () => {
      try {
        const response = await fetch(props.src)
        const blob = await response.blob()

        const link = document.createElement('a')
        link.href = URL.createObjectURL(blob)

        const name = props.fileName || `image_${new Date().toISOString().slice(0, 19).replace(/[-:]/g, '')}.png`
        link.download = name
        link.click()

        URL.revokeObjectURL(link.href)
        emit('save')
        toastService.success('图片已保存', '成功')
      } catch (error) {
        console.error('保存失败:', error)
        toastService.error('保存失败: ' + error.message, '错误')
      }
    }

    const handleCopy = async () => {
      try {
        const response = await fetch(props.src)
        const blob = await response.blob()

        await navigator.clipboard.write([
          new ClipboardItem({
            'image/png': blob
          })
        ])

        emit('copy')
        toastService.success('已复制到剪贴板', '成功')
      } catch (error) {
        console.error('复制失败:', error)
        toastService.error('复制失败: ' + error.message, '错误')
      }
    }

    watch(() => props.src, () => {
      isLoading.value = true
      hasError.value = false
      scale.value = 1
    })

    return {
      imageRef,
      scale,
      isFullscreen,
      isLoading,
      hasError,
      imageWidth,
      imageHeight,
      imageStyle,
      imageInfo,
      handleImageLoad,
      handleImageError,
      handleZoomIn,
      handleZoomOut,
      handleReset,
      handleWheel,
      toggleFullscreen,
      handleClose,
      handleSave,
      handleCopy
    }
  }
}
</script>

<style scoped>
.base-image-view {
  display: flex;
  flex-direction: column;
  height: 100%;
  background: var(--bg-primary);
  border-radius: var(--radius-md);
  overflow: hidden;
}

.base-image-view.fullscreen {
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  z-index: 1000;
  border-radius: 0;
}

.image-toolbar {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 12px 16px;
  background: var(--bg-elevated);
  border-bottom: 1px solid var(--border-color);
  gap: 16px;
}

.toolbar-left {
  display: flex;
  align-items: center;
  gap: 12px;
}

.close-btn {
  min-width: auto;
  padding: 4px 8px;
}

.image-info {
  font-size: 13px;
  color: var(--text-secondary);
}

.toolbar-right {
  display: flex;
  align-items: center;
  gap: 8px;
}

.scale-info {
  font-size: 12px;
  color: var(--text-secondary);
  min-width: 50px;
  text-align: center;
}

.image-container {
  flex: 1;
  display: flex;
  align-items: center;
  justify-content: center;
  overflow: auto;
  background: var(--bg-secondary);
  position: relative;
}

.image-container img {
  max-width: 100%;
  max-height: 100%;
  object-fit: contain;
  transition: transform 0.2s ease;
}

.loading-overlay,
.error-overlay {
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  display: flex;
  align-items: center;
  justify-content: center;
  background: var(--bg-secondary);
}

.error-overlay p {
  color: var(--danger);
  font-size: 14px;
}

.image-actions {
  display: flex;
  justify-content: center;
  gap: 12px;
  padding: 16px;
  background: var(--bg-elevated);
  border-top: 1px solid var(--border-color);
}

.action-btn {
  display: flex;
  align-items: center;
  gap: 6px;
  min-width: 100px;
}
</style>
