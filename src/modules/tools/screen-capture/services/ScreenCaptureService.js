class ScreenCaptureService {
  constructor() {
    this.isCapturing = false
    this.captureType = 'fullscreen'
    this.selectedRegion = null
    this.selectedWindow = null
  }

  setCaptureType(type) {
    this.captureType = type
  }

  getCaptureType() {
    return this.captureType
  }

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

  async captureWindow() {
    try {
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

  isValidRegion(region) {
    return (
      region.x !== undefined &&
      region.y !== undefined &&
      region.width > 0 &&
      region.height > 0
    )
  }

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

  setSelectedRegion(region) {
    this.selectedRegion = region
  }

  getSelectedRegion() {
    return this.selectedRegion
  }

  clearSelectedRegion() {
    this.selectedRegion = null
  }

  setSelectedWindow(window) {
    this.selectedWindow = window
  }

  getSelectedWindow() {
    return this.selectedWindow
  }

  clearSelectedWindow() {
    this.selectedWindow = null
  }

  isCapturingState() {
    return this.isCapturing
  }

  cancelCapture() {
    this.isCapturing = false
    this.clearSelectedRegion()
    this.clearSelectedWindow()
  }
}

const screenCaptureService = new ScreenCaptureService()

export { ScreenCaptureService, screenCaptureService }
export default screenCaptureService
