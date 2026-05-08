<template>
    <Teleport to="body">
        <div v-if="isVisible" class="capture-overlay" @click="handleOverlayClick" @contextmenu.prevent>
            <!-- 暗色背景层 -->
            <div class="overlay-background"></div>

            <!-- 画布层 -->
            <canvas ref="canvasRef" class="overlay-canvas"></canvas>

            <!-- 十字线 -->
            <div v-if="showCrosshair" class="crosshair" :style="crosshairStyle">
                <div class="crosshair-line crosshair-horizontal"></div>
                <div class="crosshair-line crosshair-vertical"></div>
                <div class="crosshair-center"></div>
            </div>

            <!-- 操作提示 -->
            <div class="capture-hints">
                <span class="hint-text">拖拽选择区域</span>
                <span class="hint-divider">|</span>
                <span class="hint-text">ESC 取消</span>
                <span class="hint-divider">|</span>
                <span class="hint-text">双击确认</span>
            </div>
        </div>
    </Teleport>
</template>

<script>
import { ref, computed, watch, onMounted, onUnmounted } from 'vue'

export default {
    name: 'CaptureOverlay',
    props: {
        isVisible: {
            type: Boolean,
            default: false
        },
        captureType: {
            type: String,
            default: 'region'
        }
    },
    emits: ['select', 'cancel'],
    setup(props, { emit }) {
        const canvasRef = ref(null)
        const isDragging = ref(false)
        const showCrosshair = ref(true)
        const mousePosition = ref({ x: 0, y: 0 })
        const startPosition = ref({ x: 0, y: 0 })
        const selectionRect = ref(null)
        const drawnPoints = ref([])

        // 十字线样式
        const crosshairStyle = computed(() => ({
            left: `${mousePosition.value.x}px`,
            top: `${mousePosition.value.y}px`
        }))

        // 获取主题色
        const getAccentColor = () => {
            return getComputedStyle(document.documentElement).getPropertyValue('--accent-color') || '#4f46e5'
        }

        // 初始化画布
        const initCanvas = () => {
            const canvas = canvasRef.value
            if (!canvas) return

            canvas.width = window.innerWidth
            canvas.height = window.innerHeight
        }

        // 清空画布
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
            if (!canvas) return

            const ctx = canvas.getContext('2d')
            const accentColor = getAccentColor()

            // 绘制暗色背景（通过div实现，这里只绘制选区）
            if (selectionRect.value) {
                const { x, y, width, height } = selectionRect.value

                // 清除选区区域显示原始内容
                ctx.clearRect(x, y, width, height)

                // 绘制选区边框
                ctx.strokeStyle = accentColor
                ctx.lineWidth = 2
                ctx.strokeRect(x, y, width, height)

                // 绘制角落标记
                const cornerSize = 10
                ctx.fillStyle = accentColor

                // 左上角
                ctx.fillRect(x - 2, y - 2, cornerSize, 2)
                ctx.fillRect(x - 2, y - 2, 2, cornerSize)
                // 右上角
                ctx.fillRect(x + width - cornerSize, y - 2, cornerSize, 2)
                ctx.fillRect(x + width - 2, y - 2, 2, cornerSize)
                // 左下角
                ctx.fillRect(x - 2, y + height - cornerSize, cornerSize, 2)
                ctx.fillRect(x - 2, y + height - 2, 2, cornerSize)
                // 右下角
                ctx.fillRect(x + width - cornerSize, y + height - cornerSize, cornerSize, 2)
                ctx.fillRect(x + width - 2, y + height - 2, 2, cornerSize)
            }

            // 如果是自由绘制模式，绘制路径
            if (drawnPoints.value.length > 1) {
                ctx.beginPath()
                ctx.moveTo(drawnPoints.value[0].x, drawnPoints.value[0].y)

                for (let i = 1; i < drawnPoints.value.length; i++) {
                    ctx.lineTo(drawnPoints.value[i].x, drawnPoints.value[i].y)
                }

                ctx.strokeStyle = accentColor
                ctx.lineWidth = 2
                ctx.stroke()

                // 计算并显示包围盒
                const bounds = calculateBoundingBox(drawnPoints.value)
                if (bounds) {
                    ctx.strokeStyle = accentColor
                    ctx.lineWidth = 1
                    ctx.setLineDash([5, 5])
                    ctx.strokeRect(bounds.x, bounds.y, bounds.width, bounds.height)
                    ctx.setLineDash([])
                }
            }
        }

        // 计算包围盒
        const calculateBoundingBox = (points) => {
            if (!points || points.length < 2) return null

            let minX = points[0].x
            let minY = points[0].y
            let maxX = points[0].x
            let maxY = points[0].y

            for (const point of points) {
                minX = Math.min(minX, point.x)
                minY = Math.min(minY, point.y)
                maxX = Math.max(maxX, point.x)
                maxY = Math.max(maxY, point.y)
            }

            return {
                x: minX,
                y: minY,
                width: maxX - minX,
                height: maxY - minY
            }
        }

        // 鼠标移动处理
        const handleMouseMove = (e) => {
            mousePosition.value = { x: e.clientX, y: e.clientY }

            if (isDragging.value) {
                const width = Math.abs(e.clientX - startPosition.value.x)
                const height = Math.abs(e.clientY - startPosition.value.y)
                const x = Math.min(e.clientX, startPosition.value.x)
                const y = Math.min(e.clientY, startPosition.value.y)

                selectionRect.value = { x, y, width, height }
                redrawCanvas()
            }
        }

        // 鼠标按下处理
        const handleMouseDown = (e) => {
            if (e.button !== 0) return // 只响应左键

            isDragging.value = true
            startPosition.value = { x: e.clientX, y: e.clientY }
            selectionRect.value = null
            drawnPoints.value = []

            if (props.captureType === 'region') {
                // 自由绘制模式
                drawnPoints.value.push({ x: e.clientX, y: e.clientY })
            }
        }

        // 鼠标松开处理
        const handleMouseUp = (e) => {
            if (!isDragging.value) return

            isDragging.value = false

            if (props.captureType === 'region' && drawnPoints.value.length > 1) {
                // 自由绘制模式：计算包围盒
                const bounds = calculateBoundingBox(drawnPoints.value)
                if (bounds && bounds.width > 10 && bounds.height > 10) {
                    emit('select', bounds)
                }
            } else if (selectionRect.value && selectionRect.value.width > 10 && selectionRect.value.height > 10) {
                // 矩形选择模式
                emit('select', selectionRect.value)
            }

            selectionRect.value = null
            drawnPoints.value = []
        }

        // 双击处理
        const handleDoubleClick = () => {
            if (selectionRect.value && selectionRect.value.width > 10 && selectionRect.value.height > 10) {
                emit('select', selectionRect.value)
            } else if (drawnPoints.value.length > 1) {
                const bounds = calculateBoundingBox(drawnPoints.value)
                if (bounds && bounds.width > 10 && bounds.height > 10) {
                    emit('select', bounds)
                }
            }
        }

        // 遮罩层点击处理
        const handleOverlayClick = (e) => {
            // 如果点击的是遮罩层本身（不是canvas或crosshair），取消截图
            if (e.target === e.currentTarget) {
                handleCancel()
            }
        }

        // 键盘事件处理
        const handleKeyUp = (e) => {
            if (e.key === 'Escape') {
                handleCancel()
            }
        }

        // 取消截图
        const handleCancel = () => {
            clearCanvas()
            selectionRect.value = null
            drawnPoints.value = []
            emit('cancel')
        }

        // 监听可见性变化
        watch(() => props.isVisible, (visible) => {
            if (visible) {
                showCrosshair.value = true
                initCanvas()
                window.addEventListener('mousemove', handleMouseMove)
                window.addEventListener('mousedown', handleMouseDown)
                window.addEventListener('mouseup', handleMouseUp)
                window.addEventListener('dblclick', handleDoubleClick)
                window.addEventListener('keyup', handleKeyUp)
                window.addEventListener('resize', initCanvas)
            } else {
                showCrosshair.value = false
                clearCanvas()
                window.removeEventListener('mousemove', handleMouseMove)
                window.removeEventListener('mousedown', handleMouseDown)
                window.removeEventListener('mouseup', handleMouseUp)
                window.removeEventListener('dblclick', handleDoubleClick)
                window.removeEventListener('keyup', handleKeyUp)
                window.removeEventListener('resize', initCanvas)
            }
        })

        onMounted(() => {
            if (props.isVisible) {
                initCanvas()
            }
        })

        onUnmounted(() => {
            window.removeEventListener('mousemove', handleMouseMove)
            window.removeEventListener('mousedown', handleMouseDown)
            window.removeEventListener('mouseup', handleMouseUp)
            window.removeEventListener('dblclick', handleDoubleClick)
            window.removeEventListener('keyup', handleKeyUp)
            window.removeEventListener('resize', initCanvas)
        })

        return {
            canvasRef,
            showCrosshair,
            mousePosition,
            crosshairStyle,
            handleMouseMove,
            handleMouseDown,
            handleMouseUp,
            handleDoubleClick,
            handleOverlayClick,
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
    overflow: hidden;
}

.overlay-background {
    position: absolute;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background: rgba(0, 0, 0, 0.5);
}

.overlay-canvas {
    position: absolute;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    pointer-events: none;
}

/* 十字线样式 */
.crosshair {
    position: absolute;
    pointer-events: none;
    transform: translate(-50%, -50%);
    z-index: 10;
}

.crosshair-line {
    position: absolute;
    background: var(--accent-color);
}

.crosshair-horizontal {
    width: 100vw;
    height: 1px;
    top: 0;
    left: -50vw;
}

.crosshair-vertical {
    width: 1px;
    height: 100vh;
    top: -50vh;
    left: 0;
}

.crosshair-center {
    position: absolute;
    width: 12px;
    height: 12px;
    top: -6px;
    left: -6px;
    border: 2px solid var(--accent-color);
    border-radius: 50%;
    background: rgba(0, 0, 0, 0.3);
}

/* 操作提示 */
.capture-hints {
    position: absolute;
    bottom: 32px;
    left: 50%;
    transform: translateX(-50%);
    display: flex;
    align-items: center;
    gap: 12px;
    padding: 10px 20px;
    background: rgba(0, 0, 0, 0.7);
    border-radius: 20px;
    backdrop-filter: blur(10px);
}

.hint-text {
    font-size: 13px;
    color: rgba(255, 255, 255, 0.8);
}

.hint-divider {
    color: rgba(255, 255, 255, 0.4);
}
</style>
