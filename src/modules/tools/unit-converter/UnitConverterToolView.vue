<template>
  <BasePanel class="panel unit-converter-panel">
    <div class="unit-converter-container">
      <div class="converter-header">
        <h3>{{ toolName }}</h3>
      </div>
      
      <div class="converter-section">
        <div class="unit-type-selector">
          <label>单位类型</label>
          <BaseSelect v-model="selectedUnitType" :options="unitTypes" @change="resetUnits" />
        </div>
        
        <div class="conversion-form">
          <div class="input-group">
            <label>输入值</label>
            <BaseInput v-model.number="inputValue" type="number" placeholder="输入数值..." @input="convert" />
          </div>
          
          <div class="unit-group">
            <label>从</label>
            <BaseSelect v-model="fromUnit" :options="currentUnits" @change="convert" />
          </div>
          
          <div class="arrow">→</div>
          
          <div class="unit-group">
            <label>到</label>
            <BaseSelect v-model="toUnit" :options="currentUnits" @change="convert" />
          </div>
          
          <div class="result-group">
            <label>结果</label>
            <BaseInput v-model="resultValue" readonly />
          </div>
        </div>
      </div>
    </div>
  </BasePanel>
</template>

<script>
import BaseInput from '../../../components/base/BaseInput.vue';
import BaseSelect from '../../../components/base/BaseSelect.vue';

