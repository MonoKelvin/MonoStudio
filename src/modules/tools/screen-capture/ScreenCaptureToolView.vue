<template>
    <BasePanel class="panel screen-capture-panel">
        <div class="screen-capture-container">
            <!-- 截图选项 -->
            <div class="capture-options">
                <div class="option-group">
                    <span class="option-label">截图类型</span>
                    <div class="radio-group">
                        <label v-for="option in captureOptions" :key="option.value" class="radio-item"
                            :class="{ active: captureType === option.value }">
                            <input type="radio" v-model="captureType" :value="option.value" :disabled="isCapturing" />
                            <BaseSvgIcon :icon="option.icon" class="radio-icon" />
                            <span>{{ option.label }}</span>
                        </label>
                    </div>
                </div>

                <BaseButton variant="primary" size="lg" @click="startCapture" :disabled="isCapturing"
                    class="capture-btn">
                    <BaseSvgIcon :icon="isCapturing ? refreshIcon : cameraIcon" class="btn-icon" :spin="isCapturing" />
                    {{ isCapturing ? '截图中...' : '开始截图' }}
                </BaseButton>
            </div>

            <!-- 截图结果 -->
            <div v-if="capturedImage" class="capture-result">
                <BaseImageView :src="capturedImage" :show-actions="true" :file-name="generateFileName()"
                    @close="clearImage" />
            </div>

            <!-- 空状态 -->
            <div v-else class="capture-empty">
                <BaseSvgIcon :icon="imageIcon" class="empty-icon" />
                <p class="empty-text">选择截图类型开始截取</p>
            </div>

            <!-- 截图遮罩层 -->
            <CaptureOverlay :is-visible="showOverlay" :capture-type="captureType" @select="handleRegionSelect"
                @cancel="handleCaptureCancel" />
        </div>
    </BasePanel>
</template>

<script>
import { ref, computed, watch } from 'vue'
import BasePanel from '../../../components/base/BasePanel.vue'
import BaseButton from '../../../components/base/BaseButton.vue'
import BaseImageView from '../../../components/base/BaseImageView.vue'
import BaseSvgIcon from '../../../components/base/BaseSvgIcon.vue'
import CaptureOverlay from './components/CaptureOverlay.vue'
import { screenCaptureService } from './services/ScreenCaptureService'
import { toastService } from '../../../common/services/toastService'

// 图标
import fullscreenIcon from '../../../assets/icons/fullscreen.svg?raw'
import windowIcon from '../../../assets/icons/window.svg?raw'
import cropIcon from '../../../assets/icons/crop.svg?raw'
import cameraIcon from '../../../assets/icons/camera.svg?raw'
import imageIcon from '../../../assets/icons/image.svg?raw'
import refreshIcon from '../../../assets/icons/refresh.svg?raw'

