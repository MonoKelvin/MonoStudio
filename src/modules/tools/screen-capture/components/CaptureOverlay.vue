<template>
    <div class="capture-overlay" v-if="isVisible" @mousemove="handleMouseMove" @mousedown="handleMouseDown"
        @mouseup="handleMouseUp" @keyup="handleKeyUp" tabindex="0">
        <!-- 暗色背景画布 -->
        <canvas ref="canvasRef" class="capture-canvas"></canvas>

        <!-- 十字线 -->
        <div class="crosshair" :style="crosshairStyle">
            <div class="crosshair-h"></div>
            <div class="crosshair-v"></div>
        </div>

        <!-- 工具栏 -->
        <div class="overlay-toolbar">
            <div class="toolbar-hint">
                <span v-if="captureType === 'region'">拖动绘制区域，按 ESC 取消</span>
                <span v-else-if="captureType === 'window'">点击窗口进行选择，按 ESC 取消</span>
                <span v-else>拖动选择区域，按 ESC 取消</span>
            </div>
            <BaseButton size="sm" variant="default" @click="handleCancel">
                取消
            </BaseButton>
        </div>

        <!-- 尺寸提示 -->
        <div v-if="selectionRect" class="size-tooltip" :style="tooltipStyle">
            {{ selectionRect.width }} × {{ selectionRect.height }}
        </div>
    </div>
</template>

<script>
import { ref, computed, onMounted, onUnmounted, nextTick } from 'vue'
import BaseButton from '../../../../components/base/BaseButton.vue'