export default {
  name: 'UnitConverterToolView',
  components: {
    BaseInput,
    BaseSelect
  },
  props: {
    toolName: {
      type: String,
      default: '单位转换'
    },
    toolDescription: {
      type: String,
      default: '各种单位之间的转换'
    }
  },
  data() {
    return {
      selectedUnitType: 'length',
      inputValue: 1,
      fromUnit: 'meter',
      toUnit: 'kilometer',
      resultValue: '',
      unitTypes: [
        { label: '长度', value: 'length' },
        { label: '重量', value: 'weight' },
        { label: '温度', value: 'temperature' },
        { label: '面积', value: 'area' },
        { label: '体积', value: 'volume' }
      ],
      units: {
        length: [
          { label: '米 (m)', value: 'meter' },
          { label: '千米 (km)', value: 'kilometer' },
          { label: '厘米 (cm)', value: 'centimeter' },
          { label: '毫米 (mm)', value: 'millimeter' },
          { label: '英寸 (in)', value: 'inch' },
          { label: '英尺 (ft)', value: 'foot' }
        ],
        weight: [
          { label: '千克 (kg)', value: 'kilogram' },
          { label: '克 (g)', value: 'gram' },
          { label: '毫克 (mg)', value: 'milligram' },
          { label: '吨 (t)', value: 'ton' },
          { label: '磅 (lb)', value: 'pound' },
          { label: '盎司 (oz)', value: 'ounce' }
        ],
        temperature: [
          { label: '摄氏度 (°C)', value: 'celsius' },
          { label: '华氏度 (°F)', value: 'fahrenheit' },
          { label: '开尔文 (K)', value: 'kelvin' }
        ],
        area: [
          { label: '平方米 (m²)', value: 'square_meter' },
          { label: '平方千米 (km²)', value: 'square_kilometer' },
          { label: '平方厘米 (cm²)', value: 'square_centimeter' },
          { label: '平方英寸 (in²)', value: 'square_inch' },
          { label: '平方英尺 (ft²)', value: 'square_foot' },
          { label: '公顷 (ha)', value: 'hectare' }
        ],
        volume: [
          { label: '立方米 (m³)', value: 'cubic_meter' },
          { label: '升 (L)', value: 'liter' },
          { label: '毫升 (mL)', value: 'milliliter' },
          { label: '立方英寸 (in³)', value: 'cubic_inch' },
          { label: '立方英尺 (ft³)', value: 'cubic_foot' },
          { label: '加仑 (gal)', value: 'gallon' }
        ]
      }
    };
  },
  computed: {
    currentUnits() {
      return this.units[this.selectedUnitType] || [];
    }
  },
  mounted() {
    this.convert();
  },
  methods: {
    resetUnits() {
      const units = this.currentUnits;
      if (units.length > 0) {
        this.fromUnit = units[0].value;
        this.toUnit = units.length > 1 ? units[1].value : units[0].value;
        this.convert();
      }
    },
    convert() {
      if (isNaN(this.inputValue)) {
        this.resultValue = '';
        return;
      }
      
      let result;
      switch (this.selectedUnitType) {
        case 'length':
          result = this.convertLength();
          break;
        case 'weight':
          result = this.convertWeight();
          break;
        case 'temperature':
          result = this.convertTemperature();
          break;
        case 'area':
          result = this.convertArea();
          break;
        case 'volume':
          result = this.convertVolume();
          break;
        default:
          result = '';
      }
      
      this.resultValue = result;
    },
    convertLength() {
      const value = this.inputValue;
      let meters;
      
      // 转换为米
      switch (this.fromUnit) {
        case 'meter': meters = value; break;
        case 'kilometer': meters = value * 1000; break;
        case 'centimeter': meters = value / 100; break;
        case 'millimeter': meters = value / 1000; break;
        case 'inch': meters = value * 0.0254; break;
        case 'foot': meters = value * 0.3048; break;
        default: meters = value;
      }
      
      // 从米转换为目标单位
      switch (this.toUnit) {
        case 'meter': return meters.toFixed(4);
        case 'kilometer': return (meters / 1000).toFixed(4);
        case 'centimeter': return (meters * 100).toFixed(4);
        case 'millimeter': return (meters * 1000).toFixed(4);
        case 'inch': return (meters / 0.0254).toFixed(4);
        case 'foot': return (meters / 0.3048).toFixed(4);
        default: return meters.toFixed(4);
      }
    },
    convertWeight() {
      const value = this.inputValue;
      let kilograms;
      
      // 转换为千克
      switch (this.fromUnit) {
        case 'kilogram': kilograms = value; break;
        case 'gram': kilograms = value / 1000; break;
        case 'milligram': kilograms = value / 1000000; break;
        case 'ton': kilograms = value * 1000; break;
        case 'pound': kilograms = value * 0.453592; break;
        case 'ounce': kilograms = value * 0.0283495; break;
        default: kilograms = value;
      }
      
      // 从千克转换为目标单位
      switch (this.toUnit) {
        case 'kilogram': return kilograms.toFixed(6);
        case 'gram': return (kilograms * 1000).toFixed(4);
        case 'milligram': return (kilograms * 1000000).toFixed(2);
        case 'ton': return (kilograms / 1000).toFixed(6);
        case 'pound': return (kilograms / 0.453592).toFixed(4);
        case 'ounce': return (kilograms / 0.0283495).toFixed(4);
        default: return kilograms.toFixed(6);
      }
    },
    convertTemperature() {
      const value = this.inputValue;
      let celsius;
      
      // 转换为摄氏度
      switch (this.fromUnit) {
        case 'celsius': celsius = value; break;
        case 'fahrenheit': celsius = (value - 32) * 5/9; break;
        case 'kelvin': celsius = value - 273.15; break;
        default: celsius = value;
      }
      
      // 从摄氏度转换为目标单位
      switch (this.toUnit) {
        case 'celsius': return celsius.toFixed(2);
        case 'fahrenheit': return (celsius * 9/5 + 32).toFixed(2);
        case 'kelvin': return (celsius + 273.15).toFixed(2);
        default: return celsius.toFixed(2);
      }
    },
    convertArea() {
      const value = this.inputValue;
      let squareMeters;
      
      // 转换为平方米
      switch (this.fromUnit) {
        case 'square_meter': squareMeters = value; break;
        case 'square_kilometer': squareMeters = value * 1000000; break;
        case 'square_centimeter': squareMeters = value / 10000; break;
        case 'square_inch': squareMeters = value * 0.00064516; break;
        case 'square_foot': squareMeters = value * 0.092903; break;
        case 'hectare': squareMeters = value * 10000; break;
        default: squareMeters = value;
      }
      
      // 从平方米转换为目标单位
      switch (this.toUnit) {
        case 'square_meter': return squareMeters.toFixed(4);
        case 'square_kilometer': return (squareMeters / 1000000).toFixed(6);
        case 'square_centimeter': return (squareMeters * 10000).toFixed(4);
        case 'square_inch': return (squareMeters / 0.00064516).toFixed(4);
        case 'square_foot': return (squareMeters / 0.092903).toFixed(4);
        case 'hectare': return (squareMeters / 10000).toFixed(6);
        default: return squareMeters.toFixed(4);
      }
    },
    convertVolume() {
      const value = this.inputValue;
      let cubicMeters;
      
      // 转换为立方米
      switch (this.fromUnit) {
        case 'cubic_meter': cubicMeters = value; break;
        case 'liter': cubicMeters = value / 1000; break;
        case 'milliliter': cubicMeters = value / 1000000; break;
        case 'cubic_inch': cubicMeters = value * 0.0000163871; break;
        case 'cubic_foot': cubicMeters = value * 0.0283168; break;
        case 'gallon': cubicMeters = value * 0.00378541; break;
        default: cubicMeters = value;
      }
      
      // 从立方米转换为目标单位
      switch (this.toUnit) {
        case 'cubic_meter': return cubicMeters.toFixed(6);
        case 'liter': return (cubicMeters * 1000).toFixed(4);
        case 'milliliter': return (cubicMeters * 1000000).toFixed(2);
        case 'cubic_inch': return (cubicMeters / 0.0000163871).toFixed(4);
        case 'cubic_foot': return (cubicMeters / 0.0283168).toFixed(4);
        case 'gallon': return (cubicMeters / 0.00378541).toFixed(4);
        default: return cubicMeters.toFixed(6);
      }
    }
  }
};
</script>

<style scoped>
.unit-converter-panel {
  height: 100%;
}

.unit-converter-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
}

.converter-header h3 {
  margin: 0 0 20px 0;
  font-size: 18px;
  font-weight: 600;
}

.converter-section {
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: 20px;
}

.unit-type-selector label {
  display: block;
  margin-bottom: 8px;
  font-weight: 500;
  color: var(--text-primary);
}

.conversion-form {
  display: flex;
  flex-direction: column;
  gap: 12px;
  background-color: var(--bg-secondary);
  padding: 20px;
  border-radius: var(--radius-md);
}

.input-group,
.unit-group,
.result-group {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.input-group label,
.unit-group label,
.result-group label {
  font-size: 14px;
  font-weight: 500;
  color: var(--text-primary);
}

.arrow {
  text-align: center;
  font-size: 20px;
  color: var(--text-secondary);
  margin: 10px 0;
}
</style>