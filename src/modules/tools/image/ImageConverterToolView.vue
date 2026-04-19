<template>
  <BasePanel class="panel image-converter-panel">
    <section class="dropzone image-dropzone">
      <label class="upload-label image-upload-label">
        <input type="file" accept="image/*" @change="handleFileUpload" />
        <span class="image-upload-icon" aria-hidden="true">
          <img :src="uploadIcon" alt="" />
        </span>
        <span class="image-upload-text" v-if="!selectedFile">点击或拖入图片文件</span>
        <span class="image-upload-text" v-else>已选择：{{ selectedFile.name }}</span>
        <span class="image-upload-hint">支持 png / jpg / webp / gif</span>
      </label>
      <BaseButton v-if="selectedFile" class="image-remove-btn" @click="clearFile">移除文件</BaseButton>
    </section>
    <section v-if="selectedFile" class="preview">
      <img :src="previewUrl" alt="预览图" />
      <div class="options">
        <label>目标格式
          <BaseSelect
            v-model="targetFormat"
            :options="[
              { value: 'png', label: 'PNG' },
              { value: 'jpeg', label: 'JPEG' },
              { value: 'webp', label: 'WebP' }
            ]"
          />
        </label>
        <label>质量 {{ quality }}%<input v-model.number="quality" type="range" min="10" max="100" /></label>
        <BaseButton class="primary" variant="primary" @click="convertImage">转换并下载</BaseButton>
      </div>
    </section>
  </BasePanel>
</template>

<script>
import uploadIcon from '../../../assets/icons/upload-image.svg';

export default {
  name: 'ImageConverterToolView',
  data() {
    return { selectedFile: null, previewUrl: '', targetFormat: 'jpeg', quality: 80, uploadIcon };
  },
  methods: {
    handleFileUpload(event) {
      const file = event.target.files[0];
      if (!file) return;
      if (this.previewUrl) URL.revokeObjectURL(this.previewUrl);
      this.selectedFile = file;
      this.previewUrl = URL.createObjectURL(file);
    },
    clearFile() {
      if (this.previewUrl) URL.revokeObjectURL(this.previewUrl);
      this.selectedFile = null;
      this.previewUrl = '';
    },
    convertImage() {
      const canvas = document.createElement('canvas');
      const ctx = canvas.getContext('2d');
      const img = new Image();
      img.onload = () => {
        canvas.width = img.width;
        canvas.height = img.height;
        ctx.drawImage(img, 0, 0);
        const mimeType = this.targetFormat === 'jpeg' ? 'image/jpeg' : `image/${this.targetFormat}`;
        canvas.toBlob((blob) => {
          if (!blob) return;
          const link = document.createElement('a');
          const blobUrl = URL.createObjectURL(blob);
          link.href = blobUrl;
          link.download = `${this.selectedFile.name.replace(/\.[^.]+$/, '')}.${this.targetFormat}`;
          link.click();
          URL.revokeObjectURL(blobUrl);
        }, mimeType, this.quality / 100);
      };
      img.src = this.previewUrl;
    }
  },
  beforeUnmount() {
    if (this.previewUrl) URL.revokeObjectURL(this.previewUrl);
  }
};
</script>
