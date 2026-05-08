/**
 * 屏幕截图服务
 * 处理截图相关的业务逻辑
 */

class ScreenCaptureService {
  constructor() {
    this.isCapturing = false
    this.captureType = 'fullscreen'
    this.selectedRegion = null
    this.selectedWindow = null
  }

  /**
   * 设置截图类型
   * @param {string} type - 截图类型: fullscreen, window, region
   */
  setCaptureType(type) {
    this.captureType = type
  }

  /**
   * 获取截图类型
   * @returns {string}
   */
  getCaptureType() {
    return this.captureType
  }

  /**
   * 开始截图流程
   * @returns {Promise<Object>}
   */
  async startCapture() {
    if (this.isCapturing) {
      throw new Error('截图正在进行中')
    }

    this.isCapturing = true

    try {
      switch (this.captureType) {
        case 'fullscreen':
          return await this.captureFullscreen()
        case 'window':
          return await this.captureWindow()
        case 'region':
          return await this.captureRegion()
        default:
          throw new Error('未知的截图类型')
      }
    } finally {
      this.isCapturing = false
    }
  }

  /**
   * 全屏截图
   * @returns {Promise<Object>}
   */
  async captureFullscreen() {
    try {
      const stream = await navigator.mediaDevices.getDisplayMedia({
        video: {
          cursor: 'always',
          displaySurface: 'monitor'
        },
        audio: false
      })

      const imageData = await this.captureStreamFrame(stream)
      stream.getTracks().forEach(track => track.stop())

      return {
        type: 'fullscreen',
        imageData,
        timestamp: Date.now()
      }
    } catch (error) {
      console.error('全屏截图失败:', error)
      throw error
    }
  }

  /**
   * 窗口截图
   * @returns {Promise<Object>}
   */
  async captureWindow() {
    try {
      // 获取屏幕流
      const stream = await navigator.mediaDevices.getDisplayMedia({
        video: {
          cursor: 'always',
          displaySurface: 'window'
        },
        audio: false
      })

      const imageData = await this.captureStreamFrame(stream)
      stream.getTracks().forEach(track => track.stop())

      return {
        type: 'window',
        imageData,
        timestamp: Date.now(),
        windowInfo: this.selectedWindow
      }
    } catch (error) {
      console.error('窗口截图失败:', error)
      throw error
    }
  }

  /**
   * 区域截图
   * @param {Object} region - 区域坐标 {x, y, width, height}
   * @returns {Promise<Object>}
   */
  async captureRegion(region) {
    if (!region || !this.isValidRegion(region)) {
      throw new Error('无效的区域选择')
    }

    try {
      const stream = await navigator.mediaDevices.getDisplayMedia({
        video: {
          cursor: 'always',
          displaySurface: 'monitor'
        },
        audio: false
      })

      const imageData = await this.captureRegionFromStream(stream, region)
      stream.getTracks().forEach(track => track.stop())

      return {
        type: 'region',
        imageData,
        timestamp: Date.now(),
        region
      }
    } catch (error) {
      console.error('区域截图失败:', error)
      throw error
    }
  }

  /**
   * 从媒体流捕获完整帧
   * @param {MediaStream} stream
   * @returns {Promise<string>}
   */
  captureStreamFrame(stream) {
    return new Promise((resolve, reject) => {
      const video = document.createElement('video')
      video.srcObject = stream
      video.onloadedmetadata = () => {
        video.play()

        const canvas = document.createElement('canvas')
        canvas.width = video.videoWidth
        canvas.height = video.videoHeight
        const ctx = canvas.getContext('2d')

        ctx.drawImage(video, 0, 0, canvas.width, canvas.height)

        resolve(canvas.toDataURL('image/png'))
      }
      video.onerror = reject
    })
  }

  /**
   * 从媒体流捕获指定区域
   * @param {MediaStream} stream
   * @param {Object} region - {x, y, width, height}
   * @returns {Promise<string>}
   */
  captureRegionFromStream(stream, region) {
    return new Promise((resolve, reject) => {
      const video = document.createElement('video')
      video.srcObject = stream
      video.onloadedmetadata = () => {
        video.play()

        const canvas = document.createElement('canvas')
        canvas.width = region.width
        canvas.height = region.height
        const ctx = canvas.getContext('2d')

        // 绘制指定区域
        ctx.drawImage(
          video,
          region.x, region.y, region.width, region.height,
          0, 0, region.width, region.height
        )

        resolve(canvas.toDataURL('image/png'))
      }
      video.onerror = reject
    })
  }

  /**
   * 验证区域是否有效
   * @param {Object} region
   * @returns {boolean}
   */
  isValidRegion(region) {
    return (
      region.x !== undefined &&
      region.y !== undefined &&
      region.width > 0 &&
      region.height > 0
    )
  }

  /**
   * 计算区域的包围盒
   * @param {Array} points - 点数组 [{x, y}, ...]
   * @returns {Object} - {x, y, width, height}
   */
  calculateBoundingBox(points) {
    if (!points || points.length === 0) {
      return null
    }

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

  /**
   * 设置选中的区域
   * @param {Object} region
   */
  setSelectedRegion(region) {
    this.selectedRegion = region
  }

  /**
   * 获取选中的区域
   * @returns {Object}
   */
  getSelectedRegion() {
    return this.selectedRegion
  }

  /**
   * 清除选中的区域
   */
  clearSelectedRegion() {
    this.selectedRegion = null
  }

  /**
   * 设置选中的窗口
   * @param {Object} window
   */
  setSelectedWindow(window) {
    this.selectedWindow = window
  }

  /**
   * 获取选中的窗口
   * @returns {Object}
   */
  getSelectedWindow() {
    return this.selectedWindow
  }

  /**
   * 清除选中的窗口
   */
  clearSelectedWindow() {
    this.selectedWindow = null
  }

  /**
   * 检查是否正在截图
   * @returns {boolean}
   */
  isCapturingState() {
    return this.isCapturing
  }

  /**
   * 取消截图
   */
  cancelCapture() {
    this.isCapturing = false
    this.clearSelectedRegion()
    this.clearSelectedWindow()
  }
}

// 创建单例实例
const screenCaptureService = new ScreenCaptureService()

export { ScreenCaptureService, screenCaptureService }
export default screenCaptureService
