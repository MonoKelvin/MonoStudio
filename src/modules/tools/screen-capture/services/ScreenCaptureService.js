/**
 * 屏幕截图服务
 * 负责处理截图的核心业务逻辑
 */
class ScreenCaptureService {
  constructor() {
    this.captureType = 'fullscreen'
    this.isCapturing = false
    this.mediaStream = null
    this.videoElement = null
    this.canvas = null
  }

  /**
   * 设置截图类型
   * @param {string} type - 'fullscreen' | 'window' | 'region'
   */
  setCaptureType(type) {
    if (['fullscreen', 'window', 'region'].includes(type)) {
      this.captureType = type
    }
  }

  /**
   * 开始截图
   * @returns {Promise<{ imageData: string, width: number, height: number }>}
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
          throw new Error('区域截图需要先选择区域')
        default:
          throw new Error('未知的截图类型')
      }
    } finally {
      this.isCapturing = false
    }
  }

  /**
   * 全屏截图
   * @returns {Promise<{ imageData: string, width: number, height: number }>}
   */
  async captureFullscreen() {
    try {
      this.mediaStream = await navigator.mediaDevices.getDisplayMedia({
        video: {
          displaySurface: 'monitor',
          logicalSurface: true
        },
        audio: false
      })

      return await this._captureFromStream()
    } finally {
      this._cleanup()
    }
  }

  /**
   * 窗口截图
   * @returns {Promise<{ imageData: string, width: number, height: number }>}
   */
  async captureWindow() {
    try {
      this.mediaStream = await navigator.mediaDevices.getDisplayMedia({
        video: {
          displaySurface: 'window',
          logicalSurface: true
        },
        audio: false
      })

      return await this._captureFromStream()
    } finally {
      this._cleanup()
    }
  }

  /**
   * 区域截图
   * @param {Object} region - 区域对象 { x, y, width, height }
   * @returns {Promise<{ imageData: string, width: number, height: number }>}
   */
  async captureRegion(region) {
    if (!region || !region.width || !region.height) {
      throw new Error('无效的截图区域')
    }

    try {
      this.mediaStream = await navigator.mediaDevices.getDisplayMedia({
        video: {
          displaySurface: 'monitor',
          logicalSurface: true
        },
        audio: false
      })

      const fullResult = await this._captureFromStream()

      // 截取指定区域
      return await this._cropImage(fullResult.imageData, region)
    } finally {
      this._cleanup()
    }
  }

  /**
   * 取消截图
   */
  cancelCapture() {
    this._cleanup()
    this.isCapturing = false
  }

  /**
   * 从媒体流捕获画面
   * @returns {Promise<{ imageData: string, width: number, height: number }>}
   */
  async _captureFromStream() {
    return new Promise((resolve, reject) => {
      this.videoElement = document.createElement('video')
      this.videoElement.autoplay = true
      this.videoElement.muted = true

      this.videoElement.addEventListener('loadedmetadata', () => {
        try {
          const width = this.videoElement.videoWidth
          const height = this.videoElement.videoHeight

          this.canvas = document.createElement('canvas')
          this.canvas.width = width
          this.canvas.height = height

          const ctx = this.canvas.getContext('2d')
          ctx.drawImage(this.videoElement, 0, 0, width, height)

          const imageData = this.canvas.toDataURL('image/png')

          resolve({
            imageData,
            width,
            height
          })
        } catch (error) {
          reject(error)
        }
      })

      this.videoElement.addEventListener('error', (error) => {
        reject(new Error('视频加载失败: ' + error.message))
      })

      this.videoElement.srcObject = this.mediaStream
    })
  }

  /**
   * 裁剪图片
   * @param {string} imageData - base64 图片数据
   * @param {Object} region - 裁剪区域
   * @returns {Promise<{ imageData: string, width: number, height: number }>}
   */
  async _cropImage(imageData, region) {
    return new Promise((resolve, reject) => {
      const img = new Image()
      img.onload = () => {
        try {
          const canvas = document.createElement('canvas')
          canvas.width = region.width
          canvas.height = region.height

          const ctx = canvas.getContext('2d')
          ctx.drawImage(
            img,
            region.x,
            region.y,
            region.width,
            region.height,
            0,
            0,
            region.width,
            region.height
          )

          resolve({
            imageData: canvas.toDataURL('image/png'),
            width: region.width,
            height: region.height
          })
        } catch (error) {
          reject(error)
        }
      }

      img.onerror = () => {
        reject(new Error('图片加载失败'))
      }

      img.src = imageData
    })
  }

  /**
   * 清理资源
   */
  _cleanup() {
    if (this.mediaStream) {
      this.mediaStream.getTracks().forEach(track => track.stop())
      this.mediaStream = null
    }

    if (this.videoElement) {
      this.videoElement.srcObject = null
      this.videoElement = null
    }

    if (this.canvas) {
      this.canvas = null
    }
  }
}

// 创建单例实例
export const screenCaptureService = new ScreenCaptureService()