export default {
    name: 'ScreenCaptureToolView',
    components: {
        BasePanel,
        BaseButton,
        BaseImageView,
        BaseSvgIcon,
        CaptureOverlay
    },
    props: {
        toolName: {
            type: String,
            default: '屏幕截图'
        }
    },
    setup() {
        // 状态
        const captureType = ref('fullscreen')
        const capturedImage = ref(null)
        const isCapturing = ref(false)
        const showOverlay = ref(false)

        // 截图选项配置
        const captureOptions = computed(() => [
            { value: 'fullscreen', label: '全屏', icon: fullscreenIcon },
            { value: 'window', label: '窗口', icon: windowIcon },
            { value: 'region', label: '区域', icon: cropIcon }
        ])

        // 监听截图类型变化
        watch(captureType, (newType) => {
            screenCaptureService.setCaptureType(newType)
        })

        // 开始截图
        const startCapture = async () => {
            if (isCapturing.value) return

            try {
                isCapturing.value = true

                if (captureType.value === 'fullscreen') {
                    // 全屏截图直接执行
                    const result = await screenCaptureService.captureFullscreen()
                    capturedImage.value = result.imageData
                    toastService.success('截图成功', '成功')
                } else {
                    // 窗口和区域截图显示遮罩层
                    showOverlay.value = true
                }
            } catch (error) {
                console.error('截图失败:', error)
                if (error.name !== 'NotAllowedError') {
                    toastService.error('截图失败: ' + error.message, '错误')
                }
            } finally {
                if (captureType.value === 'fullscreen') {
                    isCapturing.value = false
                }
            }
        }

        // 处理区域选择
        const handleRegionSelect = async (region) => {
            showOverlay.value = false

            try {
                const result = await screenCaptureService.captureRegion(region)
                capturedImage.value = result.imageData
                toastService.success('截图成功', '成功')
            } catch (error) {
                console.error('截图失败:', error)
                toastService.error('截图失败: ' + error.message, '错误')
            } finally {
                isCapturing.value = false
            }
        }

        // 取消截图
        const handleCaptureCancel = () => {
            showOverlay.value = false
            isCapturing.value = false
            screenCaptureService.cancelCapture()
        }

        // 清除图片
        const clearImage = () => {
            capturedImage.value = null
        }

        // 生成文件名
        const generateFileName = () => {
            return `screenshot_${new Date().toISOString().slice(0, 19).replace(/[-:]/g, '')}.png`
        }

        return {
            // 状态
            captureType,
            capturedImage,
            isCapturing,
            showOverlay,
            captureOptions,
            // 图标
            fullscreenIcon,
            windowIcon,
            cropIcon,
            cameraIcon,
            imageIcon,
            refreshIcon,
            // 方法
            startCapture,
            handleRegionSelect,
            handleCaptureCancel,
            clearImage,
            generateFileName
        }
    }
}
</script>

<style scoped>
.screen-capture-panel {
    height: 100%;
    display: flex;
    flex-direction: column;
}

.screen-capture-container {
    flex: 1;
    display: flex;
    flex-direction: column;
    padding: 16px;
    gap: 16px;
}

/* 截图选项 */
.capture-options {
    display: flex;
    flex-direction: column;
    gap: 16px;
    padding: 16px;
    background: var(--bg-elevated);
    border-radius: var(--radius-md);
}

.option-group {
    display: flex;
    flex-direction: column;
    gap: 12px;
}

.option-label {
    font-size: 14px;
    font-weight: 500;
    color: var(--text-primary);
}

.radio-group {
    display: flex;
    gap: 12px;
    flex-wrap: wrap;
}

.radio-item {
    display: flex;
    align-items: center;
    gap: 8px;
    padding: 10px 16px;
    background: var(--bg-secondary);
    border-radius: var(--radius-md);
    cursor: pointer;
    transition: all 0.2s ease;
    border: 2px solid transparent;
}

.radio-item:hover {
    background: var(--bg-tertiary);
}

.radio-item.active {
    background: color-mix(in srgb, var(--accent-color) 10%, var(--bg-secondary));
    border-color: var(--accent-color);
}

.radio-item input[type="radio"] {
    display: none;
}

.radio-item span {
    font-size: 14px;
    color: var(--text-primary);
}

.radio-icon {
    width: 16px;
    height: 16px;
}

.capture-btn {
    align-self: center;
    min-width: 160px;
    padding: 12px 24px;
    font-size: 15px;
}

.btn-icon {
    width: 18px;
    height: 18px;
    margin-right: 8px;
}

/* 截图结果 */
.capture-result {
    flex: 1;
    border-radius: var(--radius-md);
    overflow: hidden;
    border: 1px solid var(--border-color);
    min-height: 200px;
}

/* 空状态 */
.capture-empty {
    flex: 1;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    gap: 12px;
    padding: 40px;
    background: var(--bg-elevated);
    border-radius: var(--radius-md);
}

.empty-icon {
    width: 48px;
    height: 48px;
    color: var(--text-tertiary);
}

.empty-text {
    margin: 0;
    font-size: 14px;
    color: var(--text-secondary);
}

@media (max-width: 600px) {
    .radio-group {
        flex-direction: column;
    }

    .radio-item {
        justify-content: center;
    }
}
</style>