export default {
    name: 'CaptureOverlay',
    components: {
        BaseButton
    },
    props: {
        isVisible: {
            type: Boolean,
            default: false
        },
        captureType: {
            type: String,
            default: 'fullscreen'
        }
    },
    emits: ['select', 'cancel', 'window-select'],
    setup(props, { emit }) {
        const canvasRef = ref(null)
        const mouseX = ref(0)
        const mouseY = ref(0)
        const isDrawing = ref(false)
        const startPoint = ref({ x: 0, y: 0 })
        const selectionRect = ref(null)
        const drawnPoints = ref([])

        // 十字线样式
        const crosshairStyle = computed(() => ({
            left: `${mouseX.value}px`,
            top: `${mouseY.value}px`
        }))

        // 尺寸提示样式
        const tooltipStyle = computed(() => {
            if (!selectionRect.value) return {}
            return {
                left: `${selectionRect.value.x + selectionRect.value.width / 2}px`,
                top: `${selectionRect.value.y - 30}px`
            }
        })

        // 初始化画布
        const initCanvas = () => {
            const canvas = canvasRef.value
            if (!canvas) return

            canvas.width = window.innerWidth
            canvas.height = window.innerHeight

            const ctx = canvas.getContext('2d')
            // 绘制半透明黑色背景
            ctx.fillStyle = 'rgba(0, 0, 0, 0.5)'
            ctx.fillRect(0, 0, canvas.width, canvas.height)
        }

        // 清除画布
        const clearCanvas = () => {
            const canvas = canvasRef.value
            if (!canvas) return
            const ctx = canvas.getContext('2d')
            ctx.clearRect(0, 0, canvas.width, canvas.height)
        }

        // 重绘画布
        const redrawCanvas = () => {
            clearCanvas()
            const canvas = canvasRef.value
            const ctx = canvas.getContext('2d')

            // 绘制半透明背景
            ctx.fillStyle = 'rgba(0, 0, 0, 0.5)'
            ctx.fillRect(0, 0, canvas.width, canvas.height)

            // 如果有选中区域，清除该区域的遮罩（使其变亮）
            if (selectionRect.value) {
                ctx.clearRect(
                    selectionRect.value.x,
                    selectionRect.value.y,
                    selectionRect.value.width,
                    selectionRect.value.height
                )

                // 绘制选中区域的边框
                ctx.strokeStyle = 'var(--accent-color)'
                ctx.lineWidth = 2
                ctx.strokeRect(
                    selectionRect.value.x,
                    selectionRect.value.y,
                    selectionRect.value.width,
                    selectionRect.value.height
                )
            }

            // 如果有绘制的点（区域截图模式）
            if (drawnPoints.value.length > 0) {
                ctx.beginPath()
                ctx.moveTo(drawnPoints.value[0].x, drawnPoints.value[0].y)
                for (let i = 1; i < drawnPoints.value.length; i++) {
                    ctx.lineTo(drawnPoints.value[i].x, drawnPoints.value[i].y)
                }
                ctx.strokeStyle = 'var(--accent-color)'
                ctx.lineWidth = 2
                ctx.stroke()
            }
        }

        // 鼠标移动处理
        const handleMouseMove = (e) => {
            mouseX.value = e.clientX
            mouseY.value = e.clientY

            if (isDrawing.value) {
                const width = e.clientX - startPoint.value.x
                const height = e.clientY - startPoint.value.y

                selectionRect.value = {
                    x: width > 0 ? startPoint.value.x : e.clientX,
                    y: height > 0 ? startPoint.value.y : e.clientY,
                    width: Math.abs(width),
                    height: Math.abs(height)
                }

                redrawCanvas()
            }

            // 窗口模式下高亮检测到的窗口
            if (props.captureType === 'window') {
                detectWindowAt(e.clientX, e.clientY)
            }
        }

        // 鼠标按下处理
        const handleMouseDown = (e) => {
            if (e.button !== 0) return // 只处理左键

            isDrawing.value = true
            startPoint.value = { x: e.clientX, y: e.clientY }

            if (props.captureType === 'region') {
                drawnPoints.value = [{ x: e.clientX, y: e.clientY }]
            }
        }

        // 鼠标松开处理
        const handleMouseUp = (e) => {
            if (!isDrawing.value) return

            isDrawing.value = false

            if (props.captureType === 'region') {
                // 区域截图：计算包围盒
                const boundingBox = calculateBoundingBox(drawnPoints.value)
                if (boundingBox && boundingBox.width > 10 && boundingBox.height > 10) {
                    emit('select', boundingBox)
                }
            } else if (selectionRect.value &&
                selectionRect.value.width > 10 &&
                selectionRect.value.height > 10) {
                // 矩形截图
                emit('select', selectionRect.value)
            }

            selectionRect.value = null
            drawnPoints.value = []
        }

        // 计算包围盒
        const calculateBoundingBox = (points) => {
            if (!points || points.length < 2) return null

            const xs = points.map(p => p.x)
            const ys = points.map(p => p.y)

            const minX = Math.min(...xs)
            const maxX = Math.max(...xs)
            const minY = Math.min(...ys)
            const maxY = Math.max(...ys)

            return {
                x: minX,
                y: minY,
                width: maxX - minX,
                height: maxY - minY
            }
        }

        // 检测窗口
        const detectWindowAt = (x, y) => {
            // 这里需要调用 Electron API 获取窗口信息
            // 暂时模拟窗口检测
            const mockWindow = {
                x: Math.floor(x / 100) * 100,
                y: Math.floor(y / 100) * 100,
                width: 400,
                height: 300,
                title: '窗口'
            }

            // 高亮检测到的窗口
            const canvas = canvasRef.value
            const ctx = canvas.getContext('2d')

            redrawCanvas()

            // 绘制窗口高亮边框
            ctx.strokeStyle = 'var(--accent-color)'
            ctx.lineWidth = 3
            ctx.strokeRect(mockWindow.x, mockWindow.y, mockWindow.width, mockWindow.height)

            // 清除窗口内部的遮罩
            ctx.clearRect(mockWindow.x, mockWindow.y, mockWindow.width, mockWindow.height)
        }

        // 取消截图
        const handleCancel = () => {
            isDrawing.value = false
            selectionRect.value = null
            drawnPoints.value = []
            emit('cancel')
        }

        // 键盘事件处理
        const handleKeyUp = (e) => {
            if (e.key === 'Escape') {
                handleCancel()
            }
        }

        onMounted(() => {
            nextTick(() => {
                initCanvas()
                window.addEventListener('keyup', handleKeyUp)
            })
        })

        onUnmounted(() => {
            window.removeEventListener('keyup', handleKeyUp)
        })

        return {
            canvasRef,
            mouseX,
            mouseY,
            selectionRect,
            crosshairStyle,
            tooltipStyle,
            handleMouseMove,
            handleMouseDown,
            handleMouseUp,
            handleKeyUp,
            handleCancel
        }
    }
}
</script>

<style scoped>
.capture-overlay {
    position: fixed;
    top: 0;
    left: 0;
    width: 100vw;
    height: 100vh;
    z-index: 99999;
    cursor: crosshair;
    outline: none;
}

.capture-canvas {
    position: absolute;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
}

.crosshair {
    position: absolute;
    pointer-events: none;
    transform: translate(-50%, -50%);
}

.crosshair-h {
    position: absolute;
    width: 100vw;
    height: 1px;
    background: var(--accent-color);
    left: 50%;
    top: 50%;
    transform: translate(-50%, -50%);
}

.crosshair-v {
    position: absolute;
    width: 1px;
    height: 100vh;
    background: var(--accent-color);
    left: 50%;
    top: 50%;
    transform: translate(-50%, -50%);
}

.overlay-toolbar {
    position: fixed;
    bottom: 40px;
    left: 50%;
    transform: translateX(-50%);
    display: flex;
    align-items: center;
    gap: 16px;
    padding: 12px 24px;
    background: var(--bg-elevated);
    border-radius: var(--radius-md);
    box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3);
    z-index: 100000;
}

.toolbar-hint {
    font-size: 13px;
    color: var(--text-secondary);
}

.size-tooltip {
    position: absolute;
    padding: 4px 8px;
    background: var(--accent-color);
    color: white;
    font-size: 12px;
    border-radius: var(--radius-sm);
    pointer-events: none;
    transform: translate(-50%, 0);
    z-index: 100001;
}
</style>
