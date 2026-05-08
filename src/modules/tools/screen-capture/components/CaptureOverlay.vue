<template>
    <div class="capture-overlay" v-if="isVisible" @mousemove="handleMouseMove" @mousedown="handleMouseDown"
        @mouseup="handleMouseUp" @keyup="handleKeyUp" tabindex="0">
        <canvas ref="canvasRef" class="capture-canvas"></canvas>

        <div class="crosshair" :style="crosshairStyle">
            <div class="crosshair-h"></div>
            <div class="crosshair-v"></div>
        </div>

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

        const crosshairStyle = computed(() => ({
            left: `${mouseX.value}px`,
            top: `${mouseY.value}px`
        }))

        const tooltipStyle = computed(() => {
            if (!selectionRect.value) return {}
            return {
                left: `${selectionRect.value.x + selectionRect.value.width / 2}px`,
                top: `${selectionRect.value.y - 30}px`
            }
        })

        const initCanvas = () => {
            const canvas = canvasRef.value
            if (!canvas) return

            canvas.width = window.innerWidth
            canvas.height = window.innerHeight

            const ctx = canvas.getContext('2d')
            ctx.fillStyle = 'rgba(0, 0, 0, 0.5)'
            ctx.fillRect(0, 0, canvas.width, canvas.height)
        }

        const clearCanvas = () => {
            const canvas = canvasRef.value
            if (!canvas) return
            const ctx = canvas.getContext('2d')
            ctx.clearRect(0, 0, canvas.width, canvas.height)
        }

        const redrawCanvas = () => {
            clearCanvas()
            const canvas = canvasRef.value
            const ctx = canvas.getContext('2d')

            ctx.fillStyle = 'rgba(0, 0, 0, 0.5)'
            ctx.fillRect(0, 0, canvas.width, canvas.height)

            if (selectionRect.value) {
                ctx.clearRect(
                    selectionRect.value.x,
                    selectionRect.value.y,
                    selectionRect.value.width,
                    selectionRect.value.height
                )

                ctx.strokeStyle = 'var(--accent-color)'
                ctx.lineWidth = 2
                ctx.strokeRect(
                    selectionRect.value.x,
                    selectionRect.value.y,
                    selectionRect.value.width,
                    selectionRect.value.height
                )
            }

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

            if (props.captureType === 'window') {
                detectWindowAt(e.clientX, e.clientY)
            }
        }

        const handleMouseDown = (e) => {
            if (e.button !== 0) return

            isDrawing.value = true
            startPoint.value = { x: e.clientX, y: e.clientY }

            if (props.captureType === 'region') {
                drawnPoints.value = [{ x: e.clientX, y: e.clientY }]
            }
        }

        const handleMouseUp = (e) => {
            if (!isDrawing.value) return

            isDrawing.value = false

            if (props.captureType === 'region') {
                const boundingBox = calculateBoundingBox(drawnPoints.value)
                if (boundingBox && boundingBox.width > 10 && boundingBox.height > 10) {
                    emit('select', boundingBox)
                }
            } else if (selectionRect.value &&
                selectionRect.value.width > 10 &&
                selectionRect.value.height > 10) {
                emit('select', selectionRect.value)
            }

            selectionRect.value = null
            drawnPoints.value = []
        }

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

        const detectWindowAt = (x, y) => {
            const mockWindow = {
                x: Math.floor(x / 100) * 100,
                y: Math.floor(y / 100) * 100,
                width: 400,
                height: 300,
                title: '窗口'
            }

            const canvas = canvasRef.value
            const ctx = canvas.getContext('2d')

            redrawCanvas()

            ctx.strokeStyle = 'var(--accent-color)'
            ctx.lineWidth = 3
            ctx.strokeRect(mockWindow.x, mockWindow.y, mockWindow.width, mockWindow.height)

            ctx.clearRect(mockWindow.x, mockWindow.y, mockWindow.width, mockWindow.height)
        }

        const handleKeyUp = (e) => {
            if (e.key === 'Escape') {
                handleCancel()
            }
        }

        const handleCancel = () => {
            isDrawing.value = false
            selectionRect.value = null
            drawnPoints.value = []
            emit('cancel')
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
