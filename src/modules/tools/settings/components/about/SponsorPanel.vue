<template>
  <div v-if="visible" class="sponsor-panel-overlay" @click="close">
    <div class="sponsor-panel" @click.stop>
      <div class="sponsor-panel-header">
        <h4>支持 MonoStudio</h4>
        <button class="close-btn" @click="close">×</button>
      </div>
      <div class="sponsor-panel-body">
        <p class="sponsor-description">虽然代码不是我写的，但盯着AI写比我自己写还累，懂的都懂（=￣ω￣=）</p>
        <div class="payment-methods">
          <div class="payment-method">
            <div class="payment-header">
              <img :src="wechatPayIcon" alt="微信支付" class="payment-icon wechat">
              <span class="payment-name">微信</span>
            </div>
            <div class="qrcode-container">
              <img :src="wechatQRCode" alt="微信支付二维码" class="qrcode">
            </div>
          </div>
          <div class="payment-method">
            <div class="payment-header">
              <img :src="alipayIcon" alt="支付宝" class="payment-icon alipay">
              <span class="payment-name">支付宝</span>
            </div>
            <div class="qrcode-container">
              <img :src="alipayQRCode" alt="支付宝二维码" class="qrcode">
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import wechatQRCode from '../../../../../assets/images/wechat_payment_code.jpg';
import alipayQRCode from '../../../../../assets/images/alipay_payment_code.jpg';
import wechatPayIcon from '../../../../../assets/icons/wechat_pay.svg';
import alipayIcon from '../../../../../assets/icons/alipay.svg';

export default {
  name: 'SponsorPanel',
  props: {
    visible: {
      type: Boolean,
      default: false
    }
  },
  emits: ['update:visible'],
  data() {
    return {
      wechatQRCode: wechatQRCode,
      alipayQRCode: alipayQRCode,
      wechatPayIcon: wechatPayIcon,
      alipayIcon: alipayIcon
    };
  },
  methods: {
    close() {
      this.$emit('update:visible', false);
    }
  }
};
</script>

<style scoped>
.payment-methods {
  user-select: none;
  -webkit-user-select: none;
}
</style>